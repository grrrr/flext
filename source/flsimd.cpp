/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsimd.cpp
	\brief flext SIMD support functions    
*/

#include "flext.h"

#if FLEXT_OS == FLEXT_OS_WIN
#include <windows.h>
#endif

#if FLEXT_CPU == FLEXT_CPU_PPC && defined(__MWERKS__)
#include "Altivec.h"
#endif

static unsigned long setsimdcaps();

/*! \brief Holds SIMD capability flags
	\internal
*/
unsigned long flext::simdcaps = setsimdcaps();

/*! \brief Determine SIMD capabilities
	\internal
*/
static unsigned long setsimdcaps()
{
	return flext::simd_none;
}


