/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flutil.cpp
    \brief Implementation of the various utility functions.
*/
 
#include "flext.h"
#include <string.h>

#if FLEXT_OS == FLEXT_OS_WIN
#include <windows.h>
#elif FLEXT_OS == FLEXT_OS_LINUX || FLEXT_OS == FLEXT_OS_IRIX || defined(__GNUC__)
#include <unistd.h>
#elif FLEXT_OS == FLEXT_OS_MACOS
#include <Timer.h>
#include <Threads.h>
#endif

void flext::CopyMem(void *dst,const void *src,int bytes) 
{
	memcpy(dst,src,bytes);
}

void flext::ZeroMem(void *dst,int bytes) 
{
	memset(dst,0,bytes);
}

void flext::Sleep(double s)
{
#if FLEXT_OS == FLEXT_OS_WIN
	::Sleep((long)(s*1000.));
#elif FLEXT_OS == FLEXT_OS_LINUX || FLEXT_OS == FLEXT_OS_IRIX || defined(__GNUC__)
	usleep((long)(s*1000000.));
#elif FLEXT_OS == FLEXT_OS_MACOS
	// that's just for OS9!
	UnsignedWide tick;
	Microseconds(&tick);
	double target = tick.hi*((double)(1L<<((sizeof tick.lo)*4))*(double)(1L<<((sizeof tick.lo)*4)))+tick.lo+s*1.e6; 
	for(;;) {
		Microseconds(&tick);
		if(target <= tick.hi*((double)(1L<<((sizeof tick.lo)*4))*(double)(1L<<((sizeof tick.lo)*4)))+tick.lo) break;
		YieldToAnyThread(); // should we really yield?
	}
#else
#error
#endif
}
