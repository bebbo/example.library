#include "mylib.h"

#include <proto/exec.h>

// init all you need.
void __myglobalinit(struct MyLib * mylib asm("a6")) {
	SYSBASE;
	mylib->utilityBase = OpenLibrary((STRPTR) "utility.library", 0);
}

// close all you need.
void __myglobalexit(struct MyLib * mylib asm("a6")) {
	SYSBASE;
	CloseLibrary(mylib->utilityBase);
}


#ifdef MULTIBASE
// init / exit functions - called on each open/close
void __mylocalinit(struct PerOpenerLib * polib asm("a6")) {
	polib->counter = 42;
	polib->utilityBase = polib->mylib->utilityBase;
}

void __mylocalexit(struct PerOpenerLib * polib asm("a6")) {

}
#endif
