/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// definitions for internal flext usage


#ifndef __FLEXT_DEFS_H
#define __FLEXT_DEFS_H

#include <flstdc.h>


#ifdef PD

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

#define outlet_flint(o,v) outlet_float(o,v)

typedef t_perfroutine t_dspmethod;


#elif defined(MAXMSP)

/*
typedef void _inlet;
typedef _inlet t_inlet;
*/
  
typedef void t_outlet;
//typedef _outlet t_outlet;

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
#define newout_anything(clss) outlet_new(clss,"anything")

#define outlet_flint(o,v) outlet_int(o,v)
#define outlet_symbol(o,s) outlet_anything(o,s,0,NULL)

typedef t_perfroutine t_dspmethod;


#endif


#endif
