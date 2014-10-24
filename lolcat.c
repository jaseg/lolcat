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

#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))
char codes[] = {39,38,44,43,49,48,84,83,119,118,154,148,184,178,214,208,209,203,204,198,199,163,164,128,129,93,99,63,69,33};

/* CAUTION! this function uses a function-static variable! */
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

int main(int argc, char **argv){
	int c, cc=-1, i, l=0;
	double freq_h = 0.23, freq_v = 0.1;

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

		if(strcmp(*filename, "-"))
			f = fopen(*filename, "r");

		while((c = fgetwc(f)) > 0){
			find_escape_sequences(c, &escape_state);

			if(!escape_state){
				if(c == '\n'){
					l++;
					i = 0;
				}else if(!iscntrl(c)){
					int ncc = (int)((i++)*freq_h + l*freq_v);
					if(cc != ncc)
						printf("\033[38;5;%hhum", codes[(cc = ncc) % ARRAY_SIZE(codes)]);
				}
			}

			printf("%lc", c);

			if(escape_state == 2)
				printf("\033[38;5;%hhum", codes[cc % ARRAY_SIZE(codes)]);
		}

		if(c != WEOF){
			fprintf(stderr, "Error reading input file \"%s\": %s (%d)\n", *filename, strerror(errno), errno);
			return 2;
		}

		fclose(f);
	}
}
