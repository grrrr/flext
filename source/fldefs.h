/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fldefs.h
    \brief This file contains all #defines for actual usage
    
*/

#ifndef __FLEXT_DEFS_H
#define __FLEXT_DEFS_H

#ifdef FLEXT_GUI
#define FLEXT_CAST dynamic_cast
#else
#define FLEXT_CAST static_cast
#endif

/*!	\defgroup FLEXT_HEADER Flext class header
	One (and only one!) of these definitions is compulsory for the class declaration. 
	It has to be placed somewhere in the class definition (not necessarily in a public section).

	@{ 
*/

/*! \brief Plain flext class header
	\param NEW_CLASS name of the current C++ class
	\param PARENT_CLASS name of the base C++ class (e.g. flext_base or flext_dsp)
*/
#define FLEXT_HEADER(NEW_CLASS,PARENT_CLASS) \
\
FLEXT_REALHDR(NEW_CLASS, PARENT_CLASS)    	    	

/*! \brief Flext class header with setup function
	\param NEW_CLASS name of the current C++ class
	\param PARENT_CLASS name of the base C++ class (e.g. flext_base or flext_dsp)
	\param SETUPFUN setup function, of type "void (*setupfn)(t_class *)"

	The setup function is called after class creation. It corresponds to the
	original "object_setup" function, apart from the
	fact that all necessary class initializations have already been taken care by flext. 
	The setup function can e.g. be used for a message to the console upon first creation of an object.
	The object's t_class* data can be accessed by the static function thisClass()
*/
#define FLEXT_HEADER_S(NEW_CLASS, PARENT_CLASS, SETUPFUN)\
\
FLEXT_REALHDR_S(NEW_CLASS, PARENT_CLASS, SETUPFUN)    	    	


//! @} 



// ====================================================================================

/*!	\defgroup FLEXT_NEW Flext class instantiation
	Makes an actual instance of a class.
*/

/*!	\defgroup FLEXT_NEW_DSP Flext dsp class instantiation
	Makes an actual instance of a dsp (aka "tilde") class (with signal processing).
*/

/*!	\defgroup FLEXT_LIB Flext library class instantiation
	Makes an actual instance of a class	which is part of an object library (and not stand-alone).
*/

/*!	\defgroup FLEXT_LIB_DSP Flext dsp library class instantiation
	Makes an actual instance of a dsp (aka "tilde") class with signal processing
	which is part of an object library (and not stand-alone).
*/

// NO ARGUMENTS
// ----------------------------------------

/*! Implementation of a flext class with no arguments
	\ingroup FLEXT_NEW
	\param NAME the object's actual name(s) as a string (like "*", "trigger", "noise~", etc.)
	\param NEW_CLASS the object's C++ class name 
*/
#define FLEXT_NEW(NAME,NEW_CLASS)		\
\
REAL_NEW(NAME,NEW_CLASS,0,0)

/*! Implementation of a flext dsp class with no arguments
	\ingroup FLEXT_NEW_DSP
*/
#define FLEXT_NEW_DSP(NAME,NEW_CLASS)	\
\
REAL_NEW(NAME,NEW_CLASS,1,0)

/*! Implementation of a flext class (part of a library) with no arguments
	\ingroup FLEXT_LIB
*/
#define FLEXT_LIB(NAME,NEW_CLASS) \
\
REAL_NEW(NAME,NEW_CLASS,0,1) 

/*! Implementation of a flext dsp class (part of a library) with no arguments
	\ingroup FLEXT_LIB_DSP
*/
#define FLEXT_LIB_DSP(NAME,NEW_CLASS)	\
\
REAL_NEW(NAME,NEW_CLASS,1,1) 


// VARIABLE ARGUMENT LIST
// ----------------------------------------

/*! Implementation of a flext class with a variable argument list
	\ingroup FLEXT_NEW
*/
#define FLEXT_NEW_V(NAME,NEW_CLASS)			\
\
REAL_NEW_V(NAME,NEW_CLASS,0,0) \

/*! Implementation of a flext tilde class with a variable argument list
	\ingroup FLEXT_NEW_DSP
*/
#define FLEXT_NEW_DSP_V(NAME,NEW_CLASS)	\
\
REAL_NEW_V(NAME,NEW_CLASS,1,0) \

/*! Implementation of a flext class (part of a library) with a variable argument list
	\ingroup FLEXT_LIB
*/
#define FLEXT_LIB_V(NAME,NEW_CLASS)			\
\
REAL_NEW_V(NAME,NEW_CLASS, 0,1) 

/*! Implementation of a flext tilde class (part of a library) with a variable argument list
	\ingroup FLEXT_LIB_DSP
*/
#define FLEXT_LIB_DSP_V(NAME,NEW_CLASS)	\
\
REAL_NEW_V(NAME,NEW_CLASS, 1,1) 


// OBJECT NAME + VARIABLE ARGUMENT LIST
// ----------------------------------------

/*! Implementation of a flext class with its name and a variable argument list
	\ingroup FLEXT_NEW
*/
#define FLEXT_NEW_NV(NAME,NEW_CLASS)			\
\
REAL_NEW_NV(NAME,NEW_CLASS,0,0) \

/*! Implementation of a flext tilde class with its name and a variable argument list
	\ingroup FLEXT_NEW_DSP
*/
#define FLEXT_NEW_DSP_NV(NAME,NEW_CLASS)	\
\
REAL_NEW_NV(NAME,NEW_CLASS,1,0) \

/*! Implementation of a flext class (part of a library) with its name and a variable argument list
	\ingroup FLEXT_LIB
*/
#define FLEXT_LIB_NV(NAME,NEW_CLASS)			\
\
REAL_NEW_NV(NAME,NEW_CLASS, 0,1) 

/*! Implementation of a flext tilde class (part of a library) with its name and a variable argument list
	\ingroup FLEXT_LIB_DSP
*/
#define FLEXT_LIB_DSP_NV(NAME,NEW_CLASS)	\
\
REAL_NEW_NV(NAME,NEW_CLASS, 1,1) 


// ONE ARGUMENT
// ----------------------------------------

/*! Implementation of a flext class with one argument
	\ingroup FLEXT_NEW
*/
#define FLEXT_NEW_1(NAME,NEW_CLASS, TYPE)		\
\
REAL_NEW_1(NAME,NEW_CLASS, 0, 0,TYPE) \

/*! Implementation of a flext tilde class with one argument
	\ingroup FLEXT_NEW_DSP
*/
#define FLEXT_NEW_DSP_1(NAME,NEW_CLASS, TYPE)	\
\
REAL_NEW_1(NAME,NEW_CLASS, 1, 0,TYPE) \

/*! Implementation of a flext class (part of a library) with one argument
	\ingroup FLEXT_LIB
*/
#define FLEXT_LIB_1(NAME,NEW_CLASS, TYPE) \
\
REAL_NEW_1(NAME,NEW_CLASS, 0,1,TYPE)

/*! Implementation of a flext tilde class (part of a library) with one argument
	\ingroup FLEXT_LIB_DSP
*/
#define FLEXT_LIB_DSP_1(NAME,NEW_CLASS, TYPE)	\
\
REAL_NEW_1(NAME,NEW_CLASS, 1,1, TYPE)


// TWO ARGUMENTS
// ----------------------------------------

/*! Implementation of a flext class with 2 arguments
	\ingroup FLEXT_NEW
*/
#define FLEXT_NEW_2(NAME,NEW_CLASS, TYPE1, TYPE2)			\
\
REAL_NEW_2(NAME,NEW_CLASS, 0,0, TYPE1, TYPE2) \

/*! Implementation of a flext tilde class with 2 arguments
	\ingroup FLEXT_NEW_DSP
*/
#define FLEXT_NEW_DSP_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
\
REAL_NEW_2(NAME,NEW_CLASS, 1,0, TYPE1, TYPE2) \

/*! Implementation of a flext class (part of a library) with 2 arguments
	\ingroup FLEXT_LIB
*/
#define FLEXT_LIB_2(NAME,NEW_CLASS, TYPE1, TYPE2)		\
\
REAL_NEW_2(NAME,NEW_CLASS, 0,1, TYPE1, TYPE2)

/*! Implementation of a flext tilde class (part of a library) with 2 arguments
	\ingroup FLEXT_LIB_DSP
*/
#define FLEXT_LIB_DSP_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
\
REAL_NEW_2(NAME,NEW_CLASS, 1,1, TYPE1, TYPE2)


// THREE ARGUMENTS
// ----------------------------------------

/*! Implementation of a flext class with 3 arguments
	\ingroup FLEXT_NEW
*/
#define FLEXT_NEW_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3) \
\
REAL_NEW_3(NAME,NEW_CLASS, 0,0, TYPE1, TYPE2, TYPE3)  \

/*! Implementation of a flext tilde class with 3 arguments
	\ingroup FLEXT_NEW_DSP
*/
#define FLEXT_NEW_DSP_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
\
REAL_NEW_3(NAME,NEW_CLASS, 1,0, TYPE1, TYPE2, TYPE3) \

/*! Implementation of a flext class (part of a library) with 3 arguments
	\ingroup FLEXT_LIB
*/
#define FLEXT_LIB_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)		\
\
REAL_NEW_3(NAME,NEW_CLASS, 0,1,TYPE1, TYPE2, TYPE3)

/*! Implementation of a flext tilde class (part of a library) with 3 arguments
	\ingroup FLEXT_LIB_DSP
*/
#define FLEXT_LIB_DSP_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
\
REAL_NEW_3(NAME,NEW_CLASS, 1,1, TYPE1, TYPE2, TYPE3)


// deprecated stuff

#define FLEXT_NEW_G FLEXT_NEW_V

#define FLEXT_NEW_TILDE FLEXT_NEW_DSP
#define FLEXT_NEW_TILDE_G FLEXT_NEW_DSP_V
#define FLEXT_NEW_TILDE_1 FLEXT_NEW_DSP_1
#define FLEXT_NEW_TILDE_2 FLEXT_NEW_DSP_2
#define FLEXT_NEW_TILDE_3 FLEXT_NEW_DSP_3

#define FLEXT_LIB_G FLEXT_LIB_V

#define FLEXT_LIB_TILDE FLEXT_LIB_DSP
#define FLEXT_LIB_TILDE_G FLEXT_LIB_DSP_V
#define FLEXT_LIB_TILDE_1 FLEXT_LIB_DSP_1
#define FLEXT_LIB_TILDE_2 FLEXT_LIB_DSP_2
#define FLEXT_LIB_TILDE_3 FLEXT_LIB_DSP_3




// ====================================================================================


/*!	\defgroup FLEXT_CALLBACK Flext callbacks

	@{ 
*/

//! Set up a method callback with no arguments
#define FLEXT_CALLBACK(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(); return true; }

//! Set up a method callback for an anything argument
#define FLEXT_CALLBACK_A(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,const t_symbol *s,int argc,t_atom *argv) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(s,argc,argv); return true; }

//! Set up a method callback for a variable argument list
#define FLEXT_CALLBACK_V(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,int argc,t_atom *argv) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(argc,argv); return true; }

//! Set up a method callback for a data package (void *) argument
#define FLEXT_CALLBACK_X(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,void *data) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(data); return true; }

//! Set up a method callback for a boolean argument
#define FLEXT_CALLBACK_B(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,int &arg1) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(arg1 != 0); return true; }

//! Set up a method callback for 1 argument
#define FLEXT_CALLBACK_1(M_FUN,TP1) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(arg1); return true; }

//! Set up a method callback for 2 arguments
#define FLEXT_CALLBACK_2(M_FUN,TP1,TP2) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(arg1,arg2); return true; }

//! Set up a method callback for 3 arguments
#define FLEXT_CALLBACK_3(M_FUN,TP1,TP2,TP3) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(arg1,arg2,arg3); return true; }

//! Set up a method callback for 4 arguments
#define FLEXT_CALLBACK_4(M_FUN,TP1,TP2,TP3,TP4) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4); return true; }

//! Set up a method callback for 5 arguments
#define FLEXT_CALLBACK_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4,TP5 &arg5) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4,arg5); return true; }


//	Shortcuts

//! Set up a method callback for 1 float argument
#define FLEXT_CALLBACK_F(M_FUN) \
\
FLEXT_CALLBACK_1(M_FUN,float)

//! Set up a method callback for 2 float arguments
#define FLEXT_CALLBACK_FF(M_FUN) \
\
FLEXT_CALLBACK_2(M_FUN,float,float)

//! Set up a method callback for 3 float arguments
#define FLEXT_CALLBACK_FFF(M_FUN) \
\
FLEXT_CALLBACK_3(M_FUN,float,float,float)

//! Set up a method callback for 1 integer argument
#define FLEXT_CALLBACK_I(M_FUN) \
\
FLEXT_CALLBACK_1(M_FUN,int)

//! Set up a method callback for 2 integer arguments
#define FLEXT_CALLBACK_II(M_FUN) \
\
FLEXT_CALLBACK_2(M_FUN,int,int)

//! Set up a method callback for 3 integer arguments
#define FLEXT_CALLBACK_III(M_FUN) \
\
FLEXT_CALLBACK_3(M_FUN,int,int,int)

//! Set up a method callback for 1 symbol argument
#define FLEXT_CALLBACK_S(M_FUN) \
\
FLEXT_CALLBACK_1(M_FUN,t_symptr)


// deprecated
#define FLEXT_CALLBACK_G FLEXT_CALLBACK_V

//! @} 



/*!	\defgroup FLEXT_THREAD Flext threads

	@{ 
*/

#ifdef FLEXT_THREADS

//! Set up a threaded method callback with no arguments
#define FLEXT_THREAD(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c) {  \
	thr_params *p = new thr_params(c); \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	delete p; \
	if(ok) { \
		th->M_FUN(); \
		th->PopThread(); \
	} \
	return NULL; \
} 

//! Set up a threaded method callback for an anything argument
#define FLEXT_THREAD_A(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,const t_symbol *s,int argc,t_atom *argv) {  \
	thr_params *p = new thr_params(c); p->set_any(s,argc,argv); \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	AtomAnything *args = p->var[0]._any.args; \
	delete p; \
	if(ok) { \
		th->M_FUN(args->Header(),args->Count(),args->Atoms()); \
		th->PopThread(); \
	} \
	delete args; \
	return NULL; \
} 

//! Set up a threaded method callback for a variable argument list
#define FLEXT_THREAD_V(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,int argc,t_atom *argv) {  \
	thr_params *p = new thr_params(c); p->set_list(argc,argv); \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	AtomList *args = p->var[0]._list.args; \
	delete p; \
	if(ok) { \
		th->M_FUN(args->Count(),args->Atoms()); \
		th->PopThread(); \
	} \
	delete args; \
	return NULL; \
} 

/*! \brief Set up a threaded method callback for an arbitrary data struct
	\note Data is pure... no destructor is called upon delete
*/
#define FLEXT_THREAD_X(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,void *data) {  \
	thr_params *p = new thr_params(c); p->var[0]._ext.data = data; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	void *data = p->var[0]._ext.data; \
	delete p; \
	if(ok) { \
		th->M_FUN(data); \
		th->PopThread(); \
	} \
	delete data; \
	return NULL; \
} 

//! Set up a threaded method callback for a boolean argument
#define FLEXT_THREAD_B(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,int &arg1) {  \
	thr_params *p = new thr_params(c); p->var[0]._bool = arg1 != 0; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	bool b = p->var[0]._bool; \
	delete p; \
	if(ok) { \
		th->M_FUN(b); \
		th->PopThread(); \
	} \
	return NULL; \
} 

//! Set up a threaded method callback for 1 argument
#define FLEXT_THREAD_1(M_FUN,TP1) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1) {  \
	thr_params *p = new thr_params(c,1); \
	p->var[0]._ ## TP1 = arg1; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1); \
		th->PopThread(); \
	} \
	return NULL; \
} 

//! Set up a threaded method callback for 2 arguments
#define FLEXT_THREAD_2(M_FUN,TP1,TP2) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2) {  \
	thr_params *p = new thr_params(c,2); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP1 v2 = p->var[1]._ ## TP2; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1,v2); \
		th->PopThread(); \
	} \
	return NULL; \
} 

//! Set up a threaded method callback for 3 arguments
#define FLEXT_THREAD_3(M_FUN,TP1,TP2,TP3) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3) {  \
	thr_params *p = new thr_params(c,3); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP2 v2 = p->var[1]._ ## TP2; \
	const TP3 v3 = p->var[2]._ ## TP3; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1,v2,v3); \
		th->PopThread(); \
	} \
	return NULL; \
} 

//! Set up a threaded method callback for 4 arguments
#define FLEXT_THREAD_4(M_FUN,TP1,TP2,TP3,TP4) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4) {  \
	thr_params *p = new thr_params(c,4); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	p->var[3]._ ## TP4 = arg4; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP2 v2 = p->var[1]._ ## TP2; \
	const TP3 v3 = p->var[2]._ ## TP3; \
	const TP4 v4 = p->var[3]._ ## TP4; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1,v2,v3,v4); \
		th->PopThread(); \
	} \
	return NULL; \
} 

//! Set up a threaded method callback for 5 arguments
#define FLEXT_THREAD_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4,TP5 &arg5) {  \
	thr_params *p = new thr_params(c,5); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	p->var[3]._ ## TP4 = arg4; \
	p->var[4]._ ## TP5 = arg5; \
	return StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void *FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP2 v2 = p->var[1]._ ## TP2; \
	const TP3 v3 = p->var[2]._ ## TP3; \
	const TP4 v4 = p->var[3]._ ## TP4; \
	const TP5 v5 = p->var[4]._ ## TP5; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1,v2,v3,v4,v5); \
		th->PopThread(); \
	} \
	return NULL; \
} 


//!	Shortcuts

//! Set up a threaded method callback for 1 float argument
#define FLEXT_THREAD_F(M_FUN) \
\
FLEXT_THREAD_1(M_FUN,float)

//! Set up a threaded method callback for 2 float arguments
#define FLEXT_THREAD_FF(M_FUN) \
\
FLEXT_THREAD_2(M_FUN,float,float)

//! Set up a threaded method callback for 3 float arguments
#define FLEXT_THREAD_FFF(M_FUN) \
\
FLEXT_THREAD_3(M_FUN,float,float,float)

//! Set up a threaded method callback for 1 integer argument
#define FLEXT_THREAD_I(M_FUN) \
\
FLEXT_THREAD_1(M_FUN,int)

//! Set up a threaded method callback for 2 integer arguments
#define FLEXT_THREAD_II(M_FUN) \
\
FLEXT_THREAD_2(M_FUN,int,int)

//! Set up a threaded method callback for 3 integer arguments
#define FLEXT_THREAD_III(M_FUN) \
\
FLEXT_THREAD_3(M_FUN,int,int,int)

//! Set up a threaded method callback for 1 symbol argument
#define FLEXT_THREAD_S(M_FUN) \
\
FLEXT_THREAD_1(M_FUN,t_symptr)

// deprecated
#define FLEXT_THREAD_G FLEXT_THREAD_V

#endif // FLEXT_THREAD

//! @} 



// ====================================================================================



/*!	\defgroup FLEXT_ADDMETHOD Add flext methods
	These should be the used in the class' constructor

	@{ 
*/

//! Enable list element distribution over inlets (if no better handler found)
#define FLEXT_ADDDIST() \
\
SetDist(true)	

//! Add a method handler for bang 
#define FLEXT_ADDBANG(IX,M_FUN) \
\
AddMethod(IX,"bang",FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with no arguments
#define FLEXT_ADDMETHOD(IX,M_FUN) \
AddMethod(IX,FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with a (variable argument) list
#define FLEXT_ADDMETHOD_V(IX,M_FUN) \
\
AddMethod(IX,FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with an anything argument
#define FLEXT_ADDMETHOD_A(IX,M_FUN) \
\
AddMethod(IX,FLEXT_CALL_PRE(M_FUN))	

//! Add a a handler for a method with implicit arguments
#define FLEXT_ADDMETHOD_(IX,M_TAG,M_FUN) \
\
AddMethod(IX,M_TAG,FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with 1 enum type argument
#define FLEXT_ADDMETHOD_E(IX,M_TAG,M_FUN) \
\
AddMethod(IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),a_int,a_null)

//! Add a handler for a method with 1 argument
#define FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,TP1) \
\
AddMethod(IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),a_null)	

//! Add a handler for a method with 2 arguments
#define FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,TP1,TP2) \
\
AddMethod(IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),a_null)

//! Add a handler for a method with 3 arguments
#define FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,TP1,TP2,TP3) \
\
AddMethod(IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),a_null)

//! Add a handler for a method with 4 arguments
#define FLEXT_ADDMETHOD_4(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4) \
\
AddMethod(IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),a_null)

//! Add a handler for a method with 5 arguments
#define FLEXT_ADDMETHOD_5(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4,TP5) \
\
AddMethod(IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),FLEXTARG(TP5),a_null)


//	Shortcuts

//! Add a handler for a method with a boolean argument
#define FLEXT_ADDMETHOD_B(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,bool)

//! Add a handler for a method with 1 float argument
#define FLEXT_ADDMETHOD_F(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,float)

//! Add a handler for a method with 2 float arguments
#define FLEXT_ADDMETHOD_FF(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,float,float)

//! Add a handler for a method with 3 float arguments
#define FLEXT_ADDMETHOD_FFF(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,float,float,float)

//! Add a handler for a method with 1 integer argument
#define FLEXT_ADDMETHOD_I(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,int)

//! Add a handler for a method with 2 integer arguments
#define FLEXT_ADDMETHOD_II(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,int,int)

//! Add a handler for a method with 3 integer arguments
#define FLEXT_ADDMETHOD_III(IX,M_TAG,M_FUN) \
\
FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,int,int,int)

//! @} 



/*!	\defgroup FLEXT_CALLMETHOD Call flext methods manually
	These can only be used in class member functions!

	@{ 
*/

//! Call a (already defined) method with no arguments
#define FLEXT_CALLMETHOD(M_FUN) \
\
FLEXT_CALL_PRE(M_FUN)(this)

//! Call a (already defined) method with variable list arguments
#define FLEXT_CALLMETHOD_V(M_FUN,ARGC,ARGV) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARGC,ARGV)

//! Call a (already defined) method with anything arguments
#define FLEXT_CALLMETHOD_A(M_FUN,HDR,ARGC,ARGV) \
\
FLEXT_CALL_PRE(M_FUN)(this,HDR,ARGC,ARGV)

//! Call a (already defined) method with a data package (void *)
#define FLEXT_CALLMETHOD_X(M_FUN,DATA) \
\
FLEXT_CALL_PRE(M_FUN)(this,DATA)

//! Call a (already defined) method with 1 enum type argument
#define FLEXT_CALLMETHOD_E(M_FUN,ARG) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARG)

//! Call a (already defined) method with 1 argument
#define FLEXT_CALLMETHOD_1(M_FUN,ARG) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARG)

//! Call a (already defined) method with 2 arguments
#define FLEXT_CALLMETHOD_2(M_FUN,ARG1,ARG2) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARG1,ARG2)

//! Call a (already defined) method with 3 arguments
#define FLEXT_CALLMETHOD_3(M_FUN,ARG1,ARG2,ARG3) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARG1,ARG2,ARG3)

//! Call a (already defined) method with 4 arguments
#define FLEXT_CALLMETHOD_4(M_FUN,ARG1,ARG2,ARG3,ARG4) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARG1,ARG2,ARG3,ARG4)

//! Call a (already defined) method with 5 arguments
#define FLEXT_CALLMETHOD_5(M_FUN,ARG1,ARG2,ARG3,ARG4,ARG5) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARG1,ARG2,ARG3,ARG4,ARG5)

//! @} 



/*!	\defgroup FLEXT_DEFATTR flext functions for attribute definition

	@{ 
*/

#define FLEXT_CALLSET_(FUN,TP) \
static bool FLEXT_SET_PRE(FUN)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(arg); return true; }

#define FLEXT_CALLGET_(FUN,TP) \
static bool FLEXT_GET_PRE(FUN)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(arg); return true; }

#define FLEXT_ATTRSET_(VAR,TP) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = arg; return true; }

#define FLEXT_ATTRGET_(VAR,TP) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,TP &arg) \
{ arg = FLEXT_CAST<thisType *>(c)->VAR; return true; }

#define FLEXT_CALLSET_F(SFUN) FLEXT_CALLSET_(SFUN,float)
#define FLEXT_CALLSET_I(SFUN) FLEXT_CALLSET_(SFUN,int)
#define FLEXT_CALLSET_S(SFUN) FLEXT_CALLSET_(SFUN,t_symptr)

#define FLEXT_CALLSET_V(FUN) \
static bool FLEXT_SET_PRE(FUN)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(*arg); return true; }

#define FLEXT_CALLGET_F(GFUN) FLEXT_CALLGET_(GFUN,float)
#define FLEXT_CALLGET_I(GFUN) FLEXT_CALLGET_(GFUN,int)
#define FLEXT_CALLGET_S(GFUN) FLEXT_CALLGET_(GFUN,t_symptr)

#define FLEXT_CALLGET_V(FUN) \
static bool FLEXT_GET_PRE(FUN)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(*arg); return true; }

#define FLEXT_CALLXFER_F(GFUN,SFUN) FLEXT_CALLGET_F(GFUN) FLEXT_CALLSET_F(SFUN) 
#define FLEXT_CALLXFER_I(GFUN,SFUN) FLEXT_CALLGET_I(GFUN) FLEXT_CALLSET_I(SFUN) 
#define FLEXT_CALLXFER_S(GFUN,SFUN) FLEXT_CALLGET_S(GFUN) FLEXT_CALLSET_S(SFUN) 
#define FLEXT_CALLXFER_V(GFUN,SFUN) FLEXT_CALLGET_V(GFUN) FLEXT_CALLSET_V(SFUN) 

#define FLEXT_ATTRSET_F(VAR) FLEXT_ATTRSET_(VAR,float)
#define FLEXT_ATTRSET_I(VAR) FLEXT_ATTRSET_(VAR,int)
#define FLEXT_ATTRSET_S(VAR) FLEXT_ATTRSET_(VAR,t_symptr)

#define FLEXT_ATTRSET_V(VAR) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = *arg; return true; }

#define FLEXT_ATTRGET_F(VAR) FLEXT_ATTRGET_(VAR,float)
#define FLEXT_ATTRGET_I(VAR) FLEXT_ATTRGET_(VAR,int)
#define FLEXT_ATTRGET_S(VAR) FLEXT_ATTRGET_(VAR,t_symptr)

#define FLEXT_ATTRGET_V(VAR) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,AtomList *&arg) \
{ *arg = FLEXT_CAST<thisType *>(c)->VAR; return true; }

#define FLEXT_ATTRXFER_F(VAR) FLEXT_ATTRGET_F(VAR) FLEXT_ATTRSET_F(VAR) 
#define FLEXT_ATTRXFER_I(VAR) FLEXT_ATTRGET_I(VAR) FLEXT_ATTRSET_I(VAR) 
#define FLEXT_ATTRXFER_S(VAR) FLEXT_ATTRGET_S(VAR) FLEXT_ATTRSET_S(VAR) 
#define FLEXT_ATTRXFER_V(VAR) FLEXT_ATTRGET_V(VAR) FLEXT_ATTRSET_V(VAR) 

//! @} 

/*!	\defgroup FLEXT_ADDATTR flext methods for defining attributes
	These can only be used in class member functions!

	@{ 
*/

#define	FLEXT_ADDATTR_GET(NAME,GFUN) AddAttrib(NAME,(FLEXT_GET_PRE(GFUN)),NULL)
#define	FLEXT_ADDATTR_SET(NAME,SFUN) AddAttrib(NAME,NULL,(FLEXT_SET_PRE(SFUN)))
#define	FLEXT_ADDATTRIB(NAME,FUN) AddAttrib(NAME,(FLEXT_GET_PRE(FUN)),(FLEXT_SET_PRE(FUN)))

//! @} 




#endif // __FLEXT_DEFS_H
