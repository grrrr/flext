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

//! flext version number
#define FLEXT_VERSION 300
//! flext version string
#define FLEXT_VERSTR "0.3.0pre"

// include all the important flext interface definitions
#include <fldefs.h>

// include the basic flext object classes
#include <flclass.h>

// include pthreads if threading is enabled
#ifdef FLEXT_THREADS
#include <flthr.h>
#endif

#endif // FLEXT_H


