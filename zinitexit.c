#include "mylib.h"

#include <proto/exec.h>

// init all you need.
void __myinit(MYLIB mylib asm("a6")) {
	SYSBASE;
	mylib->utilityBase = OpenLibrary((STRPTR) "utility.library", 0);
}

// close all you need.
void __myexit(MYLIB mylib asm("a6")) {
	SYSBASE;
	CloseLibrary(mylib->utilityBase);
}
