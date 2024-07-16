
CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -O3 -Wno-sign-compare
LIBS := -lm

PREFIX ?= /usr/local

all: lolcat censor

debug: CFLAGS += -g
debug: all

.PHONY: install clean debug

xterm256lut.h: xterm256lut_gen.py
	python $< > $@

lolcat: lolcat.c xterm256lut.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

censor: censor.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

install: lolcat censor
	install lolcat $(DESTDIR)$(PREFIX)/bin/lolcat
	install censor $(DESTDIR)$(PREFIX)/bin/censor

clean:
	rm -f lolcat censor
