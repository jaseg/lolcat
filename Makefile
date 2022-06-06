CC ?= gcc
LOLCAT_SRC ?= lolcat.c
CENSOR_SRC ?= censor.c
CFLAGS ?= -std=c11 -Wall -Wextra -O3 -Wno-sign-compare
LIBS := -lm

DESTDIR ?= /usr/local/bin

all: lolcat censor

debug: CFLAGS += -g
debug: all

.PHONY: install clean debug

lolcat: $(LOLCAT_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

censor: $(CENSOR_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

install: lolcat censor
	install lolcat $(DESTDIR)/lolcat
	install censor $(DESTDIR)/censor

uninstall: lolcat censor
	rm -f $(DESTDIR)/lolcat
	rm -f $(DESTDIR)/censor

clean:
	rm -f lolcat censor
