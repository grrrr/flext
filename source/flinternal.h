/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flinternal.h
    \brief Definitions for internal flext usage
	\internal
    
    Here, a few shortcuts for common Max/MSP or PD library calls and type definitions 
	are declared
*/

#ifndef __FLEXT_INTERNALS_H
#define __FLEXT_INTERNALS_H

#include "flstdc.h"


#if FLEXT_SYS == FLEXT_SYS_PD

#define object_new(clss) pd_new(clss)
#define object_free(obj) pd_free(&(obj).ob_pd)
				


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
#define add_anything(clss,meth) class_addanything(clss,meth)


#define newout_signal(clss) outlet_new(clss,&s_signal)
#define newout_float(clss) outlet_new(clss,&s_float)
#define newout_flint(clss) outlet_new(clss,&s_float)
#define newout_list(clss) outlet_new(clss,&s_list)
#define newout_symbol(clss) outlet_new(clss,&s_symbol)
#define newout_anything(clss) outlet_new(clss,&s_anything)

#define outlet_flint(o,v) outlet_float(o,(float)(v))

typedef t_perfroutine t_dspmethod;

#define qelem_new clock_new
#define qelem_free clock_free
#define qelem_set clock_set
#define qelem_unset clock_unset

#define CRITON() 
#define CRITOFF() 


#elif FLEXT_SYS == FLEXT_SYS_MAX

/*
typedef void _inlet;
typedef _inlet t_inlet;
*/
  
typedef void t_outlet;
//typedef _outlet t_outlet;

#define object_new(clss) pd_new(clss)
#define object_free(obj) freeobject((object *)obj)

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
#define add_anything(clss,meth) addmess((method)meth, "anything", A_GIMME,A_NOTHING)

#define add_assist(clss,meth) addmess((method)meth, "assist", A_CANT, A_NULL)
#define add_loadbang(clss,meth) addmess((method)meth, "loadbang", A_CANT, A_NULL)

#define newout_signal(clss) outlet_new(clss,"signal")
#define newout_float(clss) outlet_new(clss,"float")
#define newout_flint(clss) outlet_new(clss,"int")
#define newout_list(clss) outlet_new(clss,"list")
#define newout_symbol(clss) outlet_new(clss,"symbol")
#define newout_anything(clss) outlet_new(clss,0)

#define outlet_flint(o,v) outlet_int(o,(int)(v))
#define outlet_symbol(o,s) outlet_anything(o,s,0,NULL)

typedef t_perfroutine t_dspmethod;

#define CRITON() short state = lockout_set(1)
#define CRITOFF() lockout_set(state) 

#elif FLEXT_SYS == FLEXT_SYS_JMAX


#endif


#endif
