# SISH
SISH implements a very simple command-line interpreter or shell in C. It is suitable to be used interactively or as a login shell. It only implements a very small subset of what would usually be expected of a Unix shell, and does explicitly not lend itself as a scripting language.

## Manual Page
https://stevens.netmeister.org/631/sish.1.pdf

## How to build?
```sh
make clean && make
```

## How to run?
```sh
./sish [OPTIONS]
SISH$ ls
a b c d
SISH$ whoami
z
SISH$ echo $?
0
SISH$ echo $$
11304
SISH$ exit
```
