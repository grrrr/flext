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

#ifdef FLEXT_USE_IPP
#include <ipps.h>
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
    flext::simd_type simdflags;
#if FLEXT_CPU == FLEXT_CPU_INTEL 
    #ifdef FLEXT_USE_IPP
        simdflags = flext::simd_none;
    #else
    	simdflags = flext::simd_none;
    #endif
#else
	simdflags = flext::simd_none;
#endif
    return simdflags;
}


void flext::CopySamples(t_sample *dst,const t_sample *src,int cnt) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4)
        ippsCopy_32f((const float *)src,(float *)dst,cnt); 
    else if(sizeof(t_sample) == 8)
        ippsCopy_64f((const double *)src,(double *)dst,cnt); 
    else
        ERRINTERNAL();
#else
	int n = cnt>>3;
	cnt -= n<<3;
	while(n--) {
		dst[0] = src[0]; dst[1] = src[1];
		dst[2] = src[2]; dst[3] = src[3];
		dst[4] = src[4]; dst[5] = src[5];
		dst[6] = src[6]; dst[7] = src[7];
		src += 8,dst += 8;
	}
	
	while(cnt--) *(dst++) = *(src++); 
#endif
}

void flext::SetSamples(t_sample *dst,int cnt,t_sample s) 
{
#ifdef FLEXT_USE_IPP
    if(sizeof(t_sample) == 4)
        ippsSet_32f((float)s,(float *)dst,cnt); 
    else if(sizeof(t_sample) == 8)
        ippsSet_64f((double)s,(double *)dst,cnt); 
    else
        ERRINTERNAL();
#else
	int n = cnt>>3;
	cnt -= n<<3;
	while(n--) {
		dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = s;
		dst += 8;
	}
	
	while(cnt--) *(dst++) = s; 
#endif
}
