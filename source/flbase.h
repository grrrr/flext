#ifndef __CPP_H
#define __CPP_H

#include <max-pd.h>

#ifdef PD
#pragma warning (push)
#pragma warning (disable:4091)
extern "C" {	    	    	    	    	    	    	\

#include <m_imp.h>

}
#pragma warning (pop)
#endif


// This is all borrowed from GEM by Mark Danks


#ifdef _WINDOWS
#define EXT_EXTERN __declspec(dllexport)
#else                   // other OS's
#define EXT_EXTERN
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
#ifdef PD
    	t_object    	    pd_obj;  // default signal
		F defsig;
#elif defined(MAX)
    	t_pxobject    	    pd_obj;  // proxy object for signal handling
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
        t_object          *x_obj;

    	//////////
    	// Destructor
    	virtual ~CPPExtern() = 0;
    	
#ifdef PD
        //////////
        // Get the object's canvas
        t_canvas            *getCanvas()        { return(m_canvas); }
#endif

        //////////
        // This is a holder - don't touch it
        static t_object     *m_holder;

    protected:
    	
    	//////////
    	// Creation callback
    	static void callb_setup(t_class *) {}	

		// to be called in object _setup function
		static void enable_signal(t_class *classPtr,float def = 0.) {
#ifdef PD
			((Obj_header *)classPtr)->defsig = def;
			CLASS_MAINSIGNALIN(classPtr, Obj_header, defsig);
#elif defined(MAX)
			dsp_initclass();
#endif
		}

    private:

#ifdef PD
        //////////
        // The canvas that the object is in
        t_canvas            *m_canvas;
#endif
};

// This has a dummy arg so that NT won't complain
EXT_EXTERN void *operator new(size_t, void *location, void *dummy);

////////////////////////////////////////
// This should be used in the header
////////////////////////////////////////

#define CPPEXTERN_HEADER(NEW_CLASS, PARENT_CLASS)    	    	\
public:     	    	    \
static const char *thisName() { return #NEW_CLASS; } 	    	\
static void callb_free(void *data)    	    	    	\
{ CPPExtern *mydata = ((Obj_header *)data)->data; delete mydata; \
  ((Obj_header *)data)->Obj_header::~Obj_header(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr);    	    	\
  NEW_CLASS::cb_setup(classPtr); }  	    	    	\
private:    	    	    	    	    	    	    	\
static inline NEW_CLASS *thisClass(void *data) {return((NEW_CLASS *)((Obj_header *)data)->data);} \
static void cb_setup(t_class *classPtr);


////////////////////////////////////////
// This should be the first thing in the implementation file
////////////////////////////////////////

//
// NO ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW(NEW_CLASS)    	    	    	    	\
    REAL_NEW(NEW_CLASS, _setup, _class)

//
// ONE ARGUMENT
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_ONE_ARG(NEW_CLASS, TYPE, PD_TYPE)    \
    REAL_NEW_WITH_ARG(NEW_CLASS, _setup, _class, TYPE, PD_TYPE)

//
// GIMME ARGUMENT
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_GIMME(NEW_CLASS)  	    	    	\
    REAL_NEW_WITH_GIMME(NEW_CLASS, _setup, _class)

//
// TWO ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_TWO_ARGS(NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO)	\
    REAL_NEW_WITH_ARG_ARG(NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO)

//
// THREE ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_THREE_ARGS(NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)	\
    REAL_NEW_WITH_ARG_ARG_ARG(NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE)

//
// FOUR ARGUMENTS
/////////////////////////////////////////////////
#define CPPEXTERN_NEW_WITH_FOUR_ARGS(NEW_CLASS, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR) \
    REAL_NEW_WITH_ARG_ARG_ARG_ARG(NEW_CLASS, _setup, _class, TYPE, PD_TYPE, TTWO, PD_TWO, TTHREE, PD_THREE, TFOUR, PD_FOUR)

////////////////////////////////////////
// These definitions are used below
////////////////////////////////////////

#ifdef PD
#define CPPEXTERN_NEWFN class_new
#define CPPEXTERN_CLREF(NEW_CLASS) gensym(#NEW_CLASS)
#elif defined(MAX)
#define CPPEXTERN_NEWFN setup
#define CPP_EXTERN_CLREF(NEW_CLASS) (t_messlist **)&NEW_CLASS
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
#define REAL_NEW(NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME)        \
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
    	     	CPPEXTERN_CLREF(NEW_CLASS), 	    	    	     	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::cb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	A_NULL);                                        \
    NEW_CLASS::cb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// one arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG(NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, VAR_TYPE, PD_TYPE) \
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
    	     	CPPEXTERN_CLREF(NEW_CLASS), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::cb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	PD_TYPE,                                        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::cb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// gimme arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_GIMME(NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME) \
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
    	     	CPPEXTERN_CLREF(NEW_CLASS), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::cb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	A_GIMME,                                        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::cb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// two args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG_ARG(NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_VAR_TYPE, ONE_PD_TYPE, TWO_VAR_TYPE, TWO_PD_TYPE) \
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
    	     	CPPEXTERN_CLREF(NEW_CLASS), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::cb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE,                       \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::cb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// three args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG_ARG_ARG(NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_VAR_TYPE, ONE_PD_TYPE, TWO_VAR_TYPE, TWO_PD_TYPE, THREE_VAR_TYPE, THREE_PD_TYPE) \
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
    	     	CPPEXTERN_CLREF(NEW_CLASS), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::cb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE, THREE_PD_TYPE,        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::cb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// four args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_WITH_ARG_ARG_ARG_ARG(NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_VAR_TYPE, ONE_PD_TYPE, TWO_VAR_TYPE, TWO_PD_TYPE, THREE_VAR_TYPE, THREE_PD_TYPE, FOUR_VAR_TYPE, FOUR_PD_TYPE) \
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
    	     	CPPEXTERN_CLREF(NEW_CLASS), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::cb_free,         \
    	     	sizeof(Obj_header), 0,                          \
    	     	ONE_PD_TYPE, TWO_PD_TYPE, THREE_PD_TYPE, FOUR_PD_TYPE, \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::cb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

#endif
