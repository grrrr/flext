/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flstdc.h
	\brief Definitions to unite MaxMSP and PD notions
    
	This file contains a few definitions to unite a few of the notions that 
	once drifted apart in Max and PD. It is not elegant but helps.
*/

#ifndef __FLEXT_STDC_H
#define __FLEXT_STDC_H

// Be sure that one target is defined
#if !defined(PD) && !defined(MAXMSP)
#error Either PD or MAXMSP must be defined
#endif

// Do some compiler checking
#if defined(__MRC__) && __MRC__ < 0x500
#error Apple MPW MrCpp v.5.0.0 or better compiler required
#endif


// PD stuff

#ifdef PD

/* PD definitions start here */

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4091)
#endif

#ifdef __cplusplus
extern "C" {	    	    	    	    	    	    	
#endif

// Include the relevant PD header files
#ifdef _DEBUG
#include <m_imp.h>  // for easier debugging
#else
#include <m_pd.h>
#endif

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#ifdef cabs
#undef cabs // this is defined in m_pd.h (clashes with math.h)
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

// MAX stuff
#elif defined(MAXMSP)

/* MaxMSP definitions start here */

#ifndef __MRC__
#define powerc
#endif
#define __MOTO__ 0


// Include the relevant MaxMSP header files

#include <MacTypes.h>

extern "C"
{

#include "ext.h"
/*
#include "ext_prefix.h"
#include "ext_mess.h"
#include "ext_proto.h"
*/
//#include "ext_strings.h"  // clashes with MPW
#include "ext_user.h"
#include "z_dsp.h"
//#include "z_atom.h"
}

typedef t_pxbox t_sigobj;  // that's the all-in-one object type of MaxMSP (not very memory-efficent, i guess)
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
//#define A_FLINT A_INT
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

#ifdef _DEBUG
#define ASSERT(b) ((void)(!(b)?(error("Assertion failed: " #b " - in " __FILE__ " line %i",(int)__LINE__),0):0)) 
#else
#define ASSERT(b) ((void)0)
#endif

#define ERRINTERNAL() error("flext: Internal error in file " __FILE__ ", line %i - please report",(int)__LINE__)


/* Set the right calling convention (and exporting) for the OS */

#if defined(NT)
#define FLEXT_EXT __declspec(dllexport)
#else                   // other OS's
#define FLEXT_EXT
#endif

#endif





