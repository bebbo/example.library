#ifndef __MYLIB_H__
#define __MYLIB_H__

#include <exec/types.h>
#include <exec/semaphores.h>
#include <exec/libraries.h>

struct MyLib {
	struct Library lib;
	struct ExecBase *sysBase;
	// ==== same order until here as below
	unsigned long segList;
	short initialized;
// add all data here - just an example - see below for MULTIBASE
	struct Library *utilityBase;
};

#ifdef MULTIBASE
struct PerOpenerLib {
	struct Library lib;
	struct ExecBase *sysBase;
// ==== same order until here as above
	struct MyLib * mylib;

	// add all data here - just an example - this is for MULTIBASE
	int counter;
	struct Library *utilityBase;
};
typedef struct PerOpenerLib * MYLIB;
#else
typedef struct MyLib * MYLIB;
#endif

// define SYSBASE
#define SYSBASE     struct ExecBase * const SysBase     = mylib->sysBase

// same for the other libraries
#define UTILITYBASE struct Library  * const UtilityBase = mylib->utilityBase

// version
#define __XSTR(a) __STR(a)
#define __STR(a) #a
#define MYMAJORVERSION 1
#define MYMINORVERSION 42

// name
#define MYLIBNAME "dummy.library"
#define MYLIBID   "dummy " __XSTR(MYMAJORVERSION) "." __XSTR(MYMINORVERSION) " (23.12.2020)"

// define all user functions
int myfunction1(int a asm("d0"), MYLIB mylib asm("a6"));
int myfunction2(int a asm("d0"), MYLIB mylib asm("a6"));

#define MYFUNCTIONS myfunction1, myfunction2

// init / exit functions - called once per lib
void __myglobalinit(struct MyLib * mylib asm("a6"));
void __myglobalexit(struct MyLib * mylib asm("a6"));

#ifdef MULTIBASE
// init / exit functions - called on each open/close
void __mylocalinit(struct PerOpenerLib * polib asm("a6"));
void __mylocalexit(struct PerOpenerLib * polib asm("a6"));
#endif

#endif // __MYLIB_H__
