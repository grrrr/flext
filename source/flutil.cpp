/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>

#ifdef NT
#include <windows.h>
#endif

#ifdef MAXMSP
#include <Timer.h>
#include <Threads.h>
#endif

t_atom *flext_base::CopyList(int argc,const t_atom *argv)
{
	int i;
	t_atom *dst = new t_atom[argc];
	for(i = 0; i < argc; ++i) CopyAtom(dst+i,argv+i);
	return dst;
}

void flext_base::Sleep(float s)
{
#ifdef NT
	::Sleep((long)(s*1000));
#elif defined MAXMSP
	UnsignedWide tick;
	Microseconds(&tick);
	double target = tick.hi*((double)(1<<16)*(double)(1<<16))+tick.lo+s*1.e6; 
	for(;;) {
		Microseconds(&tick);
		if(target <= tick.hi*((double)(1<<16)*(double)(1<<16))+tick.lo) break;
		YieldToAnyThread();
	}
#endif
}