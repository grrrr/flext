#ifndef __MAX_PD_H
#define __MAX_PD_H

#define F float 
#define D double
#define I int
#define L long
#define C char
#define BL bool
#define V void

#define BIGFLOAT 1.e10
#define BIGLONG 0x7fffffffL

#ifdef __MWERKS__
#define MAX  // how to define in CodeWarrior IDE?
#endif

#ifdef PD
#pragma warning (push)
#pragma warning (disable:4091)
extern "C" {	    	    	    	    	    	    	
#include <m_pd.h>
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
#define add_float1(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft1"),A_FLOAT,A_NULL)
#define add_float2(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft2"),A_FLOAT,A_NULL)
#define add_float3(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft3"),A_FLOAT,A_NULL)
#define add_float4(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft4"),A_FLOAT,A_NULL)
#define add_flint(clss,meth) class_addfloat(clss, (t_method)meth)
#define add_flint1(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft1"),A_FLOAT,A_NULL)
#define add_flint2(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft2"),A_FLOAT,A_NULL)
#define add_flint3(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft3"),A_FLOAT,A_NULL)
#define add_flint4(clss,meth) class_addmethod(clss, (t_method)meth,gensym("ft4"),A_FLOAT,A_NULL)
#define add_methodG(clss,meth,text) class_addmethod(clss, (t_method)meth, gensym(text), A_GIMME,A_NULL)
#define add_method0(clss,meth,text) class_addmethod(clss, (t_method)meth, gensym(text), A_NULL)
#define add_method1(clss,meth,text,a1) class_addmethod(clss, (t_method)meth, gensym(text), a1,A_NULL)
#define add_method2(clss,meth,text,a1,a2) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,A_NULL)
#define add_method3(clss,meth,text,a1,a2,a3) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,a3,A_NULL)
#define add_method4(clss,meth,text,a1,a2,a3,a4) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,a3,a4,A_NULL)
#define add_method5(clss,meth,text,a1,a2,a3,a5) class_addmethod(clss, (t_method)meth, gensym(text), a1,a2,a3,a4,a5,A_NULL)

#define newout_signal(clss) outlet_new(clss,&s_signal)
#define newout_float(clss) outlet_new(clss,&s_float)
#define newout_flint(clss) outlet_float(clss)
#define newout_list(clss) outlet_new(clss,&s_list)
#define newout_symbol(clss) outlet_new(clss,&s_symbol)


#elif defined(MAX)

extern "C"
{
#include "ext.h"
#include "ext_strings.h"
#include "ext_user.h"
#include "z_dsp.h"
#include "buffer.h"
}

typedef t_pxobject t_sigobj;

typedef t_int t_flint;

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
#define add_float1(clss,meth) addftx((method)meth,1)
#define add_float2(clss,meth) addftx((method)meth,2)
#define add_float3(clss,meth) addftx((method)meth,3)
#define add_float4(clss,meth) addftx((method)meth,4)
#define add_flint(clss,meth) addint((method)meth)
#define add_flint1(clss,meth) addinx((method)meth,1)
#define add_flint2(clss,meth) addinx((method)meth,2)
#define add_flint3(clss,meth) addinx((method)meth,3)
#define add_flint4(clss,meth) addinx((method)meth,4)
#define add_methodG(clss,meth,text) addmess((method)meth, text, A_GIMME,A_NOTHING)
#define add_method0(clss,meth,text) addmess((method)meth, text, A_NOTHING)
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

#endif

#define FI t_flint


#ifdef _LOG
#define LOG(s) post(s)
#define LOG1(s,v1) post(s,v1)
#define LOG2(s,v1,v2) post(s,v1,v2)
#define LOG3(s,v1,v2,v3) post(s,v1,v2,v3)
#define LOG4(s,v1,v2,v3,v4) post(s,v1,v2,v3,v4)
#define LOG5(s,v1,v2,v3,v4,v5) post(s,v1,v2,v3,v4,v5)
#else
#define LOG(s) ((V)0)
#define LOG1(s,v1) ((V)0)
#define LOG2(s,v1,v2) ((V)0)
#define LOG3(s,v1,v2,v3) ((V)0)
#define LOG4(s,v1,v2,v3,v4) ((V)0)
#define LOG5(s,v1,v2,v3,v4,v5) ((V)0)
#endif


#ifdef _WINDOWS
#define EXT_EXTERN __declspec(dllexport)
#else                   // other OS's
#define EXT_EXTERN
#endif

#endif

