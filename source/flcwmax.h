/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/* This is the prefix file for CodeWarrior projects  */

#ifndef _FLEXT_CW_MAX_H
#define _FLEXT_CW_MAX_H

#include <MacHeaders.h>

#define MAXMSP
 
#if !defined(__OPTIMIZE__) || __OPTIMIZE__ == 0
#define _DEBUG
#endif

/* #define _LOG */

#endif
