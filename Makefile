
CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O3 -Wno-sign-compare
LIBS := -lm

DESTDIR ?= /usr/local/bin

all: lolcat censor

debug: CFLAGS += -g
debug: all

.PHONY: install clean debug

xterm256lut.h: xterm256lut_gen.py
	python $< > $@

lolcat: lolcat.c xterm256lut.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

censor: censor.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

install: lolcat censor
	install lolcat $(DESTDIR)/lolcat
	install censor $(DESTDIR)/censor

clean:
	rm -f lolcat censor
