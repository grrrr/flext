/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
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
#elif FLEXT_OS == FLEXT_OS_MAC
    #if FLEXT_OSAPI != FLEXT_OSAPI_MAC_MACH
        #include <MacMemory.h>
    #else
        #include <Carbon/Carbon.h>
    #endif
#endif

void flext::CopyMem(void *dst,const void *src,int bytes) 
{
#if FLEXT_OS == FLEXT_OS_WIN
    MoveMemory(dst,src,bytes);
#elif FLEXT_OS == FLEXT_OS_MAC
    BlockMoveData(src,dst,bytes);
#else
    memcpy(dst,src,bytes);
#endif
}

void flext::ZeroMem(void *dst,int bytes) 
{
#if FLEXT_OS == FLEXT_OS_WIN
    ZeroMemory(dst,bytes);
#elif FLEXT_OS == FLEXT_OS_MAC
    BlockZero(dst,bytes);
#else
    memset(dst,0,bytes);
#endif
}


bool flext::Forward(const t_symbol *recv,const t_symbol *s,int argc,const t_atom *argv)
{
    void *cl = recv->s_thing;
    if(!cl) return false;
    
#if FLEXT_SYS == FLEXT_SYS_PD
    pd_typedmess((t_class **)cl,(t_symbol *)s,argc,(t_atom *)argv);
#elif FLEXT_SYS == FLEXT_SYS_MAX
    typedmess(recv->s_thing,(t_symbol *)s,argc,(t_atom *)argv);
#else
#error Not implemented
#endif
    return true;
}
