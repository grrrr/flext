/* 

flext - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all borrowed from GEM by Mark Danks


#ifndef _FLBASE_H
#define _FLBASE_H

#include <flstdc.h>

#ifdef __GNUC__
#include <typeinfo>
#else
#include <typeinfo.h>
#endif

class flext_obj;

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    flext_hdr
    
    The obligatory object header

DESCRIPTION
    
    This is in a separate struct to assure that when PD uses the
    class, the t_object is the very first thing.  If it were the 
    first thing in flext_obj, then there could be problems with
    the vtable.
     
-----------------------------------------------------------------*/
struct FLEXT_EXT flext_hdr
{
    	//////////
    	// The obligatory object header
    	t_sigobj    	    pd_obj;  // default signal
#ifdef PD
		F defsig;
#endif

    	//////////
    	// Our data structure
        flext_obj           *data;
};

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    flext_obj
    
    The base class for all externs written in C++

DESCRIPTION
    
    Each extern which is written in C++ needs to use the #defines at the
    end of this header file.  Currently, the operator new(size_t) and
    operator delete(void *) are not overridden.  This will be a problem
    when PD expects everything to fit in its memory space and control
    all memory allocation.
    
    The define
    
        FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)
    
    should be somewhere in your header file.
    One of the defines like
    
    FLEXT_NEW(NEW_CLASS)
    FLEXT_NEW_WITH_TWO_ARGS(NEW_CLASS, t_floatarg, A_FLOAT, t_floatarg, A_FLOAT)
    
    should be the first thing in your implementation file.
    NEW_CLASS is the name of your class and PARENT_CLASS is the 
    parent of your class.
        
-----------------------------------------------------------------*/
class FLEXT_EXT flext_obj
{
    public:

        //////////
        // Constructor
    	flext_obj();

    	//////////
    	// Destructor
    	virtual ~flext_obj() = 0;
    	
        //////////
        // Get the object's canvas
        t_canvas            *getCanvas()        { return(m_canvas); }

    protected:
    	
        //////////
        // The object header
        t_sigobj          *x_obj;        	

    	//////////
    	// Creation callback
    	static void callb_setup(t_class *) {}	

		// to be called in object _setup function
		static void enable_signal(t_class *classPtr,float def = 0.) 
		{
#ifdef PD
			((flext_hdr *)classPtr)->defsig = def;
#elif defined(MAXMSP)
			dsp_initclass();
#endif
		}

    private:

        //////////
        // The canvas (patcher) that the object is in
        t_canvas            *m_canvas;

	public:
        //////////
        // This is a holder - don't touch it
        static t_sigobj     *m_holder;

        //////////
        // The object's name in the patcher
		const char *m_name;
};

// This has a dummy arg so that NT won't complain
FLEXT_EXT void *operator new(size_t, void *location, void *dummy);

////////////////////////////////////////
// This should be used in the header
////////////////////////////////////////

#ifdef PD
#define FLEXT_GETCLASS(obj) ((t_class *)((t_object *)(obj))->te_g.g_pd)
#elif defined(MAXMSP)
#define FLEXT_GETCLASS(obj) ((t_class *)(((t_tinyobject *)obj)->t_messlist-1))
#endif

#define FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)    	    	\
public:     	    	    \
static void callb_free(void *data)    	    	    	\
{ flext_obj *mydata = ((flext_hdr *)data)->data; delete mydata; \
  ((flext_hdr *)data)->flext_hdr::~flext_hdr(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr);    	    	\
  NEW_CLASS::cb_setup(classPtr); }  	    	    	\
private:    \
inline t_class *thisClass() { return FLEXT_GETCLASS(x_obj); } \
inline const char *thisName() const { return m_name; } \
static NEW_CLASS *thisObject(V *c) { return (NEW_CLASS *)((flext_hdr *)c)->data; }	    	  \
static void cb_setup(t_class *classPtr);


////////////////////////////////////////
// This should be the first thing in the implementation file
////////////////////////////////////////

// Shortcuts for PD/Max type arguments
#define FLEXTTYPE_F A_FLOAT
#define FLEXTTYPE_float A_FLOAT
#define FLEXTTYPE_t_symbol A_SYMBOL
#define FLEXTTYPE_t_pointer A_POINTER

//
// NO ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW(NAME,NEW_CLASS)    	    	    	    	\
    REAL_NEW(NAME,NEW_CLASS, _setup, _class)

#define FLEXT_TILDE_NEW(NAME,NEW_CLASS)    	    	    	    	\
    REAL_NEW(NAME,NEW_CLASS, _tilde_setup, _class)

//
// ONE ARGUMENT
/////////////////////////////////////////////////
#define FLEXT_NEW_1ARG(NAME,NEW_CLASS, TYPE, PD_TYPE)    \
    REAL_NEW_WITH_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE)

#define FLEXT_TILDE_NEW_1ARG(NAME,NEW_CLASS, TYPE, PD_TYPE)    	    	    	    	\
    REAL_NEW_WITH_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, PD_TYPE)

#define FLEXT_1ARG(NAME,NEW_CLASS, TYPE)    \
    REAL_NEW_WITH_ARG(NAME,NEW_CLASS, _setup, _class, TYPE,FLEXTTYPE_ ## TYPE)

#define FLEXT_TILDE_1ARG(NAME,NEW_CLASS, TYPE)    	    	    	    	\
    REAL_NEW_WITH_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, FLEXTTYPE_ ## TYPE)

//
// GIMME ARGUMENT
/////////////////////////////////////////////////
#define FLEXT_NEW_GIMME(NAME,NEW_CLASS)  	    	    	\
    REAL_NEW_WITH_GIMME(NAME,NEW_CLASS, _setup, _class)

#define FLEXT_TILDE_NEW_GIMME(NAME,NEW_CLASS)  	    	    	\
    REAL_NEW_WITH_GIMME(NAME,NEW_CLASS, _tilde_setup, _class)

#define FLEXT_GIMME(NAME,NEW_CLASS)  	    	    	\
    REAL_NEW_WITH_GIMME(NAME,NEW_CLASS, _setup, _class)

#define FLEXT_TILDE_GIMME(NAME,NEW_CLASS)  	    	    	\
    REAL_NEW_WITH_GIMME(NAME,NEW_CLASS, _tilde_setup, _class)

//
// TWO ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_2ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO)	\
    REAL_NEW_WITH_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO)

#define FLEXT_TILDE_NEW_2ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO)	\
    REAL_NEW_WITH_ARG_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO)

#define FLEXT_2ARGS(NAME,NEW_CLASS, TYPE, TTWO)	\
    REAL_NEW_WITH_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, FLEXTTYPE_ ## TYPE, TTWO, FLEXTTYPE_ ## TTWO)

#define FLEXT_TILDE_2ARGS(NAME,NEW_CLASS, TYPE, TTWO)	\
    REAL_NEW_WITH_ARG_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, FLEXTTYPE_ ## TYPE, TTWO, FLEXTTYPE_ ## TTWO)

//
// THREE ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_3ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)	\
    REAL_NEW_WITH_ARG_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)

#define FLEXT_TILDE_NEW_3ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)	\
    REAL_NEW_WITH_ARG_ARG_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)

#define FLEXT_3ARGS(NAME,NEW_CLASS, TYPE, TTWO, TTHREE)	\
    REAL_NEW_WITH_ARG_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, FLEXTTYPE_ ## TYPE, TTWO, FLEXTTYPE_ ## TTWO, TTHREE, FLEXTTYPE_ ## TTHREE)

#define FLEXT_TILDE_3ARGS(NAME,NEW_CLASS, TYPE, TTWO, TTHREE)	\
    REAL_NEW_WITH_ARG_ARG_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, FLEXTTYPE_ ## TYPE, TTWO, FLEXTTYPE_ ## TTWO, TTHREE, FLEXTTYPE_ ## TTHREE)

//
// FOUR ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_4ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR) \
    REAL_NEW_WITH_ARG_ARG_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR)

#define FLEXT_TILDE_NEW_4ARGS(NAME,NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR) \
    REAL_NEW_WITH_ARG_ARG_ARG_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR)

#define FLEXT_4ARGS(NAME,NEW_CLASS, TYPE, TTWO, TTHREE, TFOUR) \
    REAL_NEW_WITH_ARG_ARG_ARG_ARG(NAME,NEW_CLASS, _setup, _class, TYPE, FLEXTTYPE_ ## TYPE, TTWO, FLEXTTYPE_ ## TTWO, TTHREE, FLEXTTYPE_ ## TTHREE, TFOUR, FLEXTTYPE_ ## TFOUR)

#define FLEXT_TILDE_4ARGS(NAME,NEW_CLASS, TYPE, TTWO, TTHREE, TFOUR) \
    REAL_NEW_WITH_ARG_ARG_ARG_ARG(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, FLEXTTYPE_ ## TYPE, TTWO, FLEXTTYPE_ ## TTWO, TTHREE, FLEXTTYPE_ ## TTHREE, TFOUR, FLEXTTYPE_ ## TFOUR)

////////////////////////////////////////
// These definitions are used below
////////////////////////////////////////

#ifdef PD
#define FLEXT_NEWFN ::class_new
#define FLEXT_CLREF(NAME,CLASS) gensym(NAME)
#define FLEXT_MAIN(MAINNAME) MAINNAME
#elif defined(MAXMSP)
#define FLEXT_NEWFN NULL; ::setup
#define FLEXT_CLREF(NAME,CLASS) (t_messlist **)&(CLASS)
#define FLEXT_MAIN(MAINNAME) main
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
{     	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS;                                  \
    flext_obj::m_holder = NULL;                                 \
	obj->data->m_name = NAME;									\
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NEW_CLASS::m_name,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), 0,                          \
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
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg);                             \
    flext_obj::m_holder = NULL;                                 \
	obj->data->m_name = NAME;									\
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), 0,                          \
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
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(argc, argv);                      \
    flext_obj::m_holder = NULL;                                 \
	obj->data->m_name = NAME;									\
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), 0,                          \
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
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg, argtwo);                     \
    flext_obj::m_holder = NULL;                                 \
	obj->data->m_name = NAME;									\
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), 0,                          \
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
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg, argtwo, argthree);           \
    flext_obj::m_holder = NULL;                                 \
   	obj->data->m_name = NAME;									\
return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), 0,                          \
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
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = &obj->pd_obj;                         \
    obj->data = new NEW_CLASS(arg, argtwo, argthree, argfour);  \
    flext_obj::m_holder = NULL;                                 \
	obj->data->m_name = NAME;									\
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE, THREE_PD_TYPE, FOUR_PD_TYPE, \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

#endif




