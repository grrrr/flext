/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flstdc.h
	\brief Definitions to unite Max/MSP and PD notions
    
	This file contains a few definitions to unite a few of the notions that 
	once drifted apart in Max and PD. It is not elegant but helps.
*/

#ifndef __FLEXT_STDC_H
#define __FLEXT_STDC_H

// PD stuff

#if FLEXT_SYS == FLEXT_SYS_PD

/* PD definitions start here */

#ifdef _MSC_VER
	#pragma warning (push)
	#pragma warning (disable:4091 4005)
#endif

#if FLEXT_OS == FLEXT_OS_WIN && !defined(NT)
#define NT
#endif

extern "C" {	    	    	    	    	    	    	
	// Include the relevant PD header files
	#ifdef FLEXT_DEBUG
		#include <m_imp.h>  // for easier debugging
	#else
		#include <m_pd.h>
	#endif
}

#ifdef _MSC_VER
	#pragma warning (pop)
#endif

#ifdef cabs
#undef cabs // this is defined in m_pd.h (clashes with math.h in MacOSX)
#endif

typedef t_object t_sigobj;
typedef t_gpointer *t_ptrtype;

typedef t_float t_flint;
typedef t_symbol *t_symptr;
typedef t_symbol *t_symtype;
typedef t_class **t_thing;

typedef t_clock t_qelem;

#define A_NOTHING A_NULL
#define A_FLINT A_FLOAT
#define A_DEFFLINT A_DEFFLOAT


#elif FLEXT_SYS == FLEXT_SYS_MAX

/* Max/MSP definitions start here */


// Include the relevant Max/MSP header files

#if FLEXT_OS == FLEXT_OS_MACOS
	#ifndef __MRC__
		#define powerc
	#endif
	#define __MOTO__ 0

	#include <MacTypes.h>
#elif FLEXT_OS == FLEXT_OS_WIN
	#define WIN_VERSION 1
#endif

extern "C"
{
	#include "ext.h"
	#include "ext_user.h"
	#include "z_dsp.h"
}

#undef WIN_VERSION


typedef t_pxbox t_sigobj;  // that's the all-in-one object type of Max/MSP (not very memory-efficent, i guess)
typedef t_patcher t_canvas;

typedef t_int t_flint;
typedef t_symbol *t_symptr;
typedef t_symbol *t_symtype;
typedef t_object *t_thing;

typedef qelem t_qelem;

typedef method t_method;
typedef method t_newmethod;
typedef int t_atomtype;

typedef struct clock t_clock;
typedef void t_binbuf;

#undef clock_free
#define clock_free(tick) freeobject((object *)tick)

#define A_NULL A_NOTHING
#define A_DEFFLINT A_DEFLONG

#ifndef A_INT
#define A_INT A_LONG
#endif

#ifndef A_SYMBOL
#define A_SYMBOL A_SYM
#endif

#endif


#ifdef _LOG

/* If _LOG is defined implement logging */
#define LOG(s) post(s)
#define LOG1(s,v1) post(s,v1)
#define LOG2(s,v1,v2) post(s,v1,v2)
#define LOG3(s,v1,v2,v3) post(s,v1,v2,v3)
#define LOG4(s,v1,v2,v3,v4) post(s,v1,v2,v3,v4)
#define LOG5(s,v1,v2,v3,v4,v5) post(s,v1,v2,v3,v4,v5)


#else

/* If _LOG is not defined avoid logging */
#define LOG(s) ((void)0)
#define LOG1(s,v1) ((void)0)
#define LOG2(s,v1,v2) ((void)0)
#define LOG3(s,v1,v2,v3) ((void)0)
#define LOG4(s,v1,v2,v3,v4) ((void)0)
#define LOG5(s,v1,v2,v3,v4,v5) ((void)0)

#endif

#ifdef FLEXT_DEBUG
#define ASSERT(b) ((void)(!(b)?(error("Assertion failed: " #b " - in " __FILE__ " line %i",(int)__LINE__),0):0)) 
#else
#define ASSERT(b) ((void)0)
#endif

#define ERRINTERNAL() error("flext: Internal error in file " __FILE__ ", line %i - please report",(int)__LINE__)


/* Set the right calling convention (and exporting) for the OS */

#ifdef _MSC_VER
	#define FLEXT_EXT __declspec(dllexport)
#else                   // other OS's
	#define FLEXT_EXT
#endif

#endif
