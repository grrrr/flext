/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all derived from GEM by Mark Danks


#ifndef _FLBASE_H
#define _FLBASE_H

#include <flstdc.h>

class flext_obj;

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    flext_hdr
    
    The obligatory object header

DESCRIPTION
    
    This is in a separate struct to assure that obj is the very first thing.  
    If it were the first thing in flext_obj, then there could be problems with
    the vtable.
     
-----------------------------------------------------------------*/
struct FLEXT_EXT flext_hdr
{
    	//////////
    	// The obligatory object header
    	t_sigobj    	    obj;  // MUST reside at memory offset 0

#ifdef PD
		float defsig;			// float signal holder for pd
#endif

#if defined(MAXMSP) 
		long curinlet;      // current inlet used by proxy objects
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
    end of this header file.  
    
    The define
    
        FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)
    
    should be somewhere in your header file.
    One of the defines like
    
    FLEXT_NEW(NEW_CLASS)
    FLEXT_NEW_2(NEW_CLASS, float, float)
    
    should be the first thing in your implementation file.
    NEW_CLASS is the name of your class and PARENT_CLASS is the 
    parent of your class.
        
-----------------------------------------------------------------*/
class FLEXT_EXT flext_obj
{
    public:

		// --- overloading of new/delete memory allocation methods ----

		void *operator new(size_t bytes);
		void operator delete(void *blk);

		#ifndef __MRC__ // doesn't allow new[] overloading?!
		void *operator new[](size_t bytes) { return operator new(bytes); }
		void operator delete[](void *blk) { operator delete(blk); }
		#endif

		// these are aligned 
		void *NewAligned(size_t bytes,int bitalign = 128);
		void FreeAligned(void *blk);
		
		// ---------------------

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
        flext_hdr          *x_obj;        	

    private:

        //////////
        // The canvas (patcher) that the object is in
        t_canvas            *m_canvas;

	public:

    	//////////
    	// Creation callback
    	static void callb_setup(t_class *) {}	

        //////////
        // This is a holder - don't touch it
        static flext_hdr     *m_holder;
        static const char *m_holdname;  // hold object's name during construction

        //////////
        // The object's name in the patcher
		const char *m_name;
		
#ifdef _DEBUG
		static bool check_tilde(const char *objname,const char *setupfun);
#endif
};

// This has a dummy arg so that NT won't complain
inline void *operator new(size_t, void *location, void *) { return location; }

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
typedef NEW_CLASS thisType;  \
static void callb_free(void *data)    	    	    	\
{ flext_obj *mydata = ((flext_hdr *)data)->data; delete mydata; \
  ((flext_hdr *)data)->flext_hdr::~flext_hdr(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr); }  	    	    	\
protected:    \
inline t_sigobj *thisHdr() { return &x_obj->obj; } \
inline t_class *thisClass() { return FLEXT_GETCLASS(x_obj); } \
inline const char *thisName() const { return m_name; } \
static NEW_CLASS *thisObject(void *c) { return (NEW_CLASS *)((flext_hdr *)c)->data; }	  


#define FLEXT_HEADER_S(NEW_CLASS, PARENT_CLASS,SETUPFUN)    	    	\
public:     	    	    \
typedef NEW_CLASS thisType;  \
static void callb_free(void *data)    	    	    	\
{ flext_obj *mydata = ((flext_hdr *)data)->data; delete mydata; \
  ((flext_hdr *)data)->flext_hdr::~flext_hdr(); }   	    	\
static void callb_setup(t_class *classPtr)  	    	\
{ PARENT_CLASS::callb_setup(classPtr);    	    	\
	NEW_CLASS::SETUPFUN(classPtr); }  	    	    	\
protected:    \
inline t_sigobj *thisHdr() { return &x_obj->obj; } \
inline t_class *thisClass() { return FLEXT_GETCLASS(x_obj); } \
inline const char *thisName() const { return m_name; } \
static NEW_CLASS *thisObject(void *c) { return (NEW_CLASS *)((flext_hdr *)c)->data; }	  


////////////////////////////////////////
// This should be the first thing in the implementation file
////////////////////////////////////////

//
// NO ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW(NAME,NEW_CLASS)    	\
    REAL_NEW(NAME,NEW_CLASS, _setup, _class)

#define FLEXT_NEW_TILDE(NAME,NEW_CLASS)    \
    REAL_NEW(NAME,NEW_CLASS, _tilde_setup, _class)

//
// ONE ARGUMENT
/////////////////////////////////////////////////
#define FLEXT_NEW_1(NAME,NEW_CLASS, TYPE)    \
    REAL_NEW_1(NAME,NEW_CLASS, _setup, _class, TYPE)

#define FLEXT_NEW_TILDE_1(NAME,NEW_CLASS, TYPE)    \
    REAL_NEW_1(NAME,NEW_CLASS, _tilde_setup, _class, TYPE)

//
// GIMME ARGUMENT
/////////////////////////////////////////////////
#define FLEXT_NEW_G(NAME,NEW_CLASS)  	 \
    REAL_NEW_G(NAME,NEW_CLASS, _setup, _class)

#define FLEXT_NEW_TILDE_G(NAME,NEW_CLASS)  	 \
    REAL_NEW_G(NAME,NEW_CLASS, _tilde_setup, _class)

//
// TWO ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_2(NAME,NEW_CLASS, TYPE, TTWO)	\
    REAL_NEW_2(NAME,NEW_CLASS, _setup, _class, TYPE, TTWO)

#define FLEXT_NEW_TILDE_2(NAME,NEW_CLASS, TYPE, TTWO)	\
    REAL_NEW_2(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, TTWO)

//
// THREE ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_3(NAME,NEW_CLASS, TYPE, TTWO, TTHREE)	\
    REAL_NEW_3(NAME,NEW_CLASS, _setup, _class, TYPE, TTWO, TTHREE)

#define FLEXT_NEW_TILDE_3(NAME,NEW_CLASS, TYPE, TTWO, TTHREE)	\
    REAL_NEW_3(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, TTWO, TTHREE)


/*
// MaxMSP doesn't seem to be able more than 3 creation arguments!

//
// FOUR ARGUMENTS
/////////////////////////////////////////////////
#define FLEXT_NEW_4(NAME,NEW_CLASS, TYPE, TTWO, TTHREE, TFOUR) \
    REAL_NEW_4(NAME,NEW_CLASS, _setup, _class, TYPE, TTWO, TTHREE, TFOUR)

#define FLEXT_NEW_TILDE_4(NAME,NEW_CLASS, TYPE, TTWO, TTHREE, TFOUR) \
    REAL_NEW_4(NAME,NEW_CLASS, _tilde_setup, _class, TYPE, TTWO, TTHREE, TFOUR)
*/



////////////////////////////////////////
// These definitions are used below
////////////////////////////////////////

// Shortcuts for PD/Max type arguments
#define FLEXTTYPE_float A_FLOAT
#define FLEXTTYPE_t_float A_FLOAT
#define FLEXTTYPE_t_flint A_FLINT
#define FLEXTTYPE_t_symtype A_SYMBOL
#define FLEXTTYPE_t_ptrtype A_POINTER

#define FLEXTTP(TP) FLEXTTYPE_ ## TP


#ifdef PD
#define FLEXT_NEWFN ::class_new
#define FLEXT_CLREF(NAME,CLASS) gensym(NAME)
#define FLEXT_MAIN(MAINNAME) MAINNAME
#define CLNEW_OPTIONS 0  // flags for class creation

#define newobject(CLSS) pd_new(CLSS)

#elif defined(MAXMSP)
#define FLEXT_NEWFN NULL; ::setup    // extremely ugly!!! I hope Mark Danks doesn't see that......
#define FLEXT_CLREF(NAME,CLASS) (t_messlist **)&(CLASS)
#define FLEXT_MAIN(MAINNAME) main
#define CLNEW_OPTIONS 0  // flags for class creation
#endif

#ifdef _DEBUG
#define CHECK_TILDE(OBJNAME,SETUPFUN) flext_obj::check_tilde(OBJNAME,SETUPFUN)
#else
#define CHECK_TILDE(OBJNAME,SETUPFUN) ((void)0)
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
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS;                                  \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{					\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
    	     	A_NULL);                                        \
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// one arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_1(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (TYPE arg)                  \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg);                             \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
    	     	FLEXTTP(TYPE),                                        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// gimme arg
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_G(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (t_symbol *, int argc, t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(argc, argv);                      \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
    	     	A_GIMME,                                        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// two args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_2(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_TYPE, TWO_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (ONE_TYPE arg, TWO_TYPE argtwo) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg, argtwo);                     \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
    	     	FLEXTTP(ONE_TYPE), FLEXTTP(TWO_TYPE),                       \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// three args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_3(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_TYPE, TWO_TYPE, THREE_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (ONE_TYPE arg, TWO_TYPE argtwo, THREE_TYPE argthree) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg, argtwo, argthree);           \
    flext_obj::m_holder = NULL;                                 \
	return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), CLNEW_OPTIONS,                     \
    	     	FLEXTTP(ONE_TYPE),FLEXTTP(TWO_TYPE),FLEXTTP(THREE_TYPE),        \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

///////////////////////////////////////////////////////////////////////////////
// four args
///////////////////////////////////////////////////////////////////////////////
#define REAL_NEW_4(NAME,NEW_CLASS, SETUP_FUNCTION, EXTERN_NAME, ONE_TYPE, TWO_TYPE, THREE_TYPE, FOUR_TYPE) \
static t_class * NEW_CLASS ## EXTERN_NAME;    	    	    	\
void * EXTERN_NAME ## NEW_CLASS (ONE_TYPE arg, TWO_TYPE argtwo, THREE_TYPE argthree, FOUR_TYPE argfour) \
{     	    	    	    	    	    	    	    	\
    flext_hdr *obj = new (newobject(NEW_CLASS ## EXTERN_NAME),(void *)NULL) flext_hdr; \
    flext_obj::m_holder = obj;                         \
    flext_obj::m_holdname = NAME;                         \
    obj->data = new NEW_CLASS(arg, argtwo, argthree, argfour);  \
    flext_obj::m_holder = NULL;                                 \
    return(obj);                                                \
}   	    	    	    	    	    	    	    	\
extern "C" {	    	    	    	    	    	    	\
FLEXT_EXT void FLEXT_MAIN(NEW_CLASS ## SETUP_FUNCTION)()    	    	    	    	\
{   	    	    	    	    	    	    	    	\
	CHECK_TILDE(NAME,#SETUP_FUNCTION); 	\
    NEW_CLASS ## EXTERN_NAME = FLEXT_NEWFN(                       \
    	     	FLEXT_CLREF(NAME,NEW_CLASS ## EXTERN_NAME), 	    	    	    	\
    	    	(t_newmethod)EXTERN_NAME ## NEW_CLASS,	    	\
    	    	(t_method)&NEW_CLASS::callb_free,         \
    	     	sizeof(flext_hdr), CLNEW_OPTIONS,                          \
    	     	FLEXTTP(ONE_TYPE),FLEXTTP(TWO_TYPE),FLEXTTP(THREE_TYPE),FLEXTTP(FOUR_TYPE), \
    	     	A_NULL);      	    	    	    	    	\
    NEW_CLASS::callb_setup(NEW_CLASS ## EXTERN_NAME); \
}   	    	    	    	    	    	    	    	\
}

#endif




