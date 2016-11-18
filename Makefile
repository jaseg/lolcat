
LOLCAT_SRC := lolcat.c
CENSOR_SRC := censor.c
CFLAGS := -std=c11 -Wall -g

DESTDIR := /usr/local/bin

ifeq ($(shell uname -s),Darwin)
	LOLCAT_SRC += memorymapping/src/fmemopen.c
	CENSOR_SRC += memorymapping/src/fmemopen.c
	CFLAGS += -Imemorymapping/src
endif

all: lolcat-static censor-static

.PHONY: install clean musl static

musl/lib/libc.a musl/lib/crt1.o:
	cd /usr/lib/musl; ./configure
	make -C musl

musl: musl/lib/libc.a musl/lib/crt1.o

static: lolcat-static censor-static

lolcat-static: lolcat.c musl
	gcc -c $(CFLAGS) -Imusl/include -o lolcat.o $<
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o $@ lolcat.o musl/lib/crt1.o musl/lib/libc.a

censor-static: censor.c musl
	gcc -c $(CFLAGS) -Imusl/include -o censor.o $<
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o $@ censor.o musl/lib/crt1.o musl/lib/libc.a

lolcat: $(LOLCAT_SRC)
	gcc $(CFLAGS) -o $@ $^

censor: $(CENSOR_SRC)
	gcc $(CFLAGS) -o $@ $^

install: lolcat censor
	install lolcat-static $(DESTDIR)/lolcat
	install censor-static $(DESTDIR)/censor

clean:
	rm -f lolcat lolcat-static.o lolcat-static censor censor-static.o censor-static
	make -C musl clean

