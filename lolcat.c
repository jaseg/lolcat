#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))
char codes[] = {39,38,44,43,49,48,84,83,119,118,154,148,184,178,214,208,209,203,204,198,199,163,164,128,129,93,99,63,69,33};

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
				fprintf(stderr, "parsing arg %s\n", argv[i]);
				freq_h = strtod(argv[i], &endptr);
				if(*endptr)
					usage();
				fprintf(stderr, "parsed -h: %lF\n", freq_h);
			}else{
				usage();
			}
		}else if(!strcmp(argv[i], "-v")){
			if((++i)<argc){
				freq_v = strtod(argv[i], &endptr);
				if(*endptr)
					usage();
				fprintf(stderr, "parsed -v: %lF\n", freq_v);
			}else{
				usage();
			}
		}else{
			if(!strcmp(argv[i], "--"))
				i++;
			fprintf(stderr, "ending option parsing\n");
			break;
		}
	}

	fprintf(stderr, "commencing input parsing...\n");
	char **inputs = argv+i;
	char **inputs_end = argv+argc;
	if(inputs == inputs_end){
		char *foo[] = {"-"};
		inputs = foo;
		inputs_end = inputs+1;
	}

	i=0;
	for(char **filename=inputs; filename<inputs_end; filename++){
		FILE *f = stdin;
		fprintf(stderr, "opening input \"%s\"\n", *filename);
		if(strcmp(*filename, "-"))
			f = fopen(*filename, "r");

		fprintf(stderr, "reading data...\n");

		while((c = fgetwc(f)) > 0){
			if(c == '\n'){
				l++;
				i = 0;
			}else if(isprint(c)){
				int ncc = (int)((i++)*freq_h + l*freq_v);
				if(ncc != cc)
					printf("\033[38;5;%hhum", codes[(cc = ncc) % ARRAY_SIZE(codes)]);
			}
			printf("%lc", c);
		}
		if(c != WEOF){
			fprintf(stderr, "Error reading input file \"%s\": %s (%d)\n", *filename, strerror(errno), errno);
			return 2;
		}

		fclose(f);
	}
}
