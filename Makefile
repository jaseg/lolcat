
all: lolcat

lolcat: lolcat.c fgetwc_fix.c
	gcc -g -std=c99 -Wall -o lolcat lolcat.c fgetwc_fix.c

install: lolcat
	install lolcat /usr/local/bin

.PHONY: install all

