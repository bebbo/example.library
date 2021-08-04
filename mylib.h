#ifndef __MYLIB_H__
#define __MYLIB_H__

#include <exec/types.h>
#include <exec/libraries.h>

struct MyLib {
	struct Library lib;
	unsigned long segList;
	struct ExecBase *sysBase;

// add all data here - just an example
	struct Library * utilityBase;
};

// define all libs
#define SYSBASE     struct ExecBase * const SysBase     = mylib->sysBase
#define UTILITYBASE struct Library  * const UtilityBase = mylib->utilityBase

// name
#define MYLIBNAME "dummy.library"
#define MYLIBID   "dummy 1.0 (23.12.2020)"

// define all user functions
int myfunction1(int a asm("d0"), struct MyLib * mylib asm("a6"));
int myfunction2(int a asm("d0"), struct MyLib * mylib asm("a6"));

#define MYFUNCTIONS myfunction1, myfunction2


#endif // __MYLIB_H__
