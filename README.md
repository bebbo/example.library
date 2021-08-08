# example.library
an example library for the Amiga

all data/bss belongs to the library structure -> no fuzz with a4.

# building without eclipse

simply run
```m68k-amigaos-gcc -nostartfiles -o example.library alibrary.c myfx.c zinitexit.c -Os```

note that alibrary.c **should** be specified as the first file.

# some notes
## defining library functions
Note that you define your library functions **with** register a6 in the library's code:

```int myfunction1(int a asm("d0"), struct MyLib * mylib asm("a6"))```

For the library users this becomes

```int myfunction1(int a asm("d0"))```

the register a6 is loaded while invoking the library function.

## library modes
this example provides a library base which shares the data to all openers.
A variant which provides a library base per opener: TBD.