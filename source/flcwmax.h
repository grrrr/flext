/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/* This is the prefix file for CodeWarrior projects  */

#ifndef _FLEXT_CW_MAX_H
#define _FLEXT_CW_MAX_H

#ifndef __MWERKS__
	#error "This header file is for CodeWarrior only."
#endif
 
#ifndef _CW_NOPRECOMP
	#include <MacHeaders.h>
#endif

#if __option(sym) || !__option(opt_dead_code)
	#define _DEBUG
#endif

#ifdef MAXMSP
	#pragma message("#define MAXMSP is deprecated, use #define FLEXT_SYS_MAX instead")
#else
	#define FLEXT_SYS_MAX
	// this is deprecated (but still necessary)
	#define MAXMSP
#endif

#if __POWERPC__
	#define FLEXT_PLATFORM_MAC
#elif __INTEL__
	#define FLEXT_PLATFORM_WIN
	// this is necessary for Cycling headers when compiling for Windows
	#define WIN_VERSION 1
#else
	#error "Target platform not supported."
#endif
 
/* #define _LOG */

#endif
