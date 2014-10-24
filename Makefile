
all: lolcat

lolcat: lolcat.c
	gcc -std=c99 -Wall -o lolcat lolcat.c
