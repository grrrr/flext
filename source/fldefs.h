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

/*!	\defgroup FLEXT_DEFS Definitions for basic flext functionality
	@{ 
*/

/*! \brief Switch for compilation of derived virtual classes
	\remark These need dynamic type casts (and RTTI, naturally)
	\ingroup FLEXT_GLOBALS
*/
#ifdef FLEXT_VIRT
#define FLEXT_CAST dynamic_cast
#else
#define FLEXT_CAST static_cast
#endif

/*!	\defgroup FLEXT_D_HEADER Flext class header
	\note One (and only one!) of these definitions is compulsory for the class declaration. 
	\note It has to be placed somewhere in the class definition (not necessarily in a public section).

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
	original PD "[object]_setup" function, apart from the
	fact that all necessary class initializations have already been taken care of by flext. 
	The setup function can e.g. be used for a message to the console upon first creation of an object.
*/
#define FLEXT_HEADER_S(NEW_CLASS, PARENT_CLASS, SETUPFUN)\
\
FLEXT_REALHDR_S(NEW_CLASS, PARENT_CLASS, SETUPFUN)    	    	


//! @} FLEXT_D_HEADER



// ====================================================================================

/*!	\defgroup FLEXT_D_INSTANCE Class instantiation
*/


/*!	\defgroup FLEXT_D_NEW Stand-alone class instantiation
	Makes an actual instance of a stand-alone class.
*/

/*!	\defgroup FLEXT_D_NEW_DSP Dsp class instantiation
	Makes an actual instance of a dsp (aka "tilde") class (with signal processing).
*/

/*!	\defgroup FLEXT_D_LIB Library class instantiation
	Makes an actual instance of a class	which is part of an object library (and not stand-alone).
*/

/*!	\defgroup FLEXT_D_LIB_DSP Dsp library class instantiation
	Makes an actual instance of a dsp (aka "tilde") class with signal processing
	which is part of an object library (and not stand-alone).
*/

// NO ARGUMENTS
// ----------------------------------------

/*! \brief Implementation of a flext class with no arguments
	\ingroup FLEXT_D_NEW
	\param NAME the object's actual name(s) as a string (like "*", "trigger", "noise~", etc.)
	\param NEW_CLASS the object's C++ class name 
*/
#define FLEXT_NEW(NAME,NEW_CLASS)		\
\
REAL_NEW(NAME,NEW_CLASS,0,0)

/*! \brief Implementation of a flext dsp class with no arguments
	\ingroup FLEXT_D_NEW_DSP
*/
#define FLEXT_NEW_DSP(NAME,NEW_CLASS)	\
\
REAL_NEW(NAME,NEW_CLASS,1,0)

/*! \brief Implementation of a flext class (part of a library) with no arguments
	\ingroup FLEXT_D_LIB
*/
#define FLEXT_LIB(NAME,NEW_CLASS) \
\
REAL_NEW(NAME,NEW_CLASS,0,1) 

/*! \brief Implementation of a flext dsp class (part of a library) with no arguments
	\ingroup FLEXT_D_LIB_DSP
*/
#define FLEXT_LIB_DSP(NAME,NEW_CLASS)	\
\
REAL_NEW(NAME,NEW_CLASS,1,1) 


// VARIABLE ARGUMENT LIST
// ----------------------------------------

/*! \brief Implementation of a flext class with a variable argument list
	\ingroup FLEXT_D_NEW
*/
#define FLEXT_NEW_V(NAME,NEW_CLASS)			\
\
REAL_NEW_V(NAME,NEW_CLASS,0,0) \

/*! \brief Implementation of a flext dsp class with a variable argument list
	\ingroup FLEXT_D_NEW_DSP
*/
#define FLEXT_NEW_DSP_V(NAME,NEW_CLASS)	\
\
REAL_NEW_V(NAME,NEW_CLASS,1,0) \

/*! \brief Implementation of a flext class (part of a library) with a variable argument list
	\ingroup FLEXT_D_LIB
*/
#define FLEXT_LIB_V(NAME,NEW_CLASS)			\
\
REAL_NEW_V(NAME,NEW_CLASS, 0,1) 

/*! \brief Implementation of a flext dsp class (part of a library) with a variable argument list
	\ingroup FLEXT_D_LIB_DSP
*/
#define FLEXT_LIB_DSP_V(NAME,NEW_CLASS)	\
\
REAL_NEW_V(NAME,NEW_CLASS, 1,1) 


// ONE ARGUMENT
// ----------------------------------------

/*! \brief Implementation of a flext class with one argument
	\ingroup FLEXT_D_NEW
*/
#define FLEXT_NEW_1(NAME,NEW_CLASS, TYPE)		\
\
REAL_NEW_1(NAME,NEW_CLASS, 0, 0,TYPE) \

/*! \brief Implementation of a flext dsp class with one argument
	\ingroup FLEXT_D_NEW_DSP
*/
#define FLEXT_NEW_DSP_1(NAME,NEW_CLASS, TYPE)	\
\
REAL_NEW_1(NAME,NEW_CLASS, 1, 0,TYPE) \

/*! \brief Implementation of a flext class (part of a library) with one argument
	\ingroup FLEXT_D_LIB
*/
#define FLEXT_LIB_1(NAME,NEW_CLASS, TYPE) \
\
REAL_NEW_1(NAME,NEW_CLASS, 0,1,TYPE)

/*! \brief Implementation of a flext dsp class (part of a library) with one argument
	\ingroup FLEXT_D_LIB_DSP
*/
#define FLEXT_LIB_DSP_1(NAME,NEW_CLASS, TYPE)	\
\
REAL_NEW_1(NAME,NEW_CLASS, 1,1, TYPE)


// TWO ARGUMENTS
// ----------------------------------------

/*! \brief Implementation of a flext class with 2 arguments
	\ingroup FLEXT_D_NEW
*/
#define FLEXT_NEW_2(NAME,NEW_CLASS, TYPE1, TYPE2)			\
\
REAL_NEW_2(NAME,NEW_CLASS, 0,0, TYPE1, TYPE2) \

/*! \brief Implementation of a flext dsp class with 2 arguments
	\ingroup FLEXT_D_NEW_DSP
*/
#define FLEXT_NEW_DSP_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
\
REAL_NEW_2(NAME,NEW_CLASS, 1,0, TYPE1, TYPE2) \

/*! \brief Implementation of a flext class (part of a library) with 2 arguments
	\ingroup FLEXT_D_LIB
*/
#define FLEXT_LIB_2(NAME,NEW_CLASS, TYPE1, TYPE2)		\
\
REAL_NEW_2(NAME,NEW_CLASS, 0,1, TYPE1, TYPE2)

/*! \brief Implementation of a flext dsp class (part of a library) with 2 arguments
	\ingroup FLEXT_D_LIB_DSP
*/
#define FLEXT_LIB_DSP_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
\
REAL_NEW_2(NAME,NEW_CLASS, 1,1, TYPE1, TYPE2)


// THREE ARGUMENTS
// ----------------------------------------

/*! \brief Implementation of a flext class with 3 arguments
	\ingroup FLEXT_D_NEW
*/
#define FLEXT_NEW_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3) \
\
REAL_NEW_3(NAME,NEW_CLASS, 0,0, TYPE1, TYPE2, TYPE3)  \

/*! \brief Implementation of a flext dsp class with 3 arguments
	\ingroup FLEXT_D_NEW_DSP
*/
#define FLEXT_NEW_DSP_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
\
REAL_NEW_3(NAME,NEW_CLASS, 1,0, TYPE1, TYPE2, TYPE3) \

/*! \brief Implementation of a flext class (part of a library) with 3 arguments
	\ingroup FLEXT_D_LIB
*/
#define FLEXT_LIB_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)		\
\
REAL_NEW_3(NAME,NEW_CLASS, 0,1,TYPE1, TYPE2, TYPE3)

/*! \brief Implementation of a flext dsp class (part of a library) with 3 arguments
	\ingroup FLEXT_D_LIB_DSP
*/
#define FLEXT_LIB_DSP_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
\
REAL_NEW_3(NAME,NEW_CLASS, 1,1, TYPE1, TYPE2, TYPE3)


// deprecated stuff

/*!	\defgroup FLEXT_D_DEPRECATED Deprecated definitions 
	@{ 

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


#define FLEXT_TILDE_SETUP FLEXT_DSP_SETUP

//! @} FLEXT_D_DEPRECATED


/*!	\defgroup FLEXT_D_LIBRARY Definitions for library objects
	@{ 
*/

/*! \brief Specify that to declare the library itself
	\note If you have a library this is compulsory (to register all the objects of the library)
*/
#define FLEXT_LIB_SETUP(NAME,SETUPFUN) REAL_LIB_SETUP(NAME,SETUPFUN)

/*! \brief Register an object in the library
	\note This is used in the library setup function
*/
#define FLEXT_SETUP(cl) REAL_SETUP(cl,0)

/*! \brief Register a DSP object in the library
	\note This is used in the library setup function
*/
#define FLEXT_DSP_SETUP(cl) REAL_SETUP(cl,1)

//! @} FLEXT_D_LIBRARY 


//! @} FLEXT_D_INSTANCE

// ====================================================================================

/*!	\defgroup FLEXT_D_METHOD Declarations for flext methods
	@{ 
*/


/*!	\defgroup FLEXT_D_CALLBACK Declare callbacks for class methods
	@{ 
*/

//! Set up a method callback with no arguments
#define FLEXT_CALLBACK(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c) \
{ FLEXT_CAST<thisType *>(c)->M_FUN(); return true; }

//! Set up a method callback for an anything argument
#define FLEXT_CALLBACK_A(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,t_symbol *s,int argc,t_atom *argv) \
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


//! \deprecated
#define FLEXT_CALLBACK_G FLEXT_CALLBACK_V

//! @} FLEXT_D_CALLBACK



/*!	\defgroup FLEXT_D_THREAD Declare threaded method callbacks
	@{ 
*/

#ifdef FLEXT_THREADS

//! Set up a threaded method callback with no arguments
#define FLEXT_THREAD(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c) {  \
	thr_params *p = new thr_params; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	delete p; \
	if(ok) { \
		th->M_FUN(); \
		th->PopThread(); \
	} \
} 

//! Set up a threaded method callback for an anything argument
#define FLEXT_THREAD_A(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,t_symbol *s,int argc,t_atom *argv) {  \
	thr_params *p = new thr_params; p->set_any(s,argc,argv); \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	AtomAnything *args = p->var[0]._any.args; \
	delete p; \
	if(ok) { \
		th->M_FUN(args->Header(),args->Count(),args->Atoms()); \
		th->PopThread(); \
	} \
	delete args; \
} 

//! Set up a threaded method callback for a variable argument list
#define FLEXT_THREAD_V(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,int argc,t_atom *argv) {  \
	thr_params *p = new thr_params; p->set_list(argc,argv); \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	AtomList *args = p->var[0]._list.args; \
	delete p; \
	if(ok) { \
		th->M_FUN(args->Count(),args->Atoms()); \
		th->PopThread(); \
	} \
	delete args; \
} 

/*! \brief Set up a threaded method callback for an arbitrary data struct
	\note Data is pure... no destructor is called upon delete
*/
#define FLEXT_THREAD_X(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,void *data) {  \
	thr_params *p = new thr_params; p->var[0]._ext.data = data; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	void *data = p->var[0]._ext.data; \
	delete p; \
	if(ok) { \
		th->M_FUN(data); \
		th->PopThread(); \
	} \
	delete data; \
} 

//! Set up a threaded method callback for a boolean argument
#define FLEXT_THREAD_B(M_FUN) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,int &arg1) {  \
	thr_params *p = new thr_params; p->var[0]._bool = arg1 != 0; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	bool b = p->var[0]._bool; \
	delete p; \
	if(ok) { \
		th->M_FUN(b); \
		th->PopThread(); \
	} \
} 

//! Set up a threaded method callback for 1 argument
#define FLEXT_THREAD_1(M_FUN,TP1) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1) {  \
	thr_params *p = new thr_params(1); \
	p->var[0]._ ## TP1 = arg1; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1); \
		th->PopThread(); \
	} \
} 

//! Set up a threaded method callback for 2 arguments
#define FLEXT_THREAD_2(M_FUN,TP1,TP2) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2) {  \
	thr_params *p = new thr_params(2); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
	thisType *th = FLEXT_CAST<thisType *>(p->cl); \
	bool ok = th->PushThread(); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP1 v2 = p->var[1]._ ## TP2; \
	delete p; \
	if(ok) { \
		th->M_FUN(v1,v2); \
		th->PopThread(); \
	} \
} 

//! Set up a threaded method callback for 3 arguments
#define FLEXT_THREAD_3(M_FUN,TP1,TP2,TP3) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3) {  \
	thr_params *p = new thr_params(3); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
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
} 

//! Set up a threaded method callback for 4 arguments
#define FLEXT_THREAD_4(M_FUN,TP1,TP2,TP3,TP4) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4) {  \
	thr_params *p = new thr_params(4); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	p->var[3]._ ## TP4 = arg4; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
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
} 

//! Set up a threaded method callback for 5 arguments
#define FLEXT_THREAD_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static bool FLEXT_CALL_PRE(M_FUN)(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4,TP5 &arg5) {  \
	thr_params *p = new thr_params(5); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	p->var[3]._ ## TP4 = arg4; \
	p->var[4]._ ## TP5 = arg5; \
	return c->StartThread(FLEXT_THR_PRE(M_FUN),p,#M_FUN); \
} \
static void FLEXT_THR_PRE(M_FUN)(thr_params *p) {  \
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

//! @} FLEXT_D_THREAD



// ====================================================================================

/*!	\defgroup FLEXT_D_CADDMETHOD Add flext methods within class scope
	\note These can only be used at class construction time
	@{ 
*/

//! Add a method handler for bang 
#define FLEXT_CADDBANG(CL,IX,M_FUN) \
\
AddMethod(CL,IX,"bang",FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with no arguments
#define FLEXT_CADDMETHOD(CL,IX,M_FUN) \
AddMethod(CL,IX,FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with a (variable argument) list
#define FLEXT_CADDMETHOD_V(CL,IX,M_FUN) \
\
AddMethod(CL,IX,FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with an anything argument
#define FLEXT_CADDMETHOD_A(CL,IX,M_FUN) \
\
AddMethod(CL,IX,FLEXT_CALL_PRE(M_FUN))	

//! Add a a handler for a method with implicit arguments
#define FLEXT_CADDMETHOD_(CL,IX,M_TAG,M_FUN) \
\
AddMethod(CL,IX,M_TAG,FLEXT_CALL_PRE(M_FUN))	

//! Add a handler for a method with 1 enum type argument
#define FLEXT_CADDMETHOD_E(CL,IX,M_TAG,M_FUN) \
\
AddMethod(ClMeths(CL),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),a_int,a_null)

//! Add a handler for a method with 1 argument
#define FLEXT_CADDMETHOD_1(CL,IX,M_TAG,M_FUN,TP1) \
\
AddMethod(ClMeths(CL),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),a_null)	

//! Add a handler for a method with 2 arguments
#define FLEXT_CADDMETHOD_2(CL,IX,M_TAG,M_FUN,TP1,TP2) \
\
AddMethod(ClMeths(CL),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),a_null)

//! Add a handler for a method with 3 arguments
#define FLEXT_CADDMETHOD_3(CL,IX,M_TAG,M_FUN,TP1,TP2,TP3) \
\
AddMethod(ClMeths(CL),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),a_null)

//! Add a handler for a method with 4 arguments
#define FLEXT_CADDMETHOD_4(CL,IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4) \
\
AddMethod(ClMeths(CL),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),a_null)

//! Add a handler for a method with 5 arguments
#define FLEXT_CADDMETHOD_5(CL,IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4,TP5) \
\
AddMethod(ClMeths(CL),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),FLEXTARG(TP5),a_null)


//	Shortcuts

//! Add a handler for a method with a boolean argument
#define FLEXT_CADDMETHOD_B(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_1(CL,IX,M_TAG,M_FUN,bool)

//! Add a handler for a method with 1 float argument
#define FLEXT_CADDMETHOD_F(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_1(CL,IX,M_TAG,M_FUN,float)

//! Add a handler for a method with 2 float arguments
#define FLEXT_CADDMETHOD_FF(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_2(CL,IX,M_TAG,M_FUN,float,float)

//! Add a handler for a method with 3 float arguments
#define FLEXT_CADDMETHOD_FFF(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_3(CL,IX,M_TAG,M_FUN,float,float,float)

//! Add a handler for a method with 1 integer argument
#define FLEXT_CADDMETHOD_I(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_1(CL,IX,M_TAG,M_FUN,int)

//! Add a handler for a method with 2 integer arguments
#define FLEXT_CADDMETHOD_II(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_2(CL,IX,M_TAG,M_FUN,int,int)

//! Add a handler for a method with 3 integer arguments
#define FLEXT_CADDMETHOD_III(CL,IX,M_TAG,M_FUN) \
\
FLEXT_CADDMETHOD_3(CL,IX,M_TAG,M_FUN,int,int,int)

//! @} FLEXT_D_CADDMETHOD


/*!	\defgroup FLEXT_D_ADDMETHOD Add flext methods
	\note These can only be used at object construction time 
	\note (in constructor or in Init() function before call to parent's Init())
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
AddMethod(ThMeths(),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),a_int,a_null)

//! Add a handler for a method with 1 argument
#define FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,TP1) \
\
AddMethod(ThMeths(),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),a_null)	

//! Add a handler for a method with 2 arguments
#define FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,TP1,TP2) \
\
AddMethod(ThMeths(),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),a_null)

//! Add a handler for a method with 3 arguments
#define FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,TP1,TP2,TP3) \
\
AddMethod(ThMeths(),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),a_null)

//! Add a handler for a method with 4 arguments
#define FLEXT_ADDMETHOD_4(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4) \
\
AddMethod(ThMeths(),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),a_null)

//! Add a handler for a method with 5 arguments
#define FLEXT_ADDMETHOD_5(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4,TP5) \
\
AddMethod(ThMeths(),IX,M_TAG,(methfun)(FLEXT_CALL_PRE(M_FUN)),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),FLEXTARG(TP5),a_null)


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


//! @} FLEXT_D_ADDMETHOD


/*!	\defgroup FLEXT_D_CALLMETHOD Call flext methods manually
	@{ 
*/

//! Call a (already defined) method with no arguments
#define FLEXT_CALLMETHOD(M_FUN) \
\
FLEXT_CALL_PRE(M_FUN)(this)

//! Call a (already defined) method with variable list arguments
#define FLEXT_CALLMETHOD_V(M_FUN,ARGC,ARGV) \
\
FLEXT_CALL_PRE(M_FUN)(this,ARGC,(t_atom *)(ARGV))

//! Call a (already defined) method with anything arguments
#define FLEXT_CALLMETHOD_A(M_FUN,HDR,ARGC,ARGV) \
\
FLEXT_CALL_PRE(M_FUN)(this,(t_symbol *)(HDR),ARGC,(t_atom *)(ARGV))

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

//! @} FLEXT_D_CALLMETHOD


//! @} FLEXT_D_METHOD


#ifdef FLEXT_ATTRIBUTES 

/*!	\defgroup FLEXT_D_ATTRIB Attribute definition
	\note These have to reside inside the class declaration
	@{ 
*/

/*! \brief Declare a attribute set function
	\internal
*/
#define FLEXT_CALLSET_(FUN,TP) \
static bool FLEXT_SET_PRE(FUN)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(arg); return true; }

/*! \brief Declare a attribute get function
	\internal
*/
#define FLEXT_CALLGET_(FUN,TP) \
static bool FLEXT_GET_PRE(FUN)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(arg); return true; }

/*! \brief Declare an implicite attribute set function
	\internal
*/
#define FLEXT_ATTRSET_(VAR,TP) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = arg; return true; }

/*! \brief Declare an implicite attribute get function
	\internal
*/
#define FLEXT_ATTRGET_(VAR,TP) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,TP &arg) \
{ arg = (TP)FLEXT_CAST<thisType *>(c)->VAR; return true; }


/*!	\defgroup FLEXT_DA_CALLSET Definition of attribute set handlers
	@{ 
*/

//! Declare a set function for a float attribute
#define FLEXT_CALLSET_F(SFUN) \
\
FLEXT_CALLSET_(SFUN,float)

//! Declare a set function for an integer attribute
#define FLEXT_CALLSET_I(SFUN) \
\
FLEXT_CALLSET_(SFUN,int)

//! Declare a set function for a boolean attribute
#define FLEXT_CALLSET_B(FUN) \
static bool FLEXT_SET_PRE(FUN)(flext_base *c,int &arg) \
{ bool b = arg != 0; FLEXT_CAST<thisType *>(c)->FUN(b); return true; }

//! Declare a set function for an enum attribute
#define FLEXT_CALLSET_E(SFUN,TP) \
\
FLEXT_CALLSET_(SFUN,TP)

//! Declare a set function for a symbol attribute
#define FLEXT_CALLSET_S(FUN) \
static bool FLEXT_SET_PRE(FUN)(flext_base *c,const t_symbol *&arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(arg); return true; }

//! Declare a set function for a variable list attribute
#define FLEXT_CALLSET_V(FUN) \
static bool FLEXT_SET_PRE(FUN)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(*arg); return true; }

//! @} FLEXT_DA_CALLSET

/*!	\defgroup FLEXT_DA_CALLGET Definition of attribute get handlers
	@{ 
*/

//! Declare a get function for a float attribute
#define FLEXT_CALLGET_F(GFUN) \
\
FLEXT_CALLGET_(GFUN,float)

//! Declare a get function for an integer attribute
#define FLEXT_CALLGET_I(GFUN) \
\
FLEXT_CALLGET_(GFUN,int)

//! Declare a get function for a boolean attribute
#define FLEXT_CALLGET_B(FUN) \
static bool FLEXT_GET_PRE(FUN)(flext_base *c,int &arg) \
{ bool b; FLEXT_CAST<thisType *>(c)->FUN(b); arg = b?1:0; return true; }

//! Declare a get function for an enum attribute
#define FLEXT_CALLGET_E(GFUN,TP) \
\
FLEXT_CALLGET_(GFUN,TP)

//! Declare a get function for a symbol attribute
#define FLEXT_CALLGET_S(FUN) \
static bool FLEXT_GET_PRE(FUN)(flext_base *c,const t_symbol *&arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(arg); return true; }

//! Declare a get function for a variable list attribute
#define FLEXT_CALLGET_V(FUN) \
static bool FLEXT_GET_PRE(FUN)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->FUN(*arg); return true; }

//! @} FLEXT_DA_CALLGET


/*!	\defgroup FLEXT_DA_CALLVAR Definition of attribute transfer handlers (both get and set)
	@{ 
*/

//! Declare both get and set functions for a float attribute
#define FLEXT_CALLVAR_F(GFUN,SFUN) \
\
FLEXT_CALLGET_F(GFUN) FLEXT_CALLSET_F(SFUN) 

//! Declare both get and set functions for an integer attribute
#define FLEXT_CALLVAR_I(GFUN,SFUN) \
\
FLEXT_CALLGET_I(GFUN) FLEXT_CALLSET_I(SFUN) 

//! Declare both get and set functions for a symbol attribute
#define FLEXT_CALLVAR_S(GFUN,SFUN) \
\
FLEXT_CALLGET_S(GFUN) FLEXT_CALLSET_S(SFUN) 

//! Declare both get and set functions for a boolean attribute
#define FLEXT_CALLVAR_B(GFUN,SFUN) \
\
FLEXT_CALLGET_B(GFUN) FLEXT_CALLSET_B(SFUN) 

//! Declare both get and set functions for an enum attribute
#define FLEXT_CALLVAR_E(GFUN,SFUN,TP) \
\
FLEXT_CALLGET_E(GFUN,TP) FLEXT_CALLSET_E(SFUN,TP) 

//! Declare both get and set functions for a variable list attribute
#define FLEXT_CALLVAR_V(GFUN,SFUN) \
\
FLEXT_CALLGET_V(GFUN) FLEXT_CALLSET_V(SFUN) 

//! @} FLEXT_DA_CALLVAR


/*!	\defgroup FLEXT_DA_ATTRSET Definition of implicite attribute set handlers
	@{ 
*/

//! Declare an implicite set function for a float attribute
#define FLEXT_ATTRSET_F(VAR) \
\
FLEXT_ATTRSET_(VAR,float)

//! Declare an implicite set function for an integer attribute
#define FLEXT_ATTRSET_I(VAR) \
\
FLEXT_ATTRSET_(VAR,int)

//! Declare an implicite set function for a symbol attribute
#define FLEXT_ATTRSET_S(VAR) \
\
FLEXT_ATTRSET_(VAR,t_symptr)

//! Declare an implicite set function for a boolean attribute
#define FLEXT_ATTRSET_B(VAR) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,int &arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = arg != 0; return true; }

//! Declare an implicite set function for an enum attribute
#define FLEXT_ATTRSET_E(VAR,TP) \
\
FLEXT_ATTRSET_(VAR,TP)

//! Declare an implicite set function for a variable list attribute
#define FLEXT_ATTRSET_V(VAR) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = *arg; return true; }

//! @} FLEXT_DA_ATTRSET

/*!	\defgroup FLEXT_DA_ATTRGET Definition of implicite attribute get handlers
	@{ 
*/

//! Declare an implicite get function for a float attribute
#define FLEXT_ATTRGET_F(VAR) \
\
FLEXT_ATTRGET_(VAR,float)

//! Declare an implicite get function for an integer attribute
#define FLEXT_ATTRGET_I(VAR) \
\
FLEXT_ATTRGET_(VAR,int)

//! Declare an implicite get function for a symbol attribute
#define FLEXT_ATTRGET_S(VAR) \
\
FLEXT_ATTRGET_(VAR,t_symptr)

//! Declare an implicite get function for a boolean attribute
#define FLEXT_ATTRGET_B(VAR) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,int &arg) \
{ arg = FLEXT_CAST<thisType *>(c)->VAR?1:0; return true; }

//! Declare an implicite get function for an enum attribute
#define FLEXT_ATTRGET_E(VAR,TP) \
\
FLEXT_ATTRGET_(VAR,TP)

//! Declare an implicite get function for a variable list attribute
#define FLEXT_ATTRGET_V(VAR) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,AtomList *&arg) \
{ *arg = FLEXT_CAST<thisType *>(c)->VAR; return true; }

//! @} FLEXT_DA_ATTRGET


/*!	\defgroup FLEXT_DA_ATTRVAR Definition of implicite attribute transfer handlers (both get and set)
	@{ 
*/

//! Declare both implicite get and set functions for a float attribute
#define FLEXT_ATTRVAR_F(VAR) \
\
FLEXT_ATTRGET_F(VAR) FLEXT_ATTRSET_F(VAR) 

//! Declare both implicite get and set functions for an integer attribute
#define FLEXT_ATTRVAR_I(VAR) \
\
FLEXT_ATTRGET_I(VAR) FLEXT_ATTRSET_I(VAR) 

//! Declare both implicite get and set functions for a symbol attribute
#define FLEXT_ATTRVAR_S(VAR) \
\
FLEXT_ATTRGET_S(VAR) FLEXT_ATTRSET_S(VAR) 

//! Declare both implicite get and set functions for a boolean attribute
#define FLEXT_ATTRVAR_B(VAR) \
\
FLEXT_ATTRGET_B(VAR) FLEXT_ATTRSET_B(VAR) 

//! Declare both implicite get and set functions for an enum attribute
#define FLEXT_ATTRVAR_E(VAR,TP) \
\
FLEXT_ATTRGET_(VAR,TP) FLEXT_ATTRSET_(VAR,TP) 

//! Declare both implicite get and set functions for a variable list attribute
#define FLEXT_ATTRVAR_V(VAR) \
\
FLEXT_ATTRGET_V(VAR) FLEXT_ATTRSET_V(VAR) 


//! @} FLEXT_DA_ATTRVAR


/*!	\defgroup FLEXT_D_CADDATTR Announce object attributes at class scope
	\note These can only be used at class construction time
	@{ 
*/

//! Add handler for a gettable attribute
#define	FLEXT_CADDATTR_GET(CL,NAME,GFUN) \
\
AddAttrib(CL,NAME,(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable attribute
#define	FLEXT_CADDATTR_SET(CL,NAME,SFUN) \
\
AddAttrib(CL,NAME,NULL,(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define	FLEXT_CADDATTR_VAR(CL,NAME,GFUN,SFUN) \
\
AddAttrib(CL,NAME,(FLEXT_GET_PRE(GFUN)),(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define	FLEXT_CADDATTR_VAR1(CL,NAME,FUN) \
\
AddAttrib(CL,NAME,(FLEXT_GET_PRE(FUN)),(FLEXT_SET_PRE(FUN)))


//! Add handler for a gettable enum attribute
#define	FLEXT_CADDATTR_GET_E(CL,NAME,GFUN) \
\
AddAttrib(CL,NAME,(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable enum attribute
#define	FLEXT_CADDATTR_SET_E(CL,NAME,SFUN) \
\
AddAttrib(CL,NAME,NULL,(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define	FLEXT_CADDATTR_VAR_E(CL,NAME,GFUN,SFUN) \
\
AddAttrib(CL,NAME,(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define	FLEXT_CADDATTR_VAR1_E(CL,NAME,FUN) \
\
AddAttrib(CL,NAME,(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(FUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(FUN)))

//! @} FLEXT_D_CADDATTR


/*!	\defgroup FLEXT_D_ADDATTR Announce object attributes 
	\note These can only be used at object construction time
	\note (in constructor or in Init() function before call to parent's Init())
	@{ 
*/

//! Add handler for a gettable attribute
#define	FLEXT_ADDATTR_GET(NAME,GFUN) \
\
AddAttrib(NAME,(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable attribute
#define	FLEXT_ADDATTR_SET(NAME,SFUN) \
\
AddAttrib(NAME,NULL,(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define	FLEXT_ADDATTR_VAR(NAME,GFUN,SFUN) \
\
AddAttrib(NAME,(FLEXT_GET_PRE(GFUN)),(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define	FLEXT_ADDATTR_VAR1(NAME,FUN) \
\
AddAttrib(NAME,(FLEXT_GET_PRE(FUN)),(FLEXT_SET_PRE(FUN)))


//! Add handler for a gettable enum attribute
#define	FLEXT_ADDATTR_GET_E(NAME,GFUN) \
\
AddAttrib(NAME,(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable enum attribute
#define	FLEXT_ADDATTR_SET_E(NAME,SFUN) \
\
AddAttrib(NAME,NULL,(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define	FLEXT_ADDATTR_VAR_E(NAME,GFUN,SFUN) \
\
AddAttrib(NAME,(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define	FLEXT_ADDATTR_VAR1_E(NAME,FUN) \
\
AddAttrib(NAME,(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(FUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(FUN)))

//! @} FLEXT_D_ADDATTR


//! @} FLEXT_D_ATTRIB

#endif // FLEXT_ATTRIBUTES


//!	@}  FLEXT_DEFS

#endif // __FLEXT_DEFS_H
