# What?

![](https://raw.githubusercontent.com/jaseg/lolcat/master/LOLCat-Rainbow.jpg)

## Screenshot

![](https://raw.githubusercontent.com/jaseg/lolcat/master/screenshot.png)

![](./sl.gif)

## Installation

### Archlinux

There's an [AUR package](https://aur.archlinux.org/packages/c-lolcat):

```bash
$ git clone https://aur.archlinux.org/packages/c-lolcat
$ cd c-lolcat
$ makepkg -csi
```

### Fedora ###

```bash
$ dnf install lolcat
```

### Ubuntu (Snap)

See [this awesome blog post by a kind person from the internet](https://blog.simos.info/how-to-make-a-snap-package-for-lolcat-with-snapcraft-on-ubuntu/):

```bash
$ snap install lolcat-c
```

### Mac

Build loclcat with:
```
$ make lolcat
```
...and put the resulting binary at a place of your choice.

### Others

```bash
$ make && sudo make install
```

## Why?

This `lolcat` clone is an attempt to reduce the world's carbon dioxide emissions by optimizing inefficient code. It's >10x as fast and <0.1% as large as the original one.

```bash
newton~/d/lolcat <3 dmesg>foo
newton~/d/lolcat <3 time upstream/bin/lolcat foo
13.51user 1.34system 0:15.99elapsed 92%CPU (0avgtext+0avgdata 10864maxresident)k
0inputs+0outputs (0major+1716minor)pagefaults 0swaps
newton~/d/lolcat <3 time ./lolcat foo
0.02user 0.00system 0:00.09elapsed 34%CPU (0avgtext+0avgdata 1936maxresident)k
0inputs+0outputs (0major+117minor)pagefaults 0swaps
```

Bonus comparison with [python-lolcat](https://github.com/tehmaze/lolcat/):
```bash
newton~/d/lolcat <3 dmesg>foo
$ time python-lolcat foo
12.27user 0.00system 0:12.29elapsed 99%CPU (0avgtext+0avgdata 11484maxresident)k
0inputs+0outputs (0major+1627minor)pagefaults 0swaps
$ time c-lolcat foo
0.29user 0.00system 0:00.30elapsed 98%CPU (0avgtext+0avgdata 468maxresident)k
0inputs+0outputs (0major+21minor)pagefaults 0swaps
```

(Read: ```c-lolcat << python-lolcat << ruby-lolcat```)

