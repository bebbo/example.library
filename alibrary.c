#include <exec/execbase.h>
#include <exec/resident.h>
#include <proto/exec.h>

#include "mylib.h"

// make cli happy
long safefail(void) {
	return -1;
}

// library header stuff - kind of forward decls - results in a rom tag directly behind the cli-do-nothing-function.
// the real values are defined at the end.
const struct Resident RomTag;
const char libName[] = MYLIBNAME;
const char libIdString[] = MYLIBID;
const APTR __FuncTable__[];

// dunno
long LibExtFunc(void) {
	return 0;
}

// init the library. defer real init stuff to LibOpen!
APTR LibInit(long segList asm("a0"), struct MyLib * mylib asm("d0"), struct ExecBase *sysBase asm("a6")) {
	/* set up header data */
	mylib->lib.lib_Node.ln_Type = NT_LIBRARY;
	mylib->lib.lib_Node.ln_Name = (char*) libName;
	mylib->lib.lib_Flags = LIBF_CHANGED | LIBF_SUMUSED;
	mylib->lib.lib_Version = (UWORD) MYMAJORVERSION;
	mylib->lib.lib_Revision = (UWORD) MYMINORVERSION;
	mylib->lib.lib_IdString = (char*) libIdString;

	/* setup private data */
	mylib->segList = segList;
	mylib->sysBase = sysBase;
	mylib->initialized = 0;  // also used as marker if init is needed.

	/* this will be added to SysBase->LibList or NULL (init error) */
	return mylib;
}


// bye bye library
long LibExpunge(struct MyLib *mylib asm("a6")) {
	SYSBASE;

	/* set delayed expunge flag */
	mylib->lib.lib_Flags |= LIBF_DELEXP;

	/* still in use? */
	if (mylib->lib.lib_OpenCnt)
		return 0;

	/* remove lib from SysBase->LibList */
	Remove(&mylib->lib.lib_Node);

	// finally call the per-lib-exit once
	__myglobalexit(mylib);

	unsigned long segList = mylib->segList;

	// free the allocated ram of the lib's struct
	int neg = mylib->lib.lib_NegSize;
	FreeMem((-neg + (char *)mylib), neg + mylib->lib.lib_PosSize);

	/* return the seglist for UnLoadSeg() */
	return segList;
}

// open the library
struct MyLib * LibOpen(struct MyLib *mylib asm("a6")) {
	// Forbid is on - increase the counter
	++mylib->lib.lib_OpenCnt;

	/* clear delayed expunge flag */
	mylib->lib.lib_Flags &= ~LIBF_DELEXP;

	// run the open code only once...
	if (!mylib->initialized) {
		__myglobalinit(mylib);
		mylib->initialized = 1;
	}

#ifdef MULTIBASE
	SYSBASE;
	// get memory per lib
	char * b = (char *)AllocVec(sizeof(struct PerOpenerLib) + mylib->lib.lib_NegSize, MEMF_PUBLIC);

	// this copies the jump table and the lib node,
	// thus the pointer will work to refer to other libraries, but you'll never find this copy.
	CopyMemQuick (((char *)mylib) - mylib->lib.lib_NegSize, b, mylib->lib.lib_NegSize + sizeof(struct Library) + sizeof(APTR));
	struct PerOpenerLib * polib = (struct PerOpenerLib *)(b + mylib->lib.lib_NegSize);

	__mylocalinit(polib);
#endif

	return mylib;
}

// close the library
long LibClose(MYLIB alib asm("a6")) {
#ifdef MULTIBASE
	struct MyLib * mylib = alib->mylib;
	SYSBASE;

	__mylocalexit(alib);

	FreeVec(((char *)alib) + mylib->lib.lib_NegSize);

	// just in case __mylocalexit broke Forbid...
	Forbid();
#else
	struct MyLib * mylib = alib;
#endif

	/* one less user */
	long r = 0;
	if (!--mylib->lib.lib_OpenCnt && (mylib->lib.lib_Flags & LIBF_DELEXP))
		r = LibExpunge(mylib);

#ifdef MULTIBASE
	Permit();
#endif
	return r;
}

// the function table
const APTR __FuncTable__[] = { LibOpen, LibClose, LibExpunge, NULL,
		MYFUNCTIONS
, (APTR) -1 };

// the libraries init table
const APTR InitTab[4] = { (APTR) sizeof(struct MyLib), (APTR)&__FuncTable__[0], (APTR) NULL, (APTR) &LibInit };

// that's what the library loader is looking for: the rom tag with references to the remaining data.
const struct Resident RomTag = { RTC_MATCHWORD, (struct Resident*) &RomTag, (struct Resident*) &RomTag + 1, RTF_AUTOINIT, 1,
NT_LIBRARY, 0, (char*) libName, (char*) libIdString, (APTR) &InitTab };
