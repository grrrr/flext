/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all borrowed from GEM by Mark Danks


#ifndef __CPP_H
#define __CPP_H

#include <max-pd.h>

#ifdef __GNUC__
#include <typeinfo>
#else
#include <typeinfo.h>
#endif

class CPPExtern;

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    Obj_header
    
    The obligatory object header

DESCRIPTION
    
    This is in a separate struct to assure that when PD uses the
    class, the t_object is the very first thing.  If it were the 
    first thing in CPPExtern, then there could be problems with
    the vtable.
     
-----------------------------------------------------------------*/
struct EXT_EXTERN Obj_header
{
    	//////////
    	// The obligatory object header
    	t_sigobj    	    pd_obj;  // default signal
#ifdef PD
		F defsig;
#endif

    	//////////
    	// Our data structure
        CPPExtern           *data;
};

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    CPPExtern
    
    The base class for all externs written in C++

DESCRIPTION
    
    Each extern which is written in C++ needs to use the #defines at the
    end of this header file.  Currently, the operator new(size_t) and
    operator delete(void *) are not overridden.  This will be a problem
    when PD expects everything to fit in its memory space and control
    all memory allocation.
    
    The define
    
        CPPEXTERN_HEADER(NEW_CLASS, PARENT_CLASS)
    
    should be somewhere in your header file.
    One of the defines like
    
    CPPEXTERN_NEW(NEW_CLASS)
    CPPEXTERN_NEW_WITH_TWO_ARGS(NEW_CLASS, t_floatarg, A_FLOAT, t_floatarg, A_FLOAT)
    
    should be the first thing in your implementation file.
    NEW_CLASS is the name of your class and PARENT_CLASS is the 
    parent of your class.
        
-----------------------------------------------------------------*/
class EXT_EXTERN CPPExtern
{
    public:

        //////////
        // Constructor
    	CPPExtern();

        //////////
        // The object header
        t_sigobj          *x_obj;        	

    	//////////
    	// Destructor
    	virtual ~CPPExtern() = 0;
    	
        //////////
        // Get the object's canvas
        t_canvas            *getCanvas()        { return(m_canvas); }

        //////////
        // This is a holder - don't touch it
        static t_sigobj     *m_holder;

    protected:
    	
    	//////////
    	// Creation callback
    	static void callb_setup(t_class *) {}	

		// to be called in object _setup function
		static void enable_signal(t_class *classPtr,float def = 0.) 
		{
#ifdef PD
			((Obj_header *)classPtr)->defsig = def;
#elif defined(MAXMSP)
			dsp_initclass();
#endif
		}

    private:

        //////////
        // The canvas (patcher) that the object is in
        t_canvas            *m_canvas;
};

// This has a dummy arg so that NT won't complain
EXT_EXTERN void *operator new(size_t, void *location, void *dummy);

////////////////////////////////////////
// This should be used in the header
////////////////////////////////////////

#ifdef PD
#define CPPEXTERN_GETCLASS(obj) ((t_class *)((t_object *)(obj))->te_g.g_pd)
#elif defined(MAXMSP)
#define CPPEXTERN_GETCLASS(obj) ((t_class *)(((t_tinyobject *)obj)->t_messlist-1))
#endif

#define CPPEXTERN_HEADER(NEW_CLASS, PARENT_CLASS)    	    	\
public:     	    	    \
const char *thisName() const { return typeid(*this).name(); /*#return NEW_CLASS;*/ } 	    	\
static void callb_free(void *data)    	    	    	\
{ CPPExtern *mydata = ((Obj_header *)data)->data; delete mydata; \
  ((Obj_header *)data)->Obj_header::~Obj_header(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr);    	    	\
  NEW_CLASS::cb_setup(classPtr); }  	    	    	\
private:    \
inline t_class *thisClass() { return CPPEXTERN_GETCLASS(x_obj); } \
static NEW_CLASS *thisObject(V *c) { return (NEW_CLASS *)((Obj_header *)c)->data; }	    	   \
static void cb_setup(t_class *classPtr);


////////////////////////////////////////
// This should be the first thing in the implementation file
////////////////////////////////////////

//
// NO ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW(NAME,NEW_CLASS)    	    	    	    	\
    REAL_NEW(NAME,NEW_CLASS, _setup, _class)

//
// ONE ARGUMENT
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_ONE_ARG(NAME,NEW_CLASS, TYPE, PD_TYPE)    \
    REAL_NEW_WITH_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE)

//
// GIMME ARGUMENT
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_GIMME(NAME,NEW_CLASS)  	    	    	\
    REAL_NEW_WITH_GIMME(NAME,NEW_CLASS, _setup, _class)

//
// TWO ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_TWO_ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO)	\
    REAL_NEW_WITH_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO)

//
// THREE ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_THREE_ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)	\
    REAL_NEW_WITH_ARG_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)

//
// FOUR ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_FOUR_ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR) \
    REAL_NEW_WITH_ARG_ARG_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR)

////////////////////////////////////////
// These definitions are used below
////////////////////////////////////////

#ifdef PD
#define CPPEXTERN_NEWFN ::class_new
#define CPPEXTERN_CLREF(NAME,CLASS) gensym(NAME)
#elif defined(MAXMSP)
#define CPPEXTERN_NEWFN NULL; ::setup
#define CPPEXTERN_CLREF(NAME,CLASS) (t_messlist **)&(CLASS)
#endif

///////////////////////////////////////////////////////////////////////////////
// These should never be called or used directly!!!
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// no args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME)        \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS ()                              \
{     	    	    	    	    	    	    	    	\
    Obj_header *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) Obj_header; \
    CPPExtern::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS;                                  \
    CPPExtern::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = CPPEXTERN_NEWFN(                       \
    	     	CPPEXTERN_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	A_NULL);                                        \
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// one arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, VAR_TYPE, PD_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (VAR_TYPE arg)                  \
{     	    	    	    	    	    	    	    	\
    Obj_header *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) Obj_header; \
    CPPExtern::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg);                             \
    CPPExtern::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = CPPEXTERN_NEWFN(                       \
    	     	CPPEXTERN_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	PD_TYPE,                                        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// gimme arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_GIMME(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (t_symbol *, int argc, t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    Obj_header *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) Obj_header; \
    CPPExtern::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(argc, argv);                      \
    CPPExtern::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = CPPEXTERN_NEWFN(                       \
    	     	CPPEXTERN_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	A_GIMME,                                        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// two args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG_ARG(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_VAR_TYPE, ONE_PD_TYPE, TWO_VAR_TYPE, TWO_PD_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (ONE_VAR_TYPE arg, TWO_VAR_TYPE argtwo) \
{     	    	    	    	    	    	    	    	\
    Obj_header *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) Obj_header; \
    CPPExtern::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg, argtwo);                     \
    CPPExtern::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = CPPEXTERN_NEWFN(                       \
    	     	CPPEXTERN_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE,                       \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// three args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG_ARG_ARG(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_VAR_TYPE, ONE_PD_TYPE, TWO_VAR_TYPE, TWO_PD_TYPE, THREE_VAR_TYPE, THREE_PD_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (ONE_VAR_TYPE arg, TWO_VAR_TYPE argtwo, THREE_VAR_TYPE argthree) \
{     	    	    	    	    	    	    	    	\
    Obj_header *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) Obj_header; \
    CPPExtern::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg, argtwo, argthree);           \
    CPPExtern::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = CPPEXTERN_NEWFN(                       \
    	     	CPPEXTERN_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE, THREE_PD_TYPE,        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// four args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG_ARG_ARG_ARG(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_VAR_TYPE, ONE_PD_TYPE, TWO_VAR_TYPE, TWO_PD_TYPE, THREE_VAR_TYPE, THREE_PD_TYPE, FOUR_VAR_TYPE, FOUR_PD_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (ONE_VAR_TYPE arg, TWO_VAR_TYPE argtwo, THREE_VAR_TYPE argthree, FOUR_VAR_TYPE argfour) \
{     	    	    	    	    	    	    	    	\
    Obj_header *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) Obj_header; \
    CPPExtern::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg, argtwo, argthree, argfour);  \
    CPPExtern::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
void NEW_CLASS ## SETUP_FUNCTION()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = CPPEXTERN_NEWFN(                       \
    	     	CPPEXTERN_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE, THREE_PD_TYPE, FOUR_PD_TYPE, \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

#endif




