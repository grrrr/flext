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
