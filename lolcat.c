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
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>

static char helpstr[] = "\n"
"Usage: lolcat [-h horizontal_speed] [-v vertical_speed] [--] [FILES...]\n"
"\n"
"Concatenate FILE(s), or standard input, to standard output.\n"
"With no FILE, or when FILE is -, read standard input.\n"
"\n"
"              -h <d>:   Horizontal rainbow frequency (default: 0.01)\n"
"              -v <d>:   Vertical rainbow frequency (default: 0.005)\n"
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
const char *codes_10b[] = {
	"\033[38;5;93m",
	"\033[38;5;99m",
	"\033[38;5;63m",
	"\033[38;5;69m",
	"\033[38;5;33m",
	"\033[38;5;39m",
	"\033[38;5;38m",
	"\033[38;5;44m",
	"\033[38;5;43m",
	"\033[38;5;49m",
	"\033[38;5;48m",
	"\033[38;5;84m",
	"\033[38;5;83m",
	"\033[38;5;119m", /* begin of 11 byte sequences */
	"\033[38;5;118m",
	"\033[38;5;154m",
	"\033[38;5;148m",
	"\033[38;5;184m",
	"\033[38;5;178m",
	"\033[38;5;214m",
	"\033[38;5;208m",
	"\033[38;5;209m",
	"\033[38;5;203m",
	"\033[38;5;204m",
	"\033[38;5;198m",
	"\033[38;5;199m",
	"\033[38;5;163m",
	"\033[38;5;164m",
	"\033[38;5;128m",
	"\033[38;5;129m"};
const char **codes_11b = codes_10b+12;
#define N_SEQS (ARRAY_SIZE(codes_10b))

static inline void write_seq(size_t idx){
	const char **p = codes_10b + (idx%N_SEQS);
	write(1, *p, 10+(p>=codes_11b));
}

char *find_escape_sequences(char *c, char *end){
	if(*c == '\033') /* Escape sequence YAY */
		for(char *p=c+1; p<end; p++)
			if(('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z'))
				return p+1;
	return c;
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
	int colors=(isatty(1) == 1);
	double freq_h = 0.01, freq_v = 0.005;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	double offx = (tv.tv_sec%300)/300.0;
	size_t i;

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
	size_t l=0;
	for(char **filename=inputs; filename<inputs_end; filename++){
		int fd = 0;
		char *fdata;
		char *fdata_end;

		/* FIXME --help and - */
		if(!strcmp(*filename, "--help")){
			fdata = helpstr;
			fdata_end = helpstr+strlen(helpstr);
		}else{
			fd = open(*filename, O_RDONLY);
			if(fd<0){
				fprintf(stderr, "Cannot open input file \"%s\": %s\n", *filename, strerror(errno));
				return 2;
			}
			
			struct stat statinfo;
			if(fstat(fd, &statinfo) == -1){
				close(fd);
				return 2;
			}

			fdata = mmap(NULL, statinfo.st_size, PROT_READ, MAP_SHARED, fd, 0);
			fdata_end = fdata+statinfo.st_size;

			if(fdata == MAP_FAILED){
				fprintf(stderr, "Cannot open input file \"%s\": %s\n", *filename, strerror(errno));
				close(fd);
				return 2;
			} 
		}
		
		char *start = fdata;
		mbstate_t mbst;
		memset(&mbst, 0, sizeof(mbst));
		size_t ncc = 0;
		size_t next_i = (size_t)(1/freq_h);
		char *p;
		char *np;

		write_seq(ncc);
		for(p=fdata; p < fdata_end;){

			size_t res;

			if(*p == '\n'){
				p++;
				l++;
				i = 0;
				
				write(1, start, p-start);
				start = p;

				double ncc_d = (offx + l*freq_v)*N_SEQS;
				ncc = (size_t)ncc_d;
				next_i = i + (size_t)((ncc_d-ncc+1.0)/freq_h/N_SEQS);
				write_seq(ncc);

			}else if((np = find_escape_sequences(p, fdata_end)) != p){
				p = np;
				write(1, start, p-start);
				start = p;
				write_seq(ncc);
			}else if((res = mbrtowc(NULL, p, fdata_end-p, &mbst)) > 0){
				p += res;
				i++;
				if(i >= next_i){
					write(1, start, p-start);
					start = p;

					double ncc_d = (offx + i*freq_h + l*freq_v)*N_SEQS;
					ncc = (size_t)ncc_d;
					next_i = i + (size_t)((ncc_d-ncc+1.0)/freq_h/N_SEQS);
					write_seq(ncc);
				}
			}else{
				fprintf(stderr, "Error reading input file \"%s\": %s\n", *filename, strerror(errno));
				if(fd)
					close(fd);
				return 2;
			}
		}

		write(1, start, p-start);
		printf("\033[0m\n");
		l = 0;

		if(fd)
			close(fd);
	}
}
