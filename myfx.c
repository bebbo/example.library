#include "mylib.h"

#include <proto/utility.h>

// an example function
// note the lib parameter with a6
int myfunction1(int a asm("d0"), MYLIB mylib asm("a6")) {
	UTILITYBASE;
	return UMult32(a, 42);
}

// an example function
// note the lib parameter with a6
int myfunction2(int a asm("d0"), MYLIB mylib asm("a6")) {
	UTILITYBASE;
	return UMult32(a, 7);
}
