/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flutil.cpp
    \brief Implementation of the various utility functions.
*/
 
#include "flext.h"
#include <string.h>

#ifdef NT
#include <windows.h>
#endif

#ifdef MAXMSP
#include <Timer.h>
#include <Threads.h>
#endif

#ifdef unix
#include <unistd.h>
#endif

//namespace flext {

t_atom *flext::CopyList(int argc,const t_atom *argv)
{
	int i;
	t_atom *dst = new t_atom[argc];
	for(i = 0; i < argc; ++i) CopyAtom(dst+i,argv+i);
	return dst;
}

void flext::CopyMem(void *dst,const void *src,int bytes) 
{
#ifdef macintosh
	BlockMoveData(src,dst,bytes);
#else
	memcpy(dst,src,bytes);
#endif
}

void flext::Sleep(float s)
{
#ifdef NT
	::Sleep((long)(s*1000));
#elif defined MAXMSP
	UnsignedWide tick;
	Microseconds(&tick);
	double target = tick.hi*((double)(1L<<((sizeof tick.lo)*4))*(double)(1L<<((sizeof tick.lo)*4)))+tick.lo+s*1.e6; 
	for(;;) {
		Microseconds(&tick);
		if(target <= tick.hi*((double)(1L<<((sizeof tick.lo)*4))*(double)(1L<<((sizeof tick.lo)*4)))+tick.lo) break;
		YieldToAnyThread(); // should we really yield?
	}
#else
	usleep((long)(s*1000000));
#endif
}

//} // namespace flext
