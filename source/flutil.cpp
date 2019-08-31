/*
flext - C++ layer for Max and Pure Data externals

Copyright (c) 2001-2015 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.
*/

/*! \file flutil.cpp
    \brief Implementation of the various utility functions.
*/
 
#ifndef __FLEXT_UTIL_CPP
#define __FLEXT_UTIL_CPP

#include "flext.h"
#include <cstring>

#if FLEXT_OS == FLEXT_OS_WIN
#include <windows.h>
#endif

#include "flpushns.h"

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::CopyMem(void *dst,const void *src,int bytes)
{
#if FLEXT_OS == FLEXT_OS_WIN
    MoveMemory(dst,src,bytes);
#else
    memmove(dst,src,bytes);
#endif
}

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::ZeroMem(void *dst,int bytes)
{
#if FLEXT_OS == FLEXT_OS_WIN
    ZeroMemory(dst,bytes);
#else
    memset(dst,0,bytes);
#endif
}

#include "flpopns.h"

#endif // __FLEXT_UTIL_CPP


