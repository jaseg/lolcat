
all: lolcat

lolcat: lolcat.c fgetwc_fix.c
	gcc -g -std=c99 -Wall -o lolcat lolcat.c fgetwc_fix.c
#	gcc -g -std=c99 -Wall -nostdinc -nostdlib -I/usr/x86_64-linux-uclibc/usr/include -I/usr/lib/gcc/x86_64-unknown-linux-gnu/4.9.1/include -I/usr/include -L/usr/x86_64-linux-uclibc/usr/lib /usr/x86_64-linux-uclibc/usr/lib/crt1.o -lc -lrt -o lolcat lolcat.c
