/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flbase.h
	\brief Internal flext base classes
    
	\remark This is all derived from GEM by Mark Danks
*/
 
#ifndef __FLEXT_BASE_H
#define __FLEXT_BASE_H

#include "flstdc.h"
#include "flsupport.h"

class FLEXT_EXT flext_obj;

// ----------------------------------------------------------------------------
/*! \brief The obligatory PD or Max/MSP object header
	\internal

    This is in a separate struct to assure that obj is the very first thing.  
    If it were the first thing in flext_obj, then there could be problems with
    the virtual table of the C++ class.
*/
// ----------------------------------------------------------------------------

struct FLEXT_EXT flext_hdr
{
	/*!	\defgroup FLEXT_OBJHEADER Actual PD or Max/MSP object
		\internal
		@{ 
	*/

    	/*! \brief The obligatory object header
			\note MUST reside at memory offset 0 (no virtual table possible)
		*/
    	t_sigobj    	    obj;  

#if FLEXT_SYS == FLEXT_SYS_PD
		//! PD only: float signal holder for pd
		float defsig;			
#endif

#if FLEXT_SYS == FLEXT_SYS_MAX
		//! Max/MSP only: current inlet used by proxy objects
		long curinlet;      
#endif

    	/*! \brief This points to the actual polymorphic C++ class
		*/
        flext_obj           *data;

	//!	@}  FLEXT_OBJHEADER
};


// ----------------------------------------------------------------------------
/*! \brief The mother of base classes for all flext external objects

    Each extern which is written in C++ needs to use the #defines at the
    end of this header file.  
    
    The define
    
        FLEXT_HEADER(NEW_CLASS, PARENT_CLASS)
    
    should be somewhere in your header file.
    One of the defines like
    
    FLEXT_NEW(NEW_CLASS)
	or
    FLEXT_NEW_2(NEW_CLASS, float, float)
    
    should be the first thing in your implementation file.
    NEW_CLASS is the name of your class and PARENT_CLASS is the 
    parent of your class.
*/
// ----------------------------------------------------------------------------

class FLEXT_EXT flext_obj:
	public flext
{
    public:

	/*!	\defgroup FLEXT_OBJCLASS Object base class
		@{ 
	*/
// --- creation -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_CREATION Creation/Destruction functionality
		@{ 
	*/

        //! Constructor
    	flext_obj();

    	//! Destructor
    	virtual ~flext_obj() = 0;

        /*! \brief Signal a construction problem
			\note This should only be used in the constructor. Object creation will be aborted.
		*/
		void InitProblem() { init_ok = false; }

		/*! \brief Enable/disable attribute procession (default = false)
			\note Use that in the static class setup function (also library setup function)
		*/
		static void ProcessAttributes(bool attr) { process_attributes = attr; }

		//! Virtual function called at creation time (but after the constructor)
		// this also guarantees that there are no instances of flext_obj
		virtual bool Init() = 0; 
	
		//! Virtual function called at destruction (before the destructor)
		virtual void Exit() {}

	//!	@}  FLEXT_O_CREATION

// --- info -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_INFO Get various information
		@{ 
	*/

        //! Get the object's canvas
        t_canvas *thisCanvas() { return(m_canvas); }

        //! Get the PD or Max/MSP object
		t_sigobj *thisHdr() { return &x_obj->obj; }
        //! Get the PD or Max/MSP object
		const t_sigobj *thisHdr() const { return &x_obj->obj; }
        //! Get the class name (as a string)
		const char *thisName() const { return GetString(m_name); } 
        //! Get the class name (as a symbol)
		const t_symbol *thisNameSym() const { return m_name; } 

#if FLEXT_SYS == FLEXT_SYS_PD
        //! Get the class pointer
		t_class *thisClass() { return (t_class *)((t_object *)(x_obj))->te_g.g_pd; }
#elif FLEXT_SYS == FLEXT_SYS_MAX
        //! Get the class pointer
		t_class *thisClass() { return (t_class *)(((t_tinyobject *)x_obj)->t_messlist-1); } 
#endif

	//!	@}  FLEXT_O_INFO

// --- memory -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_MEMORY Memory allocation functions
		@{ 
	*/

		/*! Overloaded new memory allocation method
			\warning Max/MSP (or MacOS) allows only 16K in overdrive mode!
		*/
		void *operator new(size_t bytes);
		//! Overloaded delete method
		void operator delete(void *blk);

		#ifndef __MRC__ // doesn't allow new[] overloading?!
		void *operator new[](size_t bytes) { return operator new(bytes); }
		void operator delete[](void *blk) { operator delete(blk); }
		#endif

		//! Get an aligned memory block
		static void *NewAligned(size_t bytes,int bitalign = 128);
		//! Free an aligned memory block
		static void FreeAligned(void *blk);
		
	//!	@}  FLEXT_O_MEMORY
    	
// --- help -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_HELP Help/assistance functionality
		\remark This is still PD only
		@{ 
	*/

		/*! Define the help reference symbol for a class
			\internal
		*/
		static void DefineHelp(t_class *c,const char *ref,const char *dir = NULL,bool addtilde = false);

		//! Define the help reference symbol for a class
		void DefineHelp(const char *ref,const char *dir = NULL,bool addtilde = false) { DefineHelp(thisClass(),ref,dir,addtilde); }

	//!	@} 


// --- internal stuff -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_INTERNAL Internal stuff
		\internal
		@{ 
	*/

    protected:    	
		
        //! The object header
        flext_hdr          *x_obj;        	

        //! Flag for attribute procession
        bool				procattr;

		static bool				process_attributes;

    private:

        //! The canvas (patcher) that the object is in
        t_canvas            *m_canvas;
        
        //! Flag for successful object construction
        bool				init_ok;

	public:

    	//! Creation callback
		static void __setup__(t_class *) { flext::Setup(); }	

		/*! \brief This is a temporary holder
			\warning don't touch it!
		*/
        static flext_hdr     *m_holder;
		//! Hold object's name during construction
        static const t_symbol *m_holdname;  

		//! Holders for attribute procession flag
		static bool m_holdattr;
		static int m_holdaargc;
		static const t_atom *m_holdaargv;

        //! The object's name in the patcher
		const t_symbol *m_name;

		//! Check whether construction was successful
		bool InitOk() const { return init_ok; }

		// Definitions for library objects
		static void lib_init(const char *name,void setupfun(),bool attr);
		static void obj_add(bool lib,bool dsp,bool attr,const char *idname,const char *names,void setupfun(t_class *),flext_obj *(*newfun)(int,t_atom *),void (*freefun)(flext_hdr *),int argtp1,...);
		static flext_hdr *obj_new(const t_symbol *s,int argc,t_atom *argv);
		static void obj_free(flext_hdr *o);

	//!	@} FLEXT_O_INTERNAL

	//!	@} FLEXT_OBJCLASS   	
};


// max. 4 creation args (see the following macros)
#define FLEXT_MAXNEWARGS 4 

// max. 5 method args (see the following macros)
#define FLEXT_MAXMETHARGS 5 

// prefixes for the macro generated handler functions
#define FLEXT_CALL_PRE(F) flext_c_##F
#define FLEXT_THR_PRE(F) flext_t_##F
#define FLEXT_GET_PRE(F) flext_g_##F
#define FLEXT_SET_PRE(F) flext_s_##F


#ifndef FLEXT_ATTRIBUTES
/*! \brief Switch for global attribute processing
	\note Should be set to 1 or 0 (or not be defined)
	\ingroup FLEXT_DEFS
*/
#define FLEXT_ATTRIBUTES \
\
0


#elif FLEXT_ATTRIBUTES != 0 && FLEXT_ATTRIBUTES != 1
#error "FLEXT_ATTRIBUTES must be 0 or 1"
#endif

// ----------------------------------------
// These should be used in the header
// ----------------------------------------


#define FLEXT_REALHDR(NEW_CLASS, PARENT_CLASS)    	    	\
public:     	    	    \
typedef NEW_CLASS thisType;  \
static flext_obj *__init__(int argc,t_atom *argv);  \
static void __free__(flext_hdr *hdr)    	    	    	\
{ flext_obj *mydata = hdr->data; delete mydata; \
  hdr->flext_hdr::~flext_hdr(); }   	    	\
static void __setup__(t_class *classPtr) { 	    	\
	PARENT_CLASS::__setup__(classPtr); } \
protected:    \
static inline NEW_CLASS *thisObject(void *c) { return FLEXT_CAST<NEW_CLASS *>(((flext_hdr *)c)->data); } 


#define FLEXT_REALHDR_S(NEW_CLASS, PARENT_CLASS,SETUPFUN)    	    	\
public:     	    	    \
typedef NEW_CLASS thisType;  \
static flext_obj *__init__(int argc,t_atom *argv);  \
static void __free__(flext_hdr *hdr)    	    	    	\
{ flext_obj *mydata = hdr->data; delete mydata; \
  hdr->flext_hdr::~flext_hdr(); }   	    	\
static void __setup__(t_class *classPtr)  	    	\
{ PARENT_CLASS::__setup__(classPtr);    	    	\
	NEW_CLASS::SETUPFUN(classPtr); 	}    	    	\
protected: \
static inline NEW_CLASS *thisObject(void *c) { return FLEXT_CAST<NEW_CLASS *>(((flext_hdr *)c)->data); }


// generate name of dsp/non-dsp setup function
#define FLEXT_STPF_0(NAME) NAME##_setup
#define FLEXT_STPF_1(NAME) NAME##_tilde_setup
#define FLEXT_STPF_(DSP) FLEXT_STPF_##DSP
#define FLEXT_STPF(NAME,DSP) FLEXT_STPF_(DSP)(NAME)



// --------------------------------------------------------------------------------------



// these can be used in library setup functions 
// to register the individual objects in the library

#define REAL_SETUP_0(cl) \
extern void cl##_setup(); \
cl##_setup()  

#define REAL_SETUP_1(cl) \
extern void cl##_tilde_setup(); \
cl##_tilde_setup()  

#define REAL_SETUP(cl,DSP) REAL_SETUP_##DSP(cl)

// specify that to define the library itself

#if FLEXT_SYS == FLEXT_SYS_PD
#define REAL_LIB_SETUP(NAME,SETUPFUN) extern "C" FLEXT_EXT void NAME##_setup() { flext_obj::lib_init(#NAME,SETUPFUN,FLEXT_ATTRIBUTES); }
#elif FLEXT_SYS == FLEXT_SYS_MAX
#define REAL_LIB_SETUP(NAME,SETUPFUN) extern "C" FLEXT_EXT int main() { flext_obj::lib_init(#NAME,SETUPFUN,FLEXT_ATTRIBUTES); return 0; }
#else
#error
#endif


// --------------------------------------------------


#define FLEXT_EXP_0 extern "C" FLEXT_EXT
#define FLEXT_EXP_1 
#define FLEXT_EXP(LIB) FLEXT_EXP_##LIB

#if FLEXT_SYS == FLEXT_SYS_PD
#define FLEXT_OBJ_SETUP_0(NEW_CLASS,DSP)
#elif FLEXT_SYS == FLEXT_SYS_MAX
#define FLEXT_OBJ_SETUP_0(NEW_CLASS,DSP) extern "C" FLEXT_EXT int main() { FLEXT_STPF(NEW_CLASS,DSP)(); return 0; }
#else
#error
#endif

#define FLEXT_OBJ_SETUP_1(NEW_CLASS,DSP)

#define FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB) FLEXT_OBJ_SETUP_##LIB(NEW_CLASS,DSP)



// ----------------------------------------
// These definitions are used below
// ----------------------------------------

// Shortcuts for PD/Max type arguments
#define FLEXTTYPE_void A_NULL
#define CALLBTYPE_void void
#define FLEXTTYPE_float A_FLOAT
#define CALLBTYPE_float float
#define FLEXTTYPE_t_float A_FLOAT
#define CALLBTYPE_t_float t_float

#if FLEXT_SYS == FLEXT_SYS_PD
#define FLEXTTYPE_int A_FLOAT
#define CALLBTYPE_int float
#elif FLEXT_SYS == FLEXT_SYS_MAX
#define FLEXTTYPE_int A_INT
#define CALLBTYPE_int int
#else
#error
#endif

#define FLEXTTYPE_t_symptr A_SYMBOL
#define CALLBTYPE_t_symptr t_symptr
#define FLEXTTYPE_t_symtype A_SYMBOL
#define CALLBTYPE_t_symtype t_symptr
#define FLEXTTYPE_t_ptrtype A_POINTER
#define CALLBTYPE_t_ptrtype t_ptrtype

#define FLEXTTP(TP) FLEXTTYPE_ ## TP
#define CALLBTP(TP) CALLBTYPE_ ## TP


#define ARGMEMBER_int(a) GetInt(a)
#define ARGMEMBER_float(a) GetFloat(a)
#define ARGMEMBER_t_symptr(a) GetSymbol(a)
#define ARGMEMBER_t_symtype(a) GetSymbol(a)
#define ARGCAST(arg,tp) ARGMEMBER_##tp(arg)


#define REAL_NEW(NAME,NEW_CLASS,DSP,LIB) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS;                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,&NEW_CLASS::__free__,A_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_V(NAME,NEW_CLASS,DSP,LIB) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(argc,argv);                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,&NEW_CLASS::__free__,A_GIMME,A_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_1(NAME,NEW_CLASS,DSP,LIB, TYPE1) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),A_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_2(NAME,NEW_CLASS,DSP,LIB, TYPE1,TYPE2) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1),ARGCAST(argv[1],TYPE2));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTP(TYPE2),A_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_3(NAME,NEW_CLASS,DSP,LIB, TYPE1, TYPE2, TYPE3) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1),ARGCAST(argv[1],TYPE2),ARGCAST(argv[2],TYPE3));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTP(TYPE3),A_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_4(NAME,NEW_CLASS,DSP,LIB, TYPE1,TYPE2, TYPE3, TYPE4) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1),ARGCAST(argv[1],TYPE2),ARGCAST(argv[2],TYPE3),ARGCAST(argv[3],TYPE4));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTP(TYPE3),FLEXTTP(TYPE4),A_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)


// Shortcuts for method arguments:
#define FLEXTARG_float a_float
#define FLEXTARG_int a_int
#define FLEXTARG_bool a_int
#define FLEXTARG_t_float a_float
#define FLEXTARG_t_symtype a_symbol
#define FLEXTARG_t_symptr a_symbol
#define FLEXTARG_t_ptrtype a_pointer

#define FLEXTARG(TP) FLEXTARG_ ## TP


#endif




