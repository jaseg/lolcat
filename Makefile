
CC ?= gcc
LOLCAT_SRC ?= lolcat.c
CENSOR_SRC ?= censor.c
CFLAGS ?= -std=c11 -Wall -Wextra -g

DESTDIR ?= /usr/local/bin

all: lolcat censor

.PHONY: install clean

lolcat: $(LOLCAT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

censor: $(CENSOR_SRC)
	$(CC) $(CFLAGS) -o $@ $^

install: lolcat censor
	install lolcat $(DESTDIR)/lolcat
	install censor $(DESTDIR)/censor

clean:
	rm -f lolcat censor

