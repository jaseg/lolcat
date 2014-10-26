
all: lolcat

lolcat: lolcat.c
	gcc -g -std=c99 -Wall -o lolcat lolcat.c

install: lolcat
	install lolcat /usr/local/bin

.PHONY: install all

