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

void flext::CopySamples(t_sample *dst,const t_sample *src,int cnt) 
{
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
}

void flext::SetSamples(t_sample *dst,int cnt,t_sample s) 
{
	int n = cnt>>3;
	cnt -= n<<3;
	while(n--) {
		dst[0] = dst[1] = dst[2] = dst[3] = dst[4] = dst[5] = dst[6] = dst[7] = s;
		dst += 8;
	}
	
	while(cnt--) *(dst++) = s; 
}


