#include "mylib.h"

#include <proto/exec.h>

// init all you need.
void __myinit(struct MyLib *mylib asm("a6")) {
	SYSBASE;
	mylib->utilityBase = OpenLibrary((STRPTR) "utility.library", 0);
}

// close all you need.
void __myexit(struct MyLib *mylib asm("a6")) {
	SYSBASE;
	CloseLibrary(mylib->utilityBase);
}
