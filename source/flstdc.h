/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2004 Thomas Grill (xovo@gmx.net)
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
        /*  PD header file structure has changed with version 0.37
            from then on m_imp.h needs m_pd.h to be included before
            on the other hand versions < 0.37 don't like that....
            (they want m_imp.h solely as m_pd.h is included therein)
            So better use the m_pd.h here also for the debug version.
            Change that if really needed for debugging PD internals...
        */

		#ifndef PD_VERSION
			// include only if not already included
			#include <m_pd.h>
		#endif
//		#include <m_imp.h>  // for easier debugging
    #else
		#ifndef PD_VERSION
			// include only if not already included
			#include <m_pd.h>
		#endif
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
typedef t_symbol *t_symtype;
typedef t_class **t_thing;

typedef t_clock t_qelem;

#define A_NOTHING A_NULL
#define A_FLINT A_FLOAT
#define A_DEFFLINT A_DEFFLOAT
#define A_DEFSYMBOL A_DEFSYM


#elif FLEXT_SYS == FLEXT_SYS_MAX

/* Max/MSP definitions start here */


// Include the relevant Max/MSP header files

#if FLEXT_OS == FLEXT_OS_MAC
	#ifndef __MRC__
		#define powerc
	#endif
	#define __MOTO__ 0

	#include <MacTypes.h>
#elif FLEXT_OS == FLEXT_OS_WIN
	#define WIN_VERSION 1
#endif


// necessary for the old OS9 SDK
extern "C" {	    	    	    

#include "ext.h"
#include "ext_user.h"
#include "z_dsp.h"

} // extern "C"

#undef WIN_VERSION


typedef t_pxbox t_sigobj;  // that's the all-in-one object type of Max/MSP (not very memory-efficent, i guess)
typedef t_patcher t_canvas;

typedef t_int t_flint;
typedef t_symbol *t_symtype;
typedef t_object *t_thing;

#if FLEXT_OS == FLEXT_OS_WIN
typedef void *t_qelem;  // qelem not defined in Windows SDK 
#else
typedef qelem t_qelem;
#endif

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

#ifndef A_DEFINT
#define A_DEFINT A_DEFLONG
#endif

#ifndef A_SYMBOL
#define A_SYMBOL A_SYM
#endif

#ifndef A_DEFSYMBOL
#define A_DEFSYMBOL A_DEFSYM
#endif


#elif FLEXT_SYS == FLEXT_SYS_JMAX

extern "C" {
	// Wow, the jMax developers made excessive use of C++ reserved words
	// good hit!
	#define typeid c_typeid_
	#define template c_template_
	#define this c_this_
	#define class c_class_
	
	#include <fts/fts.h>
	
	// undefine them again
	#undef typeid 
	#undef template 
	#undef this 
	#undef class 
}

typedef fts_dsp_object t_sigobj;
typedef void t_canvas; // decide type later on

typedef char t_symbol;
typedef fts_atom_t t_atom;
typedef fts_class_t t_class;
typedef float t_sample; // is there no sample type in jMax?

typedef fts_timebase_entry_t t_clock;
typedef fts_timebase_entry_t t_qelem;

#endif // FLEXT_SYS


// general definitions

typedef t_symbol *t_symptr;


// -------------------------

#ifdef FLEXT_LOGGING

/* If FLEXT_LOGGING is defined implement logging */
#define FLEXT_LOG(s) post(s)
#define FLEXT_LOG1(s,v1) post(s,v1)
#define FLEXT_LOG2(s,v1,v2) post(s,v1,v2)
#define FLEXT_LOG3(s,v1,v2,v3) post(s,v1,v2,v3)
#define FLEXT_LOG4(s,v1,v2,v3,v4) post(s,v1,v2,v3,v4)
#define FLEXT_LOG5(s,v1,v2,v3,v4,v5) post(s,v1,v2,v3,v4,v5)


#else

/* If FLEXT_LOGGING is not defined avoid logging */
#define FLEXT_LOG(s) ((void)0)
#define FLEXT_LOG1(s,v1) ((void)0)
#define FLEXT_LOG2(s,v1,v2) ((void)0)
#define FLEXT_LOG3(s,v1,v2,v3) ((void)0)
#define FLEXT_LOG4(s,v1,v2,v3,v4) ((void)0)
#define FLEXT_LOG5(s,v1,v2,v3,v4,v5) ((void)0)

#endif

#ifdef FLEXT_DEBUG
#define FLEXT_ASSERT(b) (!(b)?(error("Assertion failed: " #b " - in " __FILE__ " line %i",(int)__LINE__),0):1)
#else
#define FLEXT_ASSERT(b) (1)
#endif

#define ERRINTERNAL() error("flext: Internal error in file " __FILE__ ", line %i - please report",(int)__LINE__)


/* Set the right calling convention (and exporting) for the OS */

#ifdef _MSC_VER
	#ifdef FLEXT_SHARED
        // for compiling a shared flext library FLEXT_EXPORTS must be defined
        #ifdef FLEXT_EXPORTS
		    #define FLEXT_SHARE __declspec(dllexport)
        #else
		    #define FLEXT_SHARE __declspec(dllimport)
        #endif
	#else
		#define FLEXT_SHARE
	#endif
	#define FLEXT_EXT __declspec(dllexport)
#else                   // other OS's
	#define FLEXT_SHARE
	#define FLEXT_EXT
#endif

// ----- disable attribute editor for PD version < devel_0_36 or 0.37
#ifndef PD_MAJOR_VERSION
#undef FLEXT_NOATTREDIT
#define FLEXT_NOATTREDIT
#endif


// ----- set message queue mode -----
#undef FLEXT_QTHR

#if FLEXT_SYS == FLEXT_SYS_PD && PD_MINOR_VERSION >= 37
	// for PD version >= 0.37test10 FLEXT_PDLOCK is standard
	#undef FLEXT_PDLOCK
	#define FLEXT_PDLOCK
#endif

#ifdef FLEXT_PDLOCK
	// new PD thread locking functionality shall be used
	#if FLEXT_SYS == FLEXT_SYS_PD
		#ifdef FLEXT_THREADS
			// can only be used with PD and threaded build
			#define FLEXT_QTHR
		#endif
	#else
		#error FLEXT_PDLOCK can only be defined with PD
	#endif
#endif


#endif
