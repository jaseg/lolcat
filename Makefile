
all: lolcat

musl/lib/libc.a musl/lib/crt1.o:
	cd musl; ./configure
	make -C musl

lolcat: lolcat.c musl/lib/libc.a musl/lib/crt1.o
	gcc -c -std=c11 -Wall -Imusl/include -o lolcat.o lolcat.c
	gcc -s -nostartfiles -nodefaultlibs -nostdinc -static -ffunction-sections -fdata-sections -Wl,--gc-sections -o lolcat lolcat.o musl/lib/crt1.o musl/lib/libc.a

install: lolcat
	install lolcat /usr/local/bin

clean:
	rm -f lolcat lolcat.o

.PHONY: install clean

