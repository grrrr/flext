/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flbase.h
	\brief Internal flext base classes
    
	\remark This uses some ideas of GEM invented by Mark Danks
*/
 
#ifndef __FLEXT_BASE_H
#define __FLEXT_BASE_H

#include "flstdc.h"
#include "flsupport.h"


class FLEXT_SHARE FLEXT_CLASSDEF(flext_obj);
typedef class FLEXT_CLASSDEF(flext_obj) flext_obj;

// ----------------------------------------------------------------------------
/*! \brief The obligatory PD or Max/MSP object header
	\internal

    This is in a separate struct to assure that obj is the very first thing.  
    If it were the first thing in flext_obj, then there could be problems with
    the virtual table of the C++ class.
*/
// ----------------------------------------------------------------------------

struct FLEXT_SHARE flext_hdr
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
        FLEXT_CLASSDEF(flext_obj) *data;

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

class FLEXT_SHARE FLEXT_CLASSDEF(flext_obj):
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
    	FLEXT_CLASSDEF(flext_obj)();

    	//! Destructor
    	virtual ~FLEXT_CLASSDEF(flext_obj)();

        /*! \brief Signal a construction problem
			\note This should only be used in the constructor. Object creation will be aborted.
		*/
		void InitProblem() { init_ok = false; }

		/*! \brief Enable/disable attribute procession (default = false)
			\note Use that in the static class setup function (also library setup function)
		*/
		static void ProcessAttributes(bool attr); //{ process_attributes = attr; }

		//! Virtual function called at creation time (but after the constructor)
		// this also guarantees that there are no instances of flext_obj
		virtual bool Init(); 

		//! Virtual function called after Init() has succeeded
		virtual bool Finalize();
	
		//! Virtual function called at destruction (before the destructor)
		virtual void Exit();

	//!	@}  FLEXT_O_CREATION

// --- info -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_INFO Get various information
		@{ 
	*/

        //! Get the object's canvas
        t_canvas *thisCanvas() const { return m_canvas; }

        //! Get the PD or Max/MSP object
		t_sigobj *thisHdr() { FLEXT_ASSERT(x_obj); return &x_obj->obj; }
		const t_sigobj *thisHdr() const { FLEXT_ASSERT(x_obj); return &x_obj->obj; }
        //! Get the class name (as a string)
		const char *thisName() const { return GetString(m_name); } 
        //! Get the class name (as a symbol)
		const t_symbol *thisNameSym() const { return m_name; } 


        //! Get the class pointer
#if FLEXT_SYS == FLEXT_SYS_PD
		t_class *thisClass() const { FLEXT_ASSERT(x_obj); return (t_class *)((t_object *)(x_obj))->te_g.g_pd; }
#elif FLEXT_SYS == FLEXT_SYS_MAX
		t_class *thisClass() const { FLEXT_ASSERT(x_obj); return (t_class *)(((t_tinyobject *)x_obj)->t_messlist-1); } 
#elif FLEXT_SYS == FLEXT_SYS_JMAX
		t_class *thisClass() const { return fts_object_get_class((fts_object_t *)thisHdr()); }
#else
#error
#endif	
		
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_JMAX
		//! Typedef for unique class identifier
		typedef t_class *t_classid;
		//! Get unique id for object class
		t_classid thisClassId() const { return thisClass(); }
		//! Get class pointer from class id
		static t_class *getClass(t_classid c) { return c; }
#else
		//! Typedef for unique class identifier
		typedef void *t_classid;
		//! Get unique id for object class (for Max/MSP library object share the same (t_class *)!)
		t_classid thisClassId() const; 
		//! Get class pointer from class id
		static t_class *getClass(t_classid);
#endif

	//!	@}  FLEXT_O_INFO

// --- help -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_HELP Help/assistance functionality
		\remark This is still PD only
		@{ 
	*/

		/*! Define the help reference symbol for a class
			\internal
		*/
		static void DefineHelp(t_classid c,const char *ref,const char *dir = NULL,bool addtilde = false);

		//! Define the help reference symbol for a class
		void DefineHelp(const char *ref,const char *dir = NULL,bool addtilde = false) { DefineHelp(thisClassId(),ref,dir,addtilde); }

	//!	@} 


// --- internal stuff -------------------------------------------------------	

	/*!	\defgroup FLEXT_O_INTERNAL Internal stuff
		\internal
		@{ 
	*/

    protected:    	
		
        //! The object header
        mutable flext_hdr          *x_obj;        	

        //! Flag for attribute procession
        bool				procattr;

		static bool				process_attributes;

    private:

        //! The canvas (patcher) that the object is in
        mutable t_canvas            *m_canvas;
        
        //! Flag for successful object construction
        bool				init_ok;

        // flags for init and exit procedure;
        static bool initing;
        static bool exiting;

	public:

    	//! Creation callback
		static void __setup__(t_classid) { flext::Setup(); }	

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

        /*! Return true if in object initialization phase
            true when in constructor or Init, false when in Finalize
        */
        static bool Initing() { return initing; }

        //! Return true if in object destruction phase (Exit or destructor)
        static bool Exiting() { return exiting; }

		// Definitions for library objects
		static void lib_init(const char *name,void setupfun(),bool attr);
		static void obj_add(bool lib,bool dsp,bool attr,const char *idname,const char *names,void setupfun(t_classid),FLEXT_CLASSDEF(flext_obj) *(*newfun)(int,t_atom *),void (*freefun)(flext_hdr *),int argtp1,...);
#if FLEXT_SYS == FLEXT_SYS_JMAX
		static void obj_new(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
		static void obj_free(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
#else
#if FLEXT_SYS == FLEXT_SYS_MAX
		static flext_hdr *obj_new(const t_symbol *s,short argc,t_atom *argv);
#else
		static flext_hdr *obj_new(const t_symbol *s,int argc,t_atom *argv);
#endif
		static void obj_free(flext_hdr *o);
#endif

		//! Convert $0 or #0 symbol into appropriate value
		static bool GetParamSym(t_atom &dst,const t_symbol *s,t_canvas *c);

		//! Get the canvas arguments
		void CanvasArgs(AtomList &args) const;

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
static FLEXT_CLASSDEF(flext_obj) *__init__(int argc,t_atom *argv);  \
static void __free__(flext_hdr *hdr)    	    	    	\
{ FLEXT_CLASSDEF(flext_obj) *mydata = hdr->data; delete mydata; \
  hdr->flext_hdr::~flext_hdr(); }   	    	\
static void __setup__(t_classid classid) { 	    	\
	PARENT_CLASS::__setup__(classid); } \
protected:    \
static inline NEW_CLASS *thisObject(void *c) { return FLEXT_CAST<NEW_CLASS *>(((flext_hdr *)c)->data); } 


#define FLEXT_REALHDR_S(NEW_CLASS, PARENT_CLASS,SETUPFUN)    	    	\
public:     	    	    \
typedef NEW_CLASS thisType;  \
static FLEXT_CLASSDEF(flext_obj) *__init__(int argc,t_atom *argv);  \
static void __free__(flext_hdr *hdr)    	    	    	\
{ FLEXT_CLASSDEF(flext_obj) *mydata = hdr->data; delete mydata; \
  hdr->flext_hdr::~flext_hdr(); }   	    	\
static void __setup__(t_classid classid)  	    	\
{ PARENT_CLASS::__setup__(classid);    	    	\
	NEW_CLASS::SETUPFUN(classid); 	}    	    	\
protected: \
static inline NEW_CLASS *thisObject(void *c) { return FLEXT_CAST<NEW_CLASS *>(((flext_hdr *)c)->data); }

// generate name of dsp/non-dsp setup function
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	#define FLEXT_STPF_0(NAME) NAME##_setup
	#define FLEXT_STPF_1(NAME) NAME##_tilde_setup
#else
	#define FLEXT_STPF_0(NAME) NAME##_config
	#define FLEXT_STPF_1(NAME) signal_##NAME##_config
#endif

#define FLEXT_STPF_(DSP) FLEXT_STPF_##DSP
#define FLEXT_STPF(NAME,DSP) FLEXT_STPF_(DSP)(NAME)



// --------------------------------------------------------------------------------------



// these can be used in library setup functions 
// to register the individual objects in the library

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	#define REAL_SETUP_0(cl) \
	extern void cl##_setup(); cl##_setup()  
	
	#define REAL_SETUP_1(cl) \
	extern void cl##_tilde_setup(); cl##_tilde_setup()  
#else
	#define REAL_SETUP_0(cl) \
	extern void cl##_config(); cl##_config()  
	
	#define REAL_SETUP_1(cl) \
	extern void signal_##cl##_config(); signal_##cl##_config()  
#endif

#define REAL_SETUP(cl,DSP) REAL_SETUP_##DSP(cl)

// specify that to define the library itself

#if FLEXT_SYS == FLEXT_SYS_PD
#define REAL_LIB_SETUP(NAME,SETUPFUN) extern "C" FLEXT_EXT void NAME##_setup() { flext_obj::lib_init(#NAME,SETUPFUN,FLEXT_ATTRIBUTES); }
#elif FLEXT_SYS == FLEXT_SYS_MAX
#define REAL_LIB_SETUP(NAME,SETUPFUN) extern "C" FLEXT_EXT int main() { flext_obj::lib_init(#NAME,SETUPFUN,FLEXT_ATTRIBUTES); return 0; }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
#define REAL_LIB_SETUP(NAME,SETUPFUN) \
static void __##NAME##_initfun() { flext_obj::lib_init(#NAME,SETUPFUN,FLEXT_ATTRIBUTES); } \
fts_module_t __##NAME##_module = {#NAME,#NAME,__##NAME##_initfun,0,0};
#else
#error
#endif


// --------------------------------------------------


#define FLEXT_EXP_0 extern "C" FLEXT_EXT
#define FLEXT_EXP_1 
#define FLEXT_EXP(LIB) FLEXT_EXP_##LIB

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_JMAX
#define FLEXT_OBJ_SETUP_0(NEW_CLASS,DSP)
#elif FLEXT_SYS == FLEXT_SYS_MAX
#define FLEXT_OBJ_SETUP_0(NEW_CLASS,DSP) extern "C" FLEXT_EXT int main() { FLEXT_STPF(NEW_CLASS,DSP)(); return 0; }
#else
#error not implemented
#endif

#define FLEXT_OBJ_SETUP_1(NEW_CLASS,DSP)

#define FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB) FLEXT_OBJ_SETUP_##LIB(NEW_CLASS,DSP)



// ----------------------------------------
// These definitions are used below
// ----------------------------------------

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	// maybe that's not necessary
	#define FLEXTTPN_NULL A_NULL
	#if FLEXT_SYS == FLEXT_SYS_PD 
		#define FLEXTTPN_PTR A_POINTER
	#else
		#define FLEXTTPN_INT A_INT
		#define FLEXTTPN_DEFINT A_DEFINT
	#endif
	#define FLEXTTPN_FLOAT A_FLOAT
	#define FLEXTTPN_DEFFLOAT A_DEFFLOAT
	#define FLEXTTPN_SYM A_SYMBOL
	#define FLEXTTPN_DEFSYM A_DEFSYMBOL
	#define FLEXTTPN_VAR A_GIMME
#else
	#define FLEXTTPN_NULL 0
	#define FLEXTTPN_PTR 1
	#define FLEXTTPN_INT 2
	#define FLEXTTPN_FLOAT 3
	#define FLEXTTPN_SYM 4
	#define FLEXTTPN_VAR 5
	#define FLEXTTPN_DEFINT 6
	#define FLEXTTPN_DEFFLOAT 7
	#define FLEXTTPN_DEFSYM 8
#endif

// Shortcuts for PD/Max type arguments
#define FLEXTTYPE_void FLEXTTPN_NULL
#define CALLBTYPE_void void
#define FLEXTTYPE_float FLEXTTPN_FLOAT
#define FLEXTTYPE_float0 FLEXTTPN_DEFFLOAT
#define CALLBTYPE_float float
#define FLEXTTYPE_t_float FLEXTTPN_FLOAT
#define CALLBTYPE_t_float t_float

#if FLEXT_SYS == FLEXT_SYS_PD
#define FLEXTTYPE_int FLEXTTPN_FLOAT
#define FLEXTTYPE_int0 FLEXTTPN_DEFFLOAT
#define CALLBTYPE_int float
#define FLEXTTYPE_bool FLEXTTPN_FLOAT
#define FLEXTTYPE_bool0 FLEXTTPN_DEFFLOAT
#define CALLBTYPE_bool float
#elif FLEXT_SYS == FLEXT_SYS_MAX || FLEXT_SYS == FLEXT_SYS_JMAX
#define FLEXTTYPE_int FLEXTTPN_INT
#define FLEXTTYPE_int0 FLEXTTPN_DEFINT
#define CALLBTYPE_int int
#define FLEXTTYPE_bool FLEXTTPN_INT
#define FLEXTTYPE_bool0 FLEXTTPN_DEFINT
#define CALLBTYPE_bool int
#else
#error
#endif

#define FLEXTTYPE_t_symptr FLEXTTPN_SYM
#define FLEXTTYPE_t_symptr0 FLEXTTPN_DEFSYM
#define CALLBTYPE_t_symptr t_symptr
#define FLEXTTYPE_t_symtype FLEXTTYPE_t_symptr
#define FLEXTTYPE_t_symtype0 FLEXTTYPE_t_symptr0
#define CALLBTYPE_t_symtype t_symptr
#define FLEXTTYPE_t_ptrtype FLEXTTPN_PTR
#define CALLBTYPE_t_ptrtype t_ptrtype

#define FLEXTTP(TP) FLEXTTYPE_ ## TP
#define CALLBTP(TP) CALLBTYPE_ ## TP


#define ARGMEMBER_bool(a) GetBool(a)
#define ARGMEMBER_bool0(a) ARGMEMBER_bool(a)
#define ARGMEMBER_int(a) GetInt(a)
#define ARGMEMBER_int0(a) ARGMEMBER_int(a)
#define ARGMEMBER_float(a) GetFloat(a)
#define ARGMEMBER_float0(a) ARGMEMBER_float(a)
#define ARGMEMBER_t_symptr(a) GetSymbol(a)
#define ARGMEMBER_t_symptr0(a) ARGMEMBER_t_symptr(a)
#define ARGMEMBER_t_symtype(a) ARGMEMBER_t_symptr(a)
#define ARGMEMBER_t_symtype0(a) ARGMEMBER_t_symptr0(a)
#define ARGCAST(a,tp) ARGMEMBER_##tp(a)


#define REAL_NEW(NAME,NEW_CLASS,DSP,LIB) \
flext_obj *NEW_CLASS::__init__(int ,t_atom *) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS;                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,&NEW_CLASS::__free__,FLEXTTPN_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_V(NAME,NEW_CLASS,DSP,LIB) \
flext_obj *NEW_CLASS::__init__(int argc,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(argc,argv);                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,&NEW_CLASS::__free__,FLEXTTPN_VAR,FLEXTTPN_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_1(NAME,NEW_CLASS,DSP,LIB, TYPE1) \
flext_obj *NEW_CLASS::__init__(int,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTPN_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_2(NAME,NEW_CLASS,DSP,LIB, TYPE1,TYPE2) \
flext_obj *NEW_CLASS::__init__(int,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1),ARGCAST(argv[1],TYPE2));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTPN_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_3(NAME,NEW_CLASS,DSP,LIB, TYPE1, TYPE2, TYPE3) \
flext_obj *NEW_CLASS::__init__(int,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1),ARGCAST(argv[1],TYPE2),ARGCAST(argv[2],TYPE3));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTP(TYPE3),FLEXTTPN_NULL); \
} \
FLEXT_OBJ_SETUP(NEW_CLASS,DSP,LIB)

#define REAL_NEW_4(NAME,NEW_CLASS,DSP,LIB, TYPE1,TYPE2, TYPE3, TYPE4) \
flext_obj *NEW_CLASS::__init__(int,t_atom *argv) \
{     	    	    	    	    	    	    	    	\
    return new NEW_CLASS(ARGCAST(argv[0],TYPE1),ARGCAST(argv[1],TYPE2),ARGCAST(argv[2],TYPE3),ARGCAST(argv[3],TYPE4));                     \
}   	    	    	    	    	    	    	    	\
FLEXT_EXP(LIB) void FLEXT_STPF(NEW_CLASS,DSP)()   \
{   	    	    	    	    	    	    	    	\
    flext_obj::obj_add(LIB,DSP,FLEXT_ATTRIBUTES,#NEW_CLASS,NAME,NEW_CLASS::__setup__,NEW_CLASS::__init__,NEW_CLASS::__free__,FLEXTTP(TYPE1),FLEXTTP(TYPE2),FLEXTTP(TYPE3),FLEXTTP(TYPE4),FLEXTTPN_NULL); \
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




