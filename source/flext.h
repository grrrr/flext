/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flext.h
    \brief This is the main flext include file.
    
    The basic definitions are set here and the necessary header files are included
*/

#ifndef __FLEXT_H
#define __FLEXT_H


/*!	\defgroup FLEXT_GLOBAL Flext global definitions
	@{
*/

//! \brief flext version number
#define FLEXT_VERSION 401

//! \brief flext version string
#define FLEXT_VERSTR "0.4.1pre"

//! @}


// determine System/OS/CPU
#include "flprefix.h"

#ifdef FLEXT_THREADS
extern "C" {
	#include <pthread.h>
	#include <sched.h>
}
#endif

// include all the flext interface definitions
#include "fldefs.h"

// include the basic flext object classes
#include "flclass.h"

// include the flext dsp class
#include "fldsp.h"

#endif // FLEXT_H


