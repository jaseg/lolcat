# What?

![](https://raw.githubusercontent.com/jaseg/lolcat/master/LOLCat-Rainbow.jpg)

## Screenshot

![](https://raw.githubusercontent.com/jaseg/lolcat/master/screenshot.png)

## Installation

```bash
$ make && sudo make install
```

## Why?

```bash
newton~/d/lolcat <3 dmesg>foo
newton~/d/lolcat <3 cat foo foo foo foo foo foo >bar
newton~/d/lolcat <3 cat bar bar bar bar bar bar >foo
newton~/d/lolcat <3 cat foo foo foo foo foo foo >bar
newton~/d/lolcat <3 cat bar bar bar bar bar bar >foo
newton~/d/lolcat <3 ls -lh foo
-rw-r--r-- 1 jaseg jaseg 578M Oct 25 16:48 foo
newton~/d/lolcat <3 time upstream/bin/lolcat foo
13.51user 1.34system 0:15.99elapsed 92%CPU (0avgtext+0avgdata 10864maxresident)k
0inputs+0outputs (0major+1716minor)pagefaults 0swaps
newton~/d/lolcat <3 time ./lolcat foo
0.02user 0.00system 0:00.09elapsed 34%CPU (0avgtext+0avgdata 1936maxresident)k
0inputs+0outputs (0major+117minor)pagefaults 0swaps
```

