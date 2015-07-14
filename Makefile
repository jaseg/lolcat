
all: lolcat censor

musl/lib/libc.a musl/lib/crt1.o:
	cd musl; ./configure
	make -C musl

musl: musl/lib/libc.a musl/lib/crt1.o

lolcat: lolcat.c musl
	gcc -c -std=c11 -Wall -Imusl/include -o lolcat.o lolcat.c
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o lolcat lolcat.o musl/lib/crt1.o musl/lib/libc.a

censor: censor.c musl
	gcc -c -std=c11 -Wall -Imusl/include -o censor.o censor.c
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o censor censor.o musl/lib/crt1.o musl/lib/libc.a

install: lolcat censor
	install lolcat /usr/local/bin
	install censor /usr/local/bin

clean:
	rm -f lolcat lolcat.o censor censor.o
	make -C musl clean

.PHONY: install clean musl

