/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flcwmax.h
    \brief Prefix file for CodeWarrior projects - OS 9 version.
*/
 
#ifndef _FLEXT_CW_MAX_H
#define _FLEXT_CW_MAX_H

#ifndef __MWERKS__
	#error "This header file is for CodeWarrior only."
#endif

#define FLEXT_SYS 1
 
#ifndef _CW_NOPRECOMP
	#include <MacHeaders.h>
#endif

#if __option(sym) || !__option(opt_dead_code)
	#define FLEXT_DEBUG
#endif

/* #define _LOG */

#endif
