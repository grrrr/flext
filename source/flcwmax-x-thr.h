/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is the prefix file for CodeWarrior projects - threaded version

#ifndef _FLEXT_CW_MAX_X_THR_H
#define _FLEXT_CW_MAX_X_THR_H

#define FLEXT_THREADS
#define _POSIX_THREADS

/*
	old CodeWarrior version (<= 6) don't have sigset_t defined which
	is needed for pthreads
*/
#if defined(__MWERKS__) && (__MWERKS__ <= 0x6000)
	typedef unsigned int sigset_t;
	#define _CW_NOPRECOMP // no precompiled headers
#endif

#include "flcwmax-x.h"

#endif
