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
#include <unistd.h>
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
                        "        --color_offset <d>, -o <d>: Start with a different color\n"
                        "                       --24bit, -b: Output in 24-bit \"true\" RGB mode (slower and\n"
                        "                                    not supported by all terminals)\n"
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

static void find_escape_sequences(wint_t c, int* state)
{
    if (c == '\033') { /* Escape sequence YAY */
        *state = 1;
    } else if (*state == 1) {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
            *state = 2;
    } else {
        *state = 0;
    }
}

static void usage(void)
{
    wprintf(L"Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n");
    exit(1);
}

static void version(void)
{
    wprintf(L"lolcat version 1.2, (c) 2020 jaseg\n");
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
    int start_color = 0;
    int rgb = 0;
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
        } else if (!strcmp(argv[i], "--version")) {
            version();
        } else {
            if (!strcmp(argv[i], "--"))
                i++;
            break;
        }
    }

    int rand_offset = 0;
    if (random) {
        srand(time(NULL));
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
        int escape_state = 0;

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
                find_escape_sequences(c, &escape_state);

                if (!escape_state) {
                    if (c == '\n') {
                        l++;
                        i = 0;

                    } else {
                        if (rgb) {
                            i += wcwidth(c);
                            float theta = i * freq_h / 5.0f + l * freq_v + (offx + 2.0f * (rand_offset + start_color) / RAND_MAX) * M_PI;
                            float offset = 0.1;

                            uint8_t red   = lrintf((offset + (1.0f - offset) * (0.5f + 0.5f * sin(theta + 0            ))) * 255.0f);
                            uint8_t green = lrintf((offset + (1.0f - offset) * (0.5f + 0.5f * sin(theta + 2 * M_PI / 3 ))) * 255.0f);
                            uint8_t blue  = lrintf((offset + (1.0f - offset) * (0.5f + 0.5f * sin(theta + 4 * M_PI / 3 ))) * 255.0f);
                            wprintf(L"\033[38;2;%d;%d;%dm", red, green, blue);

                        } else {
                            int ncc = offx * ARRAY_SIZE(codes) + (int)((i += wcwidth(c)) * freq_h + l * freq_v);
                            if (cc != ncc)
                                wprintf(L"\033[38;5;%hhum", codes[(rand_offset + start_color + (cc = ncc)) % ARRAY_SIZE(codes)]);
                        }
                    }
                }
            }

            putwchar(c);

            if (escape_state == 2) /* implies "colors" */
                wprintf(L"\033[38;5;%hhum", codes[(rand_offset + start_color + cc) % ARRAY_SIZE(codes)]);
        }

        if (colors)
            wprintf(L"\033[0m");

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
