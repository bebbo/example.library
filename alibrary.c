#include <exec/resident.h>
#include <proto/exec.h>

#include "mylib.h"

// make cli happy
long safefail(void) {
	return -1;
}

// library header stuff
const struct Resident RomTag;
const char libName[];
const char libIdString[];

const char libName[] = MYLIBNAME;
const char libIdString[] = MYLIBID;

// dunno
long LibExtFunc(void) {
	return 0;
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

	/* return the seglist for UnLoadSeg() */
	return mylib->segList;
}

// close the library
long LibClose(struct MyLib *mylib asm("a6")) {
	SYSBASE;

	/* do your user-close here */
	CloseLibrary(mylib->utilityBase);

	/* one less user */
	if (!--mylib->lib.lib_OpenCnt && (mylib->lib.lib_Flags & LIBF_DELEXP))
		return LibExpunge(mylib);

	return 0;
}

// open the library
struct MyLib * LibOpen(struct MyLib *mylib asm("a6")) {
	SYSBASE;
	/* any memory allocation can cause a call of THIS device expunge vector.
	 if OpenCnt is zero the library might go away ... so fake a user :-) */
	++mylib->lib.lib_OpenCnt;

	/* do your opening stuff here */
    mylib->utilityBase = OpenLibrary((STRPTR)"utility.library", 0);

	/* clear delayed expunge flag */
	mylib->lib.lib_Flags &= ~LIBF_DELEXP;

	/* one new user */
	++mylib->lib.lib_OpenCnt;

	/* end of expunge protection */
	--mylib->lib.lib_OpenCnt;

	return mylib;
}

void foo() {}

APTR LibInit(long segList asm("a0"), struct MyLib * lib asm("d0"), struct ExecBase *sysBase asm("a6")) {
	/* set up header data */
	lib->lib.lib_Node.ln_Type = NT_LIBRARY;
	lib->lib.lib_Node.ln_Name = (char*) libName;
	lib->lib.lib_Flags = LIBF_CHANGED | LIBF_SUMUSED;
	lib->lib.lib_Version = (UWORD) 1;
	lib->lib.lib_Revision = (UWORD) 0;
	lib->lib.lib_IdString = (char*) libIdString;

	/* setup private data */

	lib->segList = segList;
	lib->sysBase = sysBase;

	/* this will be added to SysBase->LibList or NULL (init error) */
	return lib;
}

// magic
const APTR __FuncTable__[] = { LibOpen, LibClose, LibExpunge, NULL,
		MYFUNCTIONS
, (APTR) -1 };

const APTR InitTab[4] = { (APTR) sizeof(struct MyLib), (APTR)&__FuncTable__[0], (APTR) NULL, (APTR) &LibInit };

const struct Resident RomTag = { RTC_MATCHWORD, (struct Resident*) &RomTag, (struct Resident*) &RomTag + 1, RTF_AUTOINIT, 1,
NT_LIBRARY, 0, (char*) libName, (char*) libIdString, (APTR) &InitTab };
