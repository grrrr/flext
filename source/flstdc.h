/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*
this file contains a few definitions to unite a few of the notions that 
once drifted apart in Max and PD. It is not elegant but helps.
*/

#ifndef __FLSTDC_H
#define __FLSTDC_H

#ifdef __cplusplus
// namespace std {
#endif

#if !defined(PD) && !defined(MAXMSP)
#error Either PD or MAXMSP must be defined
#endif

#ifdef PD

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4091)
#endif

#ifdef __cplusplus
extern "C" {	    	    	    	    	    	    	
#endif

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

typedef t_object t_sigobj;
typedef t_gpointer *t_ptrtype;

typedef t_float t_flint;

#define A_NOTHING A_NULL
#define A_FLINT A_FLOAT
#define A_DEFFLINT A_DEFFLOAT
 

#elif defined(MAXMSP)

extern "C"
{
#include "ext.h"
#include "ext_strings.h"
#include "ext_user.h"
#include "z_dsp.h"
#include "z_atom.h"
#include "buffer.h"
}

typedef t_pxobject t_sigobj;
typedef t_patcher t_canvas;

typedef t_int t_flint;

typedef method t_method;
typedef method t_newmethod;
typedef int t_atomtype;

#define A_NULL A_NOTHING
#define A_FLINT A_INT
#define A_DEFFLINT A_DEFLONG

#endif


typedef t_symbol *t_symtype;

#ifdef _LOG
#define LOG(s) post(s)
#define LOG1(s,v1) post(s,v1)
#define LOG2(s,v1,v2) post(s,v1,v2)
#define LOG3(s,v1,v2,v3) post(s,v1,v2,v3)
#define LOG4(s,v1,v2,v3,v4) post(s,v1,v2,v3,v4)
#define LOG5(s,v1,v2,v3,v4,v5) post(s,v1,v2,v3,v4,v5)
#define ASSERT(b) ((void)(!(b)?(error("Assertion failed: " #b " - in " __FILE__ " line %i",(I)__LINE__),0):0)) 
#else
#define LOG(s) ((void)0)
#define LOG1(s,v1) ((void)0)
#define LOG2(s,v1,v2) ((void)0)
#define LOG3(s,v1,v2,v3) ((void)0)
#define LOG4(s,v1,v2,v3,v4) ((void)0)
#define LOG5(s,v1,v2,v3,v4,v5) ((void)0)
#define ASSERT(b) ((void)0)
#endif


#if defined(NT)
#define FLEXT_EXT __declspec(dllexport)
#else                   // other OS's
#define FLEXT_EXT
#endif


#ifdef __cplusplus
// }  // namespace std 
#endif

#endif




