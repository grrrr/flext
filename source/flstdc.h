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
#pragma warning (push)
#pragma warning (disable:4091)
extern "C" {	    	    	    	    	    	    	
#ifdef _DEBUG
#include <m_imp.h>  // for easier debugging
#else
#include <m_pd.h>
#endif
}
#pragma warning (pop)

typedef t_object t_sigobj;

typedef t_float t_flint;

#define A_NOTHING A_NULL
#define A_FLINT A_FLOAT
 
#define atom_getflintarg atom_getfloatarg
#define atom_getsymarg atom_getsymbolarg

#define newobject(CLSS) pd_new(CLSS)

#define add_dsp(clss,meth) class_addmethod(clss, (t_method)meth,gensym("dsp"),A_NULL)
#define add_bang(clss,meth) class_addbang(clss, (t_method)meth)
#define add_float(clss,meth) class_addfloat(clss, (t_method)meth)
#define add_floatn(clss,meth,n) class_addmethod(clss, (t_method)meth,gensym("ft" #n),A_FLOAT,A_NULL)
#define add_flint(clss,meth) class_addfloat(clss, (t_method)meth)
#define add_flintn(clss,meth,n) class_addmethod(clss, (t_method)meth,gensym("ft" #n),A_FLOAT,A_NULL)
#define add_method(clss,meth,text) class_addmethod(clss, (t_method)meth, gensym(text), A_NULL)
#define add_methodG(clss,meth,text) class_addmethod(clss, (t_method)meth, gensym(text), A_GIMME,A_NULL)
#define add_method1(clss,meth,text,a1) class_addmethod(clss, (t_method)meth, gensym(text), a1,A_NULL)
#define add_method2(clss,meth,text,a1,a2) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,A_NULL)
#define add_method3(clss,meth,text,a1,a2,a3) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,a3,A_NULL)
#define add_method4(clss,meth,text,a1,a2,a3,a4) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,a3,a4,A_NULL)
#define add_method5(clss,meth,text,a1,a2,a3,a5) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,a3,a4,a5,A_NULL)
#define add_loadbang(clss,meth) class_addmethod(clss,(t_method)meth, gensym("loadbang"), A_CANT, A_NULL)

#define newout_signal(clss) outlet_new(clss,&s_signal)
#define newout_float(clss) outlet_new(clss,&s_float)
#define newout_flint(clss) outlet_new(clss,&s_float)
#define newout_list(clss) outlet_new(clss,&s_list)
#define newout_symbol(clss) outlet_new(clss,&s_symbol)

#define outlet_flint(o,v) outlet_float(o,v)


#elif defined(MAXMSP)

extern "C"
{
#include "ext.h"
#include "ext_strings.h"
#include "ext_user.h"
#include "z_dsp.h"
#include "buffer.h"
}

typedef t_pxobject t_sigobj;
typedef t_patcher t_canvas;

typedef t_int t_flint;

typedef void _inlet;
typedef _inlet t_inlet;
typedef void _outlet;
typedef _outlet t_outlet;

#define t_method method
#define t_newmethod method

#define A_NULL A_NOTHING
#define A_FLINT A_LONG

#define atom_getflintarg atom_getintarg
#define atom_getsymbolarg atom_getsymarg

#define add_dsp(clss,meth) addmess((method)meth,"dsp",A_CANT,A_NOTHING)
#define add_bang(clss,meth) addbang((method)meth)
#define add_float(clss,meth) addfloat((method)meth)
#define add_floatn(clss,meth,n) addftx((method)meth,n)
#define add_flint(clss,meth) addint((method)meth)
#define add_flintn(clss,meth,n) addinx((method)meth,n)
#define add_method(clss,meth,text) addmess((method)meth, text, A_NOTHING)
#define add_methodG(clss,meth,text) addmess((method)meth, text, A_GIMME,A_NOTHING)
#define add_method1(clss,meth,text,a1) addmess((method)meth, text, a1,A_NOTHING)
#define add_method2(clss,meth,text,a1,a2) addmess((method)meth, text, a1,a2,A_NOTHING)
#define add_method3(clss,meth,text,a1,a2,a3) addmess((method)meth, text, a1,a2,a3,A_NOTHING)
#define add_method4(clss,meth,text,a1,a2,a3,a4) addmess((method)meth, text, a1,a2,a3,a4,A_NOTHING)
#define add_method5(clss,meth,text,a1,a2,a3,a5) addmess((method)meth, text, a1,a2,a3,a4,a5,A_NOTHING)

#define add_assist(clss,meth) addmess((method)meth, "assist", A_CANT, A_NULL)
#define add_loadbang(clss,meth) addmess((method)meth, "loadbang", A_CANT, A_NULL)

#define newout_signal(clss) outlet_new(clss,"signal")
#define newout_float(clss) outlet_new(clss,"float")
#define newout_flint(clss) outlet_new(clss,"int")
#define newout_list(clss) outlet_new(clss,"list")
#define newout_symbol(clss) outlet_new(clss,"symbol")

#define outlet_flint(o,v) outlet_int(o,v)

#endif


#ifdef _LOG
#define LOG(s) post(s)
#define LOG1(s,v1) post(s,v1)
#define LOG2(s,v1,v2) post(s,v1,v2)
#define LOG3(s,v1,v2,v3) post(s,v1,v2,v3)
#define LOG4(s,v1,v2,v3,v4) post(s,v1,v2,v3,v4)
#define LOG5(s,v1,v2,v3,v4,v5) post(s,v1,v2,v3,v4,v5)
#else
#define LOG(s) ((void)0)
#define LOG1(s,v1) ((void)0)
#define LOG2(s,v1,v2) ((void)0)
#define LOG3(s,v1,v2,v3) ((void)0)
#define LOG4(s,v1,v2,v3,v4) ((void)0)
#define LOG5(s,v1,v2,v3,v4,v5) ((void)0)
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


