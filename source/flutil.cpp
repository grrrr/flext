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

void flext::CopyMem(void *dst,const void *src,int bytes) 
{
	memcpy(dst,src,bytes);
}

void flext::ZeroMem(void *dst,int bytes) 
{
	memset(dst,0,bytes);
}


bool flext::Forward(const t_symbol *recv,int argc,const t_atom *argv)
{
	t_class **cl = (t_class **)recv->s_thing;
	if(!cl) return false;
    
#if FLEXT_SYS == FLEXT_SYS_PD
	pd_forwardmess(cl,argc,(t_atom *)argv);
#else
    #error Not implemented
#endif

    return true;
}
