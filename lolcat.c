/* Copyright (C) 2020 jaseg <github@jaseg.net>
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * Version 2, December 2004
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#define _XOPEN_SOURCE

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/param.h>
#include <unistd.h>
#include <limits.h>
#include <wchar.h>
#include <time.h>
#include "math.h"

static char helpstr[] = "\n"
                        "Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n"
                        "\n"
                        "Concatenate FILE(s), or standard input, to standard output.\n"
                        "With no FILE, or when FILE is -, read standard input.\n"
                        "\n"
                        "--horizontal-frequency <d>, -h <d>: Horizontal rainbow frequency (default: 0.23)\n"
                        "  --vertical-frequency <d>, -v <d>: Vertical rainbow frequency (default: 0.1)\n"
                        "                 --force-color, -f: Force color even when stdout is not a tty\n"
                        "             --no-force-locale, -l: Use encoding from system locale instead of\n"
                        "                                    assuming UTF-8\n"
                        "                      --random, -r: Random colors\n"
                        "                --seed <d>, -s <d>: Random colors based on given seed,\n"
                        "                                    implies --random\n"
                        "        --color_offset <d>, -o <d>: Start with a different color\n"
                        "            --gradient <g>, -g <g>: Use color gradient from given start to end color,\n"
                        "                                    format: -g ff4444:00ffff\n"
                        "                       --24bit, -b: Output in 24-bit \"true\" RGB mode (slower and\n"
                        "                                    not supported by all terminals)\n"
                        "                     --16color, -x: Output in 16-color mode for basic terminals\n"
                        "                      --invert, -i: Invert foreground and background\n"
                        "                         --version: Print version and exit\n"
                        "                            --help: Show this message\n"
                        "\n"
                        "Examples:\n"
                        "  lolcat f - g      Output f's contents, then stdin, then g's contents.\n"
                        "  lolcat            Copy standard input to standard output.\n"
                        "  fortune | lolcat  Display a rainbow cookie.\n"
                        "\n"
                        "Report lolcat bugs to <https://github.com/jaseg/lolcat/issues>\n"
                        "lolcat home page: <https://github.com/jaseg/lolcat/>\n"
                        "Original idea: <https://github.com/busyloop/lolcat/>\n";

#define ARRAY_SIZE(foo) (sizeof(foo) / sizeof(foo[0]))
const unsigned char codes[] = { 39, 38, 44, 43, 49, 48, 84, 83, 119, 118, 154, 148, 184, 178, 214, 208, 209, 203, 204, 198, 199, 163, 164, 128, 129, 93, 99, 63, 69, 33 };
const unsigned char codes16[] = {31, 33, 32, 36, 34, 35, 95, 94, 96, 92, 93, 91};
unsigned int codes_gradient[128];

union rgb_c {
    struct {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    };
    unsigned int i;
};

#include "xterm256lut.h"

int xterm256lookup(union rgb_c *in) {
    size_t min_i;
    int min_v = INT_MAX;
    for (size_t i=0; i<ARRAY_SIZE(xterm256lut); i++) {
        int dr = in->r - xterm256lut[i].r;
        int dg = in->g - xterm256lut[i].g;
        int db = in->b - xterm256lut[i].b;
        int d = dr*dr + dg*dg + db*db;
        if (d < min_v) {
            min_v = d;
            min_i = i;
        }
    }
    return 16 + min_i;
}

void rgb_interpolate(union rgb_c *start, union rgb_c *end, union rgb_c *out, double f) {
    out->r = start->r + (end->r - start->r)*f;
    out->g = start->g + (end->g - start->g)*f;
    out->b = start->b + (end->b - start->b)*f;
}

enum esc_st {
    ST_NONE=0,
    ST_ESC_BEGIN,
    ST_ESC_STRING,
    ST_ESC_CSI,
    ST_ESC_STRING_TERM,
    ST_ESC_CSI_TERM,
    ST_ESC_TERM,
    NUM_ST
};

const char * esc_st_names[NUM_ST] = {
    [ST_NONE]               = "NONE",
    [ST_ESC_BEGIN]          = "BEGIN",
    [ST_ESC_STRING]         = "STRING",
    [ST_ESC_CSI]            = "CSI",
    [ST_ESC_STRING_TERM]    = "STRING_TERM",
    [ST_ESC_CSI_TERM]       = "CSI_TERM",
    [ST_ESC_TERM]           = "TERM",
};

static enum esc_st find_escape_sequences(wint_t c, enum esc_st st)
{

    if (st == ST_NONE || st == ST_ESC_CSI_TERM) {
        if (c == '\033') { /* Escape sequence YAY */
            return ST_ESC_BEGIN;
        } else {
            return ST_NONE;
        }

    } else if (st == ST_ESC_BEGIN) {
        if (c == '[') {
            return ST_ESC_CSI;
        } else if (c == 'P' || c == ']' || c == 'X' || c == '^' || c == '_') {
            return ST_ESC_STRING;
        } else {
            return ST_ESC_TERM;
        }

    } else if (st == ST_ESC_CSI) {
        if (0x40 <= c && c <= 0x7e) {
            return ST_ESC_CSI_TERM;
        } else {
            return st;
        }

    } else if (st == ST_ESC_STRING) {
        if (c == '\007') {
            return ST_NONE;
        } else if (c == '\033') {
            return ST_ESC_STRING_TERM;
        } else {
            return st;
        }

    } else if (st == ST_ESC_STRING_TERM) {
        if (c == '\\') {
            return ST_NONE;
        } else {
            return ST_ESC_STRING;
        }

    } else {
        return ST_NONE;
    }
}

static void usage(void)
{
    wprintf(L"Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n");
    wprintf(L"\n");
    wprintf(L"Use lolcat --help to print the full help text.\n");
    exit(2);
}

static void version(void)
{
    wprintf(L"lolcat version 1.4, (c) 2023 jaseg\n");
    exit(0);
}

static wint_t helpstr_hack(FILE * _ignored)
{
    (void)_ignored;
    static size_t idx = 0;
    char c = helpstr[idx++];
    if (c)
        return c;
    idx = 0;
    return WEOF;
}

int main(int argc, char** argv)
{
    char* default_argv[] = { "-" };
    int cc = -1, i, l = 0;
    wint_t c;
    int colors    = isatty(STDOUT_FILENO);
    int force_locale = 1;
    int random = 0;
    unsigned int seed = time(NULL);
    int start_color = 0;
    int rgb = 0;
    int ansi16 = 0;
    int invert = 0;
    int gradient = 0;
    union rgb_c rgb_start, rgb_end;
    double freq_h = 0.23, freq_v = 0.1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double offx = (tv.tv_sec % 300) / 300.0;

    for (i = 1; i < argc; i++) {
        char* endptr;
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--horizontal-frequency")) {
            if ((++i) < argc) {
                freq_h = strtod(argv[i], &endptr);
                if (*endptr)
                    usage();
            } else {
                usage();
            }
        } else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--vertical-frequency")) {
            if ((++i) < argc) {
                freq_v = strtod(argv[i], &endptr);
                if (*endptr)
                    usage();
            } else {
                usage();
            }
        } else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--force-color")) {
            colors = 1;
        } else if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--no-force-locale")) {
            force_locale = 0;
        } else if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--random")) {
            random = 1;
        } else if (!strcmp(argv[i], "-S") || !strcmp(argv[i], "--seed")) {
            random = 1;
            if ((++i) < argc) {
                seed = strtoul(argv[i], &endptr, 10);
                if (*endptr)
                    usage();
            } else {
                usage();
            }
        } else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--color_offset")) {
            if ((++i) < argc) {
                start_color = strtod(argv[i], &endptr);
                if (*endptr)
                    usage();
            } else {
                usage();
            }
        } else if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--24bit")) {
            rgb = 1;
        } else if (!strcmp(argv[i], "-x") || !strcmp(argv[i], "--16color")) {
            ansi16 = 1;
        } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--invert")) {
            invert = 1;
        } else if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--gradient")) {
            if ((++i) >= argc) {
                usage();
            }

            if (strlen(argv[i]) != 6+1+6 || argv[i][6] != ':') {
                wprintf(L"Invalid format for --gradient\n");
                usage();
            }

            argv[i][6] = '\0';
            rgb_start.i = strtoul(argv[i], &endptr, 16);
            if (*endptr) {
                wprintf(L"Invalid format for --gradient\n");
                usage();
            }

            rgb_end.i = strtoul(argv[i]+6+1, &endptr, 16);
            if (*endptr) {
                wprintf(L"Invalid format for --gradient\n");
                usage();
            }

            gradient = 1;
        } else if (!strcmp(argv[i], "--version")) {
            version();
        } else {
            if (!strcmp(argv[i], "--"))
                i++;
            break;
        }
    }

    if (rgb && ansi16) {
        wprintf(L"Only one of --24bit and --16color can be given at a time\n");
        usage();
    }

    if (gradient) {
        if (ansi16) {
            wprintf(L"--gradient and --16color are mutually exclusive\n");
            usage();
        }

        if (!rgb) {
            double correction_factor =(2*ARRAY_SIZE(codes_gradient)) / (double)ARRAY_SIZE(codes);
            freq_h *= correction_factor;
            freq_v *= correction_factor;
            for (size_t i=0; i<ARRAY_SIZE(codes_gradient); i++) {
                double f = i / (double)(ARRAY_SIZE(codes_gradient) - 1);
                union rgb_c rgb_intermediate;
                rgb_interpolate(&rgb_start, &rgb_end, &rgb_intermediate, f);
                codes_gradient[i] = xterm256lookup(&rgb_intermediate);
            }
        }
    }

    if (invert) {
        if (ansi16) {
            wprintf(L"\033[30m\n");
        } else {
            wprintf(L"\033[38;5;16m\n");
        }
    }

    int rand_offset = 0;
    if (random) {
        srand(seed);
        rand_offset = rand();
    }

    char** inputs = argv + i;
    char** inputs_end = argv + argc;
    if (inputs == inputs_end) {
        inputs = default_argv;
        inputs_end = inputs + 1;
    }

    char* env_lang = getenv("LANG");
    if (force_locale && env_lang && !strstr(env_lang, "UTF-8")) {
        if (!setlocale(LC_ALL, "C.UTF-8")) { /* C.UTF-8 may not be available on all platforms */
            setlocale(LC_ALL, ""); /* Let's hope for the best */
        }
    } else {
        setlocale(LC_ALL, "");
    }

    i = 0;
    for (char** filename = inputs; filename < inputs_end; filename++) {
        wint_t (*this_file_read_wchar)(FILE*); /* Used for --help because fmemopen is universally broken when used with fgetwc */
        FILE* f;
        int escape_state = ST_NONE;

        if (!strcmp(*filename, "--help")) {
            this_file_read_wchar = &helpstr_hack;
            f = 0;

        } else if (!strcmp(*filename, "-")) {
            this_file_read_wchar = &fgetwc;
            f = stdin;

        } else {
            this_file_read_wchar = &fgetwc;
            f = fopen(*filename, "r");
            if (!f) {
                fwprintf(stderr, L"Cannot open input file \"%s\": %s\n", *filename, strerror(errno));
                return 2;
            }
        }

        while ((c = this_file_read_wchar(f)) != WEOF) {
            if (colors) {


                escape_state = find_escape_sequences(c, escape_state);
#ifdef ESC_DEBUG
                fprintf(stderr, "%02x %c %s\n", c, c > 32 ? c : '.', esc_st_names[escape_state]);
#endif

                if (escape_state == ST_ESC_CSI_TERM) {
                    putwchar(c);
                }

                if (escape_state == ST_NONE || escape_state == ST_ESC_CSI_TERM) {
                    if (c == '\n') {
                        l++;
                        i = 0;
                        if (invert) {
                            wprintf(L"\033[49m");
                        }

                    } else {
                        if (escape_state == ST_NONE) {
                            i += wcwidth(c);
                        }

                        if (rgb) {
                            float theta = i * freq_h / 5.0f + l * freq_v + (offx + 2.0f * (rand_offset + start_color) / RAND_MAX)*M_PI;

                            union rgb_c c;
                            if (gradient) {
                                theta = fmodf(theta/2.0/M_PI, 2.0f);
                                if (theta > 1.0f) {
                                    theta = 2.0f - theta;
                                }
                                rgb_interpolate(&rgb_start, &rgb_end, &c, theta);
                            } else {
                                float offset = 0.1;
                                c.r = lrintf((offset + (1.0f - offset) * (0.5f + 0.5f * sin(theta + 0            ))) * 255.0f);
                                c.g = lrintf((offset + (1.0f - offset) * (0.5f + 0.5f * sin(theta + 2 * M_PI / 3 ))) * 255.0f);
                                c.b = lrintf((offset + (1.0f - offset) * (0.5f + 0.5f * sin(theta + 4 * M_PI / 3 ))) * 255.0f);
                            }
                            wprintf(L"\033[%d;2;%d;%d;%dm", (invert ? 48 : 38), c.r, c.g, c.b);

                        } else if (ansi16) {
                            int ncc = offx * ARRAY_SIZE(codes16) + (int)(i * freq_h + l * freq_v);
                            if (cc != ncc || escape_state == ST_ESC_CSI_TERM) {
                                wprintf(L"\033[%hhum", (invert ? 10 : 0) + codes16[(rand_offset + start_color + (cc = ncc)) % ARRAY_SIZE(codes16)]);
                            }

                        } else {
                            if (gradient) {
                                int ncc = offx * ARRAY_SIZE(codes_gradient) + (int)(i * freq_h + l * freq_v);
                                if (cc != ncc || escape_state == ST_ESC_CSI_TERM) {
                                    size_t lookup = (rand_offset + start_color + (cc = ncc)) % (2*ARRAY_SIZE(codes_gradient));
                                    if (lookup >= ARRAY_SIZE(codes_gradient)) {
                                        lookup = 2*ARRAY_SIZE(codes_gradient) - 1 - lookup;
                                    }
                                    wprintf(L"\033[%d;5;%hhum", (invert ? 48 : 38), codes_gradient[lookup]);
                                }

                            } else {
                                int ncc = offx * ARRAY_SIZE(codes) + (int)(i * freq_h + l * freq_v);
                                if (cc != ncc || escape_state == ST_ESC_CSI_TERM) {
                                    wprintf(L"\033[%d;5;%hhum", (invert ? 48 : 38), codes[(rand_offset + start_color + (cc = ncc)) % ARRAY_SIZE(codes)]);
                                }
                            }
                        }
                    }
                }
            }

            if (escape_state != ST_ESC_CSI_TERM) {
                putwchar(c);
            }
        }

        if (colors) {
            wprintf(L"\033[0m");
        }

        cc = -1;

        if (f) {
            if (ferror(f)) {
                fwprintf(stderr, L"Error reading input file \"%s\": %s\n", *filename, strerror(errno));
                fclose(f);
                return 2;
            }

            if (fclose(f)) {
                fwprintf(stderr, L"Error closing input file \"%s\": %s\n", *filename, strerror(errno));
                return 2;
            }
        }
    }
}
