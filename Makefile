
all: lolcat censor

.PHONY: install clean musl static

musl/lib/libc.a musl/lib/crt1.o:
	cd musl; ./configure
	make -C musl

musl: musl/lib/libc.a musl/lib/crt1.o

static: lolcat-static censor-static

lolcat-static: lolcat.c musl
	gcc -c -std=c11 -Wall -Imusl/include -o lolcat.o $<
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o $@ lolcat.o musl/lib/crt1.o musl/lib/libc.a

censor-static: censor.c musl
	gcc -c -std=c11 -Wall -Imusl/include -o censor.o $<
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o $@ censor.o musl/lib/crt1.o musl/lib/libc.a

lolcat: lolcat.c
	gcc -std=c11 -Wall -o $@ $<

censor: censor.c
	gcc -std=c11 -Wall -o $@ $<

install: lolcat censor
	install lolcat /usr/local/bin
	install censor /usr/local/bin

clean:
	rm -f lolcat lolcat-static.o lolcat-static censor censor-static.o censor-static
	make -C musl clean

