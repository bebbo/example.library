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
const char libName[];
const char libIdString[];

const char libName[] = MYLIBNAME;
const char libIdString[] = MYLIBID;

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
	mylib->lib.lib_Version = (UWORD) 1;
	mylib->lib.lib_Revision = (UWORD) 0;
	mylib->lib.lib_IdString = (char*) libIdString;

	/* setup private data */
	mylib->segList = segList;
	mylib->sysBase = sysBase;
	mylib->initialized = 0;  // also used as marker if init is needed.

	SYSBASE;

	// init a semaphore
	InitSemaphore(&mylib->sema);

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

	__myexit(mylib);

	/* return the seglist for UnLoadSeg() */
	return mylib->segList;
}

// open the library
struct MyLib * LibOpen(struct MyLib *mylib asm("a6")) {
	SYSBASE;

	ObtainSemaphore(&mylib->sema);

	/* any memory allocation can cause a call of THIS device expunge vector.
	 if OpenCnt is zero the library might go away ... so fake a user :-) */
	++mylib->lib.lib_OpenCnt;

	/* clear delayed expunge flag */
	mylib->lib.lib_Flags &= ~LIBF_DELEXP;

	/* one new user */
	++mylib->lib.lib_OpenCnt;

	// run the open code only once...
	if (!mylib->initialized) {
		mylib->initialized = 1;
		__myinit(mylib);
	}

	/* end of expunge protection */
	--mylib->lib.lib_OpenCnt;

	ReleaseSemaphore(&mylib->sema);

	return mylib;
}

// close the library
long LibClose(struct MyLib *mylib asm("a6")) {
	/* one less user */
	if (!--mylib->lib.lib_OpenCnt && (mylib->lib.lib_Flags & LIBF_DELEXP))
		return LibExpunge(mylib);

	return 0;
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
