/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fldefs.h
    \brief Definitions for actual usage
    
*/

#ifndef __FLEXT_DEFS_H
#define __FLEXT_DEFS_H

// ====================================================================================


/*! @name flext class header
	One of these definitions is compulsary for the class declaration
*/

//@{ 
// FLEXT_HEADERS

//! Header without setup callback
#define FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)    	    	\
	FLEXT_REALHDR(NEW_CLASS, PARENT_CLASS)    	    	

//! Header with setup callback
#define FLEXT_HEADER_S(NEW_CLASS, PARENT_CLASS, SETUPFUN)    	    	\
	FLEXT_REALHDR_S(NEW_CLASS, PARENT_CLASS, SETUPFUN)    	    	

//@} FLEXT_HEADERS



// ====================================================================================



/*! @name flext implementation
	One of these definitions is compulsary for the class declaration
*/

//@{ 
// FLEXT_NEWS

/*! These definitions implement flext based external classes.
*/

// NO ARGUMENTS
// ----------------------------------------

//! Implementation of a flext class with no arguments
#define FLEXT_NEW(NAME,NEW_CLASS)		\
REAL_NEW(NAME,NEW_CLASS, _setup)  \
REAL_EXT(NEW_CLASS, _setup)

//! Implementation of a flext tilde class with no arguments
#define FLEXT_NEW_TILDE(NAME,NEW_CLASS)	\
REAL_NEW(NAME,NEW_CLASS, _tilde_setup) \
REAL_EXT(NEW_CLASS, _tilde_setup)

//! Implementation of a flext class (part of a library) with no arguments
#define FLEXT_LIB(NAME,NEW_CLASS) \
REAL_LIB(NAME,NEW_CLASS, _setup) 

//! Implementation of a flext tilde class (part of a library) with no arguments
#define FLEXT_LIB_TILDE(NAME,NEW_CLASS)	\
REAL_LIB(NAME,NEW_CLASS, _tilde_setup) 


// ONE ARGUMENT
// ----------------------------------------

//! Implementation of a flext class with one argument
#define FLEXT_NEW_1(NAME,NEW_CLASS, TYPE)		\
REAL_NEW_1(NAME,NEW_CLASS, _setup, TYPE) \
REAL_EXT(NEW_CLASS, _setup)

//! Implementation of a flext tilde class with one argument
#define FLEXT_NEW_TILDE_1(NAME,NEW_CLASS, TYPE)	\
REAL_NEW_1(NAME,NEW_CLASS, _tilde_setup, TYPE) \
REAL_EXT(NEW_CLASS, _tilde_setup)

//! Implementation of a flext class (part of a library) with one argument
#define FLEXT_LIB_1(NAME,NEW_CLASS, TYPE)		\
REAL_LIB_1(NAME,NEW_CLASS, _setup,TYPE)

//! Implementation of a flext tilde class (part of a library) with one argument
#define FLEXT_LIB_TILDE_1(NAME,NEW_CLASS, TYPE)	\
REAL_LIB_1(NAME,NEW_CLASS, _tilde_setup, TYPE)


// GIMME ARGUMENT
// ----------------------------------------

//! Implementation of a flext class with a variable argument list
#define FLEXT_NEW_G(NAME,NEW_CLASS)			\
REAL_NEW_G(NAME,NEW_CLASS, _setup) \
REAL_EXT(NEW_CLASS, _setup)

//! Implementation of a flext tilde class with a variable argument list
#define FLEXT_NEW_TILDE_G(NAME,NEW_CLASS)	\
REAL_NEW_G(NAME,NEW_CLASS,_tilde_setup) \
REAL_EXT(NEW_CLASS, _tilde_setup)

//! Implementation of a flext class (part of a library) with a variable argument list
#define FLEXT_LIB_G(NAME,NEW_CLASS)			\
REAL_LIB_G(NAME,NEW_CLASS, _setup) 

//! Implementation of a flext tilde class (part of a library) with a variable argument list
#define FLEXT_LIB_TILDE_G(NAME,NEW_CLASS)	\
REAL_LIB_G(NAME,NEW_CLASS, _tilde_setup) 


// TWO ARGUMENTS
// ----------------------------------------

//! Implementation of a flext class with two arguments
#define FLEXT_NEW_2(NAME,NEW_CLASS, TYPE1, TYPE2)			\
REAL_NEW_2(NAME,NEW_CLASS, _setup, TYPE1, TYPE2) \
REAL_EXT(NEW_CLASS, _setup)

//! Implementation of a flext tilde class with one argument
#define FLEXT_NEW_TILDE_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
REAL_NEW_2(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2) \
REAL_EXT(NEW_CLASS, _tilde_setup)

//! Implementation of a flext class (part of a library) with two arguments
#define FLEXT_LIB_2(NAME,NEW_CLASS, TYPE1, TYPE2)		\
REAL_LIB_2(NAME,NEW_CLASS, _setup, TYPE1, TYPE2)

//! Implementation of a flext tilde class (part of a library) with two arguments
#define FLEXT_LIB_TILDE_2(NAME,NEW_CLASS, TYPE1, TYPE2)	\
REAL_LIB_2(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2)


// THREE ARGUMENTS
// ----------------------------------------

//! Implementation of a flext class with three arguments
#define FLEXT_NEW_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3) \
REAL_NEW_3(NAME,NEW_CLASS, _setup, TYPE1, TYPE2, TYPE3)  \
REAL_EXT(NEW_CLASS, _setup)

//! Implementation of a flext tilde class with three arguments
#define FLEXT_NEW_TILDE_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
REAL_NEW_3(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2, TYPE3) \
REAL_EXT(NEW_CLASS, _tilde_setup)

//! Implementation of a flext class (part of a library) with three arguments
#define FLEXT_LIB_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)		\
REAL_LIB_3(NAME,NEW_CLASS, _setup,TYPE1, TYPE2, TYPE3)

//! Implementation of a flext tilde class (part of a library) with three arguments
#define FLEXT_LIB_TILDE_3(NAME,NEW_CLASS, TYPE1, TYPE2, TYPE3)	\
REAL_LIB_3(NAME,NEW_CLASS, _tilde_setup, TYPE1, TYPE2, TYPE3)


// MaxMSP doesn't seem to be able to handle more than 3 creation arguments! -> USE GIMME

//@} FLEXT_NEWS


// ====================================================================================


/*! @name flext callbacks
	Description the callbacks
*/

//@{ FLEXT_CALLBACKS

//! with no arguments
#define FLEXT_CALLBACK(M_FUN) \
static void cb_ ## M_FUN(flext_base *c) { static_cast<thisType *>(c)->M_FUN(); }

//! for anything
#define FLEXT_CALLBACK_A(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,t_symbol *s,int argc,t_atom *argv) { static_cast<thisType *>(c)->M_FUN(s,argc,argv); }

//! for gimme
#define FLEXT_CALLBACK_G(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int argc,t_atom *argv) { static_cast<thisType *>(c)->M_FUN(argc,argv); }

//! for boolean argument
#define FLEXT_CALLBACK_B(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int &arg1) { static_cast<thisType *>(c)->M_FUN(arg1 != 0); }

//! 1 argument
#define FLEXT_CALLBACK_1(M_FUN,TP1) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1) { static_cast<thisType *>(c)->M_FUN(arg1); }

//! 2 arguments
#define FLEXT_CALLBACK_2(M_FUN,TP1,TP2) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2) { static_cast<thisType *>(c)->M_FUN(arg1,arg2); }

//! 3 arguments
#define FLEXT_CALLBACK_3(M_FUN,TP1,TP2,TP3) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3) { static_cast<thisType *>(c)->M_FUN(arg1,arg2,arg3); }

//! 4 arguments
#define FLEXT_CALLBACK_4(M_FUN,TP1,TP2,TP3,TP4) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4) { static_cast<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4); }

//! 5 arguments
#define FLEXT_CALLBACK_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4,TP5 &arg5) { static_cast<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4,arg5); }


// Shortcuts:

//! 1 float argument
#define FLEXT_CALLBACK_F(M_FUN) FLEXT_CALLBACK_1(M_FUN,float)
//! 2 float arguments
#define FLEXT_CALLBACK_FF(M_FUN) FLEXT_CALLBACK_2(M_FUN,float,float)
//! 3 float arguments
#define FLEXT_CALLBACK_FFF(M_FUN) FLEXT_CALLBACK_3(M_FUN,float,float,float)

//! 1 int argument
#define FLEXT_CALLBACK_I(M_FUN) FLEXT_CALLBACK_1(M_FUN,int)
//! 2 int arguments
#define FLEXT_CALLBACK_II(M_FUN) FLEXT_CALLBACK_2(M_FUN,int,int)
//! 3 int arguments
#define FLEXT_CALLBACK_III(M_FUN) FLEXT_CALLBACK_3(M_FUN,int,int,int)

//@} FLEXT_CALLBACKS



//@{ FLEXT_THREADS

#ifdef FLEXT_THREADS

//! with no arguments
#define FLEXT_THREAD(M_FUN) \
static void cb_ ## M_FUN(flext_base *c) {  \
	thr_params *p = new thr_params(c); \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(); \
	th->DecThreads(); \
	return NULL; \
} 

//! for anything
#define FLEXT_THREAD_A(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,t_symbol *s,int argc,t_atom *argv) {  \
	thr_params *p = new thr_params(c); p->set_any(s,argc,argv); \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	t_symbol *s; int argc; t_atom argv; p->get_any(s,argc,argv); \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(s,argc,argv); \
	th->DecThreads(); \
	return NULL; \
} 

//! for gimme
#define FLEXT_THREAD_G(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int argc,t_atom *argv) {  \
	thr_params *p = new thr_params(c); p->set_gimme(argc,argv); \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	int argc; t_atom argv; p->get_gimme(argc,argv); \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(argc,argv); \
	th->DecThreads(); \
	return NULL; \
} 

//! for boolean argument
#define FLEXT_THREAD_B(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int &arg1) {  \
	thr_params *p = new thr_params(c); p->var[0]._bool = arg1 != 0; \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	bool b = p->var[0]; \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(b); \
	th->DecThreads(); \
	return NULL; \
} 

//! 1 argument
#define FLEXT_THREAD_1(M_FUN,TP1) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1) {  \
	thr_params *p = new thr_params(c); \
	p->var[0]._ ## TP1 = arg1; \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(v1); \
	th->DecThreads(); \
	return NULL; \
} 

//! 2 arguments
#define FLEXT_THREAD_2(M_FUN,TP1,TP2) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2) {  \
	thr_params *p = new thr_params(c); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP1 v2 = p->var[1]._ ## TP2; \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(v1,v2); \
	th->DecThreads(); \
	return NULL; \
} 

//! 3 arguments
#define FLEXT_THREAD_3(M_FUN,TP1,TP2,TP3) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3) {  \
	thr_params *p = new thr_params(c); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP2 v2 = p->var[1]._ ## TP2; \
	const TP3 v3 = p->var[2]._ ## TP3; \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(v1,v2,v3); \
	th->DecThreads(); \
	return NULL; \
} 

//! 4 arguments
#define FLEXT_THREAD_4(M_FUN,TP1,TP2,TP3,TP4) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4) {  \
	thr_params *p = new thr_params(c); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	p->var[3]._ ## TP4 = arg4; \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP2 v2 = p->var[1]._ ## TP2; \
	const TP3 v3 = p->var[2]._ ## TP3; \
	const TP4 v4 = p->var[3]._ ## TP4; \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(v1,v2,v3,v4); \
	th->DecThreads(); \
	return NULL; \
} 

//! 5 arguments
#define FLEXT_THREAD_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4,TP5 &arg5) {  \
	thr_params *p = new thr_params(c); \
	p->var[0]._ ## TP1 = arg1; \
	p->var[1]._ ## TP2 = arg2; \
	p->var[2]._ ## TP3 = arg3; \
	p->var[3]._ ## TP4 = arg4; \
	p->var[4]._ ## TP5 = arg5; \
	StartThread(thr_ ## M_FUN,p,#M_FUN); \
} \
static void *thr_ ## M_FUN(thr_params *p) {  \
	thisType *th = static_cast<thisType *>(p->cl); \
	const TP1 v1 = p->var[0]._ ## TP1; \
	const TP2 v2 = p->var[1]._ ## TP2; \
	const TP3 v3 = p->var[2]._ ## TP3; \
	const TP4 v4 = p->var[3]._ ## TP4; \
	const TP5 v5 = p->var[4]._ ## TP5; \
	delete p; \
	th->IncThreads(); \
	th->M_FUN(v1,v2,v3,v4,v5); \
	th->DecThreads(); \
	return NULL; \
} 


// Shortcuts:

//! 1 float argument
#define FLEXT_THREAD_F(M_FUN) FLEXT_THREAD_1(M_FUN,float)
//! 2 float arguments
#define FLEXT_THREAD_FF(M_FUN) FLEXT_THREAD_2(M_FUN,float,float)
//! 3 float arguments
#define FLEXT_THREAD_FFF(M_FUN) FLEXT_THREAD_3(M_FUN,float,float,float)

//! 1 int argument
#define FLEXT_THREAD_I(M_FUN) FLEXT_THREAD_1(M_FUN,int)
//! 2 int arguments
#define FLEXT_THREAD_II(M_FUN) FLEXT_THREAD_2(M_FUN,int,int)
//! 3 int arguments
#define FLEXT_THREAD_III(M_FUN) FLEXT_THREAD_3(M_FUN,int,int,int)

#endif // FLEXT_THREADS

//@} FLEXT_CALLBACKS



// ====================================================================================



/*! @name add flext methods
	These should be the used in the class' constructor
*/

//@{ FLEXT_ADDMETHODS

//! enable list element distribution over inlets (if no better handler found)
#define FLEXT_ADDDIST() \
SetDist(true)	

//! add handler for bang 
#define FLEXT_ADDBANG(IX,M_FUN) \
AddMethod(IX,"bang",cb_ ## M_FUN)	

//! add handler for method with no args
#define FLEXT_ADDMETHOD(IX,M_FUN) \
AddMethod(IX,cb_ ## M_FUN)	

//! add handler for method with implicit args
#define FLEXT_ADDMETHOD_(IX,M_TAG,M_FUN) \
AddMethod(IX,M_TAG,cb_ ## M_FUN)	

//! add handler for method with 1 enum type arg
#define FLEXT_ADDMETHOD_E(IX,M_TAG,M_FUN) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),a_int,a_null)

//! add handler for method with 1 arg
#define FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,TP1) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),a_null)	

//! add handler for method with 2 args
#define FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,TP1,TP2) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),a_null)

//! add handler for method with 3 args
#define FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,TP1,TP2,TP3) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),a_null)

//! add handler for method with 4 args
#define FLEXT_ADDMETHOD_4(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),a_null)

//! add handler for method with 5 args
#define FLEXT_ADDMETHOD_5(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4,TP5) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),FLEXTARG(TP5),a_null)

// a few shortcuts:

//! boolean argument
#define FLEXT_ADDMETHOD_B(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,bool)
//! 1 float argument
#define FLEXT_ADDMETHOD_F(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,float)
//! 2 float arguments
#define FLEXT_ADDMETHOD_FF(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,float,float)
//! 3 float arguments
#define FLEXT_ADDMETHOD_FFF(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,float,float,float)
//! 1 int argument
#define FLEXT_ADDMETHOD_I(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,int)
//! 2 int arguments
#define FLEXT_ADDMETHOD_II(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,int,int)
//! 3 int arguments
#define FLEXT_ADDMETHOD_III(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,int,int,int)

//@} FLEXT_ADDMETHODS


#endif // __FLEXT_DEFS_H
