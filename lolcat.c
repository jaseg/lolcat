/* Copyright (C) 2014 jaseg <github@jaseg.net>
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

#define _GNU_SOURCE //for fmemopen

#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <sys/time.h>


static char helpstr[] = "\n"
"Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n"
"\n"
"Concatenate FILE(s), or standard input, to standard output.\n"
"With no FILE, or when FILE is -, read standard input.\n"
"\n"
"              -h <d>:   Horizontal rainbow frequency (default: 0.23)\n"
"              -v <d>:   Vertical rainbow frequency (default: 0.1)\n"
"                  -f:   Force color even when stdout is not a tty\n"
"           --version:   Print version and exit\n"
"              --help:   Show this message\n"
"\n"
"Examples:\n"
"  lolcat f - g      Output f's contents, then stdin, then g's contents.\n"
"  lolcat            Copy standard input to standard output.\n"
"  fortune | lolcat  Display a rainbow cookie.\n"
"\n"
"Report lolcat bugs to <http://www.github.org/jaseg/lolcat/issues>\n"
"lolcat home page: <http://www.github.org/jaseg/lolcat/>\n"
"Original idea: <http://www.github.org/busyloop/lolcat/>\n";

#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))
const char codes[] = {39,38,44,43,49,48,84,83,119,118,154,148,184,178,214,208,209,203,204,198,199,163,164,128,129,93,99,63,69,33};

void find_escape_sequences(int c, int *state){
    if(c == '\033'){ /* Escape sequence YAY */
        *state = 1;
    }else if(*state == 1){
        if(('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
            *state = 2;
    }else{
        *state = 0;
    }
}


void usage(){
    printf("Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n");
    exit(1);
}

void version(){
    printf("lolcat version 0.1, (c) 2014 jaseg\n");
    exit(0);
}

int main(int argc, char **argv){
    int c, cc=-1, i, l=0;
    int colors=1;
    double freq_h = 0.23, freq_v = 0.1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    double offx = (tv.tv_sec%300)/300.0;

    for(i=1;i<argc;i++){
        char *endptr;
        if(!strcmp(argv[i], "-h")){
            if((++i)<argc){
                freq_h = strtod(argv[i], &endptr);
                if(*endptr)
                    usage();
            }else{
                usage();
            }
        }else if(!strcmp(argv[i], "-v")){
            if((++i)<argc){
                freq_v = strtod(argv[i], &endptr);
                if(*endptr)
                    usage();
            }else{
                usage();
            }
        }else if(!strcmp(argv[i], "-f")){
            colors = 1;
        }else if(!strcmp(argv[i], "--version")){
            version();
        }else{
            if(!strcmp(argv[i], "--"))
                i++;
            break;
        }
    }

    char **inputs = argv+i;
    char **inputs_end = argv+argc;
    if(inputs == inputs_end){
        char *foo[] = {"-"};
        inputs = foo;
        inputs_end = inputs+1;
    }

    setlocale(LC_ALL, "");

    i=0;
    for(char **filename=inputs; filename<inputs_end; filename++){
        FILE *f = stdin;
        int escape_state = 0;

        if(!strcmp(*filename, "--help"))
            f = fmemopen(helpstr, strlen(helpstr), "r");
        else if(strcmp(*filename, "-"))
            f = fopen(*filename, "r");
        
        if(!f){
            fprintf(stderr, "Cannot open input file \"%s\": %s\n", *filename, strerror(errno));
            return 2;
        } 

        while((c = fgetwc(f)) > 0){
            if(colors){
                find_escape_sequences(c, &escape_state);

                if(!escape_state){
                    if(c == '\n'){
                        l++;
                        i = 0;
                    }else{
                        int ncc = offx*ARRAY_SIZE(codes) + (int)((i+=wcwidth(c))*freq_h + l*freq_v);
                        if(cc != ncc)
                            printf("\033[38;5;%hhum", codes[(cc = ncc) % ARRAY_SIZE(codes)]);
                    }
                }
            }

            printf("%lc", c);

            if(escape_state == 2)
                printf("\033[38;5;%hhum", codes[cc % ARRAY_SIZE(codes)]);
        }
        printf("\n\033[0m");
        cc = -1;

        fclose(f);

        if(c != WEOF && c != 0){
            fprintf(stderr, "Error reading input file \"%s\": %s\n", *filename, strerror(errno));
            return 2;
        }
    }
}
