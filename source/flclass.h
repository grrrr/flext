/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flclass.h
	\brief User accessible flext base classes
    
*/

#ifndef __FLCLASS_H
#define __FLCLASS_H

// include the header file declaring the base classes
#include "flbase.h"
#include "flsupport.h"


#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#ifdef __BORLANDC__
#pragma warn -8008 // Condition is always false
#pragma warn -8057 // Parameter is never used
#pragma warn -8066 // Unreachable code
#endif


// === flext_base ==================================================

/*! \brief Flext message only base object

    This is the base class from which typical external objects derive.
    DSP objects should use the flext_dsp class which inherits from flext_base and
    provides the necessary functionality.

    For a valid external object class you would also need FLEXT_HEADER, also if it's only
    a base class without instantiated objects again.
    To make an instance of an object class you would typically use FLEXT_NEW or 
    its companions.

    See the flext_obj class for additional information.
*/


class FLEXT_SHARE FLEXT_CLASSDEF(flext_base);
typedef class FLEXT_SHARE FLEXT_CLASSDEF(flext_base) flext_base;

class FLEXT_SHARE FLEXT_CLASSDEF(flext_base): 
	public flext_obj
{
	FLEXT_HEADER_S(FLEXT_CLASSDEF(flext_base),flext_obj,Setup)
	
	friend class FLEXT_SHARE FLEXT_CLASSDEF(flext_obj);

	/*!	\defgroup FLEXT_CLASS Flext base class
		@{ 
	*/
public:

	/*!	\defgroup FLEXT_C_BASE Basic class functionality
		@{ 
	*/

// ---  compatibility mode ----------------------------------------

	/*! \brief Cross-platform compatibility flag.
		If set flext allows only operations valid for all platforms.
		Set to true by default!
	*/
	static bool compatibility;  

//!		@} FLEXT_C_BASE

// --- inheritable virtual methods --------------------------------

	/*!	\defgroup FLEXT_C_VIRTUAL Virtual base class functions

		@{ 
	*/

	//! called on "help" message: should post some text
	virtual void m_help();
	
	//! called on patcher load (not on mere object creation!)
	virtual void m_loadbang();

	//! quickhelp for inlets/outlets (gets called in Max/MSP only)
	virtual void m_assist(long /*msg*/,long /*arg*/,char * /*s*/);

	/*!	\brief Called for every incoming message.
		All method handling is done in there
		\return True if a handler was found and called
	*/
	virtual bool m_methodmain(int inlet,const t_symbol *s,int argc,const t_atom *argv);

	/*! \brief Called for every unhandled message (by m_methodmain)
	*/
	virtual bool m_method_(int inlet,const t_symbol *s,int argc,const t_atom *argv);

//!		@} FLEXT_C_VIRTUAL


// --- inlet/outlet stuff -----------------------------------------	

	/*!	\defgroup FLEXT_C_INOUT Flext in-/outlet functions
		\note These must be called in the class' constructor
		\note All (also default) inlets must be defined
		@{ 
	*/

	/*!	\defgroup FLEXT_C_IO_ADD Announce in-/outlet functions
		@{ 
	*/

	// argument m specifies multiple inlet/outlet count
	
//	void AddInDef() { AddInlet(xlet::tp_def,1); }

	/*! \brief Add inlet(s) for anythings
		\remark That's the one to choose for the left-most (first) inlet unless it's a signal inlet.
	*/
	void AddInAnything(int m = 1) { AddInlet(xlet::tp_any,m); } 
	/*! \brief Add inlet(s) for anythings (with description)
		\remark That's the one to choose for the left-most (first) inlet unless it's a signal inlet.
	*/
	void AddInAnything(const char *desc,int m = 1) { AddInlet(xlet::tp_any,m,desc); } 
	//! Add inlet(s) for floats
	void AddInFloat(int m = 1) { AddInlet(xlet::tp_float,m); }
	//! Add inlet(s) for floats (with description)
	void AddInFloat(const char *desc,int m = 1) { AddInlet(xlet::tp_float,m,desc); }
	//! Add inlet(s) for integers
	void AddInInt(int m = 1) { AddInlet(xlet::tp_int,m); }
	//! Add inlet(s) for integers (with description)
	void AddInInt(const char *desc,int m = 1) { AddInlet(xlet::tp_int,m,desc); }
	//! Add inlet(s) for symbols
	void AddInSymbol(int m = 1) { AddInlet(xlet::tp_sym,m); }
	//! Add inlet(s) for symbol (with description)
	void AddInSymbol(const char *desc,int m = 1) { AddInlet(xlet::tp_sym,m,desc); }
	//! Add inlet(s) for bang
	void AddInBang(int m = 1) { AddInlet(xlet::tp_sym,m); }
	//! Add inlet(s) for bangs (with description)
	void AddInBang(const char *desc,int m = 1) { AddInlet(xlet::tp_sym,m,desc); }
	//! Add inlet(s) for lists
	void AddInList(int m = 1) { AddInlet(xlet::tp_list,m); }  
	//! Add inlet(s) for lists (with description)
	void AddInList(const char *desc,int m = 1) { AddInlet(xlet::tp_list,m,desc); }  
	
	//! Add outlet(s) for anythings
	void AddOutAnything(int m = 1) { AddOutlet(xlet::tp_any,m); }
	//! Add outlet(s) for anythings (with description)
	void AddOutAnything(const char *desc,int m = 1) { AddOutlet(xlet::tp_any,m,desc); }
	//! Add outlet(s) for floats
	void AddOutFloat(int m = 1) { AddOutlet(xlet::tp_float,m); }
	//! Add outlet(s) for floats (with description)
	void AddOutFloat(const char *desc,int m = 1) { AddOutlet(xlet::tp_float,m,desc); }
	//! Add outlet(s) for integers
	void AddOutInt(int m = 1) { AddOutlet(xlet::tp_int,m); }
	//! Add outlet(s) for integers (with description)
	void AddOutInt(const char *desc,int m = 1) { AddOutlet(xlet::tp_int,m,desc); }
	//! Add outlet(s) for symbols
	void AddOutSymbol(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	//! Add outlet(s) for symbols (with description)
	void AddOutSymbol(const char *desc,int m = 1) { AddOutlet(xlet::tp_sym,m,desc); }
	//! Add outlet(s) for bangs
	void AddOutBang(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	//! Add outlet(s) for bangs (with description)
	void AddOutBang(const char *desc,int m = 1) { AddOutlet(xlet::tp_sym,m,desc); }
	//! Add outlet(s) for lists
	void AddOutList(int m = 1) { AddOutlet(xlet::tp_list,m); }
	//! Add outlet(s) for lists (with description)
	void AddOutList(const char *desc,int m = 1) { AddOutlet(xlet::tp_list,m,desc); }

	//! \deprecated inlets and outlets are now set up automatically
	bool SetupInOut() { return true; }

	//!	@} FLEXT_C_IO_ADD 

	/*!	\defgroup FLEXT_C_IO_MISC Miscellanous in-/outlet functionality
		@{ 
	*/

	//! Get number of inlets
	int CntIn() const { return incnt; }
	//! Get number of outlets
	int CntOut() const { return outcnt; }
	//! Get number of signal inlets
	int CntInSig() const { return insigs; }
	//! Get number of signal outlets
	int CntOutSig() const { return outsigs; }

	//! Retrieve currently processed message tag (NULL if no message processing)
	const t_symbol *thisTag() const { return curtag; }

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	class outlet;

	//! Get pointer to outlet (not in the constructor!)
	outlet *GetOut(int ix) const { return outlets[ix]; }
#endif
	int GetOutAttr() const { return procattr?CntOut():0; }

	//! @} FLEXT_C_IO_MISC

	/*!	\defgroup FLEXT_C_IO_OUT Output data to inlets/outlets
		@{ 
	*/

	// output messages 

	//! Output bang (index n starts with 0)
	void ToOutBang(int n) const;

	//! Output float (index n starts with 0)
	void ToOutFloat(int n,float f) const;

	//! Output integer (index n starts with 0)
	void ToOutInt(int n,int f) const;
	
	//! Output boolean (index n starts with 0)
    void ToOutBool(int n,bool f) const { ToOutInt(n,f?1:0); }
	
	//! Output symbol (index n starts with 0)
	void ToOutSymbol(int n,const t_symbol *s) const;
	//! Output string aka symbol (index n starts with 0)
	void ToOutString(int n,const char *s) const { ToOutSymbol(n,MakeSymbol(s)); }

	//! Output list (index n starts with 0)
	void ToOutList(int n,int argc,const t_atom *argv) const;
	//! Output list (index n starts with 0)
	void ToOutList(int n,const AtomList &list) const { ToOutList(n,list.Count(),list.Atoms()); }
	
	//! Output anything (index n starts with 0)
	void ToOutAnything(int n,const t_symbol *s,int argc,const t_atom *argv) const;
	//! Output anything (index n starts with 0)
	void ToOutAnything(int n,const AtomAnything &any) const { ToOutAnything(n,any.Header(),any.Count(),any.Atoms()); }
	
	//! @} FLEXT_C_IO_OUT

	/*!	\defgroup FLEXT_C_IO_QUEUE Low-priority output of data to inlets/outlets
		@{ 
	*/

	//! Output bang (index n starts with 0)
	void ToQueueBang(int n) const;

	//! Output float (index n starts with 0)
	void ToQueueFloat(int n,float f) const;

	//! Output integer (index n starts with 0)
	void ToQueueInt(int n,int f) const;

	//! Output boolean (index n starts with 0)
	void ToQueueBool(int n,bool f) const { ToQueueInt(n,f?1:0); }

	//! Output symbol (index n starts with 0)
	void ToQueueSymbol(int n,const t_symbol *s) const;
	//! Output string aka symbol (to appointed outlet)
	void ToQueueString(int n,const char *s) const { ToQueueSymbol(n,MakeSymbol(s)); }

	//! Output list (index n starts with 0)
	void ToQueueList(int n,int argc,const t_atom *argv) const; 
	//! Output list (index n starts with 0)
	void ToQueueList(int n,const AtomList &list) const  { ToQueueList(n,list.Count(),list.Atoms()); }

	//! Output anything (index n starts with 0)
	void ToQueueAnything(int n,const t_symbol *s,int argc,const t_atom *argv)  const;
	//! Output anything (index n starts with 0)
	void ToQueueAnything(int n,const AtomAnything &any) const  { ToQueueAnything(n,any.Header(),any.Count(),any.Atoms()); }


	//! Send bang to self (inlet n)
	void ToSelfBang(int n) const; 

	//! Send float to self (inlet n)
	void ToSelfFloat(int n,float f) const; 

	//! Send integer to self (inlet n)
	void ToSelfInt(int n,int f) const; 

	//! Send boolean to self (inlet n)
	void ToSelfBool(int n,bool f) const { ToSelfInt(n,f?1:0); }

	//! Send symbol to self (inlet n)
	void ToSelfSymbol(int n,const t_symbol *s) const; 
	//! Send string aka symbol to self (inlet 0)
	void ToSelfString(int n,const char *s) const { ToSelfSymbol(n,MakeSymbol(s)); }

	//! Send list to self (inlet n)
	void ToSelfList(int n,int argc,const t_atom *argv) const; 
	//! Send list to self (inlet n)
	void ToSelfList(int n,const AtomList &list) const  { ToSelfList(n,list.Count(),list.Atoms()); }

	//! Send anything to self (inlet n)
	void ToSelfAnything(int n,const t_symbol *s,int argc,const t_atom *argv)  const; 
	//! Send anything to self (inlet n)
	void ToSelfAnything(int n,const AtomAnything &any) const { ToSelfAnything(n,any.Header(),any.Count(),any.Atoms()); }

//!		@} FLEXT_C_IO_QUEUE

//!	@} FLEXT_C_INOUT


// --- message handling -------------------------------------------

	enum metharg {
		a_null = 0,
		a_float,a_int,
		a_symbol,a_pointer,
		a_list,a_any, // (t_symbol *) / int / t_atom *
		a_LIST,a_ANY // AtomList, AtomAnything
	};

	typedef bool (*methfun)(flext_base *c);

	/*!	\defgroup FLEXT_C_ADDMETHOD Method handling (object scope)
		\internal
		@{ 
	*/

	void AddMethodDef(int inlet,const char *tag = NULL); // call virtual function for tag && inlet

	void AddMethod(int inlet,bool (*m)(flext_base *,int,t_atom *)) { AddMethod(ThMeths(),inlet,"list",(methfun)m,a_list,a_null); }
	void AddMethod(int inlet,bool (*m)(flext_base *,int,const t_atom *)) { AddMethod(ThMeths(),inlet,"list",(methfun)m,a_list,a_null); }
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_null); }  // pure method
	void AddMethod(int inlet,bool (*m)(flext_base *,t_symbol *,int,t_atom *)) { AddMethod(ThMeths(),inlet,"anything",(methfun)m,a_any,a_null); } // anything
	void AddMethod(int inlet,bool (*m)(flext_base *,const t_symbol *,int,const t_atom *)) { AddMethod(ThMeths(),inlet,"anything",(methfun)m,a_any,a_null); } // anything
	void AddMethod(int inlet,bool (*m)(flext_base *,t_symbol *&)) { AddMethod(ThMeths(),inlet,"symbol",(methfun)m,a_symbol,a_null); } // single symbol
	void AddMethod(int inlet,bool (*m)(flext_base *,const t_symbol *&)) { AddMethod(ThMeths(),inlet,"symbol",(methfun)m,a_symbol,a_null); } // single symbol
	void AddMethod(int inlet,bool (*m)(flext_base *,float &)) { AddMethod(ThMeths(),inlet,"float",(methfun)m,a_float,a_null); }  // single float
	void AddMethod(int inlet,bool (*m)(flext_base *,float &,float &)) { AddMethod(ThMeths(),inlet,"list",(methfun)m,a_float,a_float,a_null); } // list of 2 floats
	void AddMethod(int inlet,bool (*m)(flext_base *,float &,float &,float &)) { AddMethod(ThMeths(),inlet,"list",(methfun)m,a_float,a_float,a_float,a_null); } // list of 3 floats
#if FLEXT_SYS == FLEXT_SYS_PD
	void AddMethod(int inlet,bool (*m)(flext_base *,int &)) { AddMethod(ThMeths(),inlet,"float",(methfun)m,a_int,a_null); }  // single float
#elif FLEXT_SYS == FLEXT_SYS_MAX || FLEXT_SYS == FLEXT_SYS_JMAX
	void AddMethod(int inlet,bool (*m)(flext_base *,int &)) { AddMethod(ThMeths(),inlet,"int",(methfun)m,a_int,a_null); }  // single float
#else
#error
#endif
	void AddMethod(int inlet,bool (*m)(flext_base *,int &,int &)) { AddMethod(ThMeths(),inlet,"list",(methfun)m,a_int,a_int,a_null); } // list of 2 floats
	void AddMethod(int inlet,bool (*m)(flext_base *,int &,int &,int &)) { AddMethod(ThMeths(),inlet,"list",(methfun)m,a_int,a_int,a_int,a_null); } // list of 3 floats
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,int,t_atom *)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_list,a_null); } // method+gimme
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,int,const t_atom *)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_list,a_null); } // method+gimme
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,t_symbol *,int,t_atom *)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_any,a_null); } // method+gimme 
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,const t_symbol *,int,const t_atom *)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_any,a_null); } // method+gimme 
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,t_symbol *&)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,const t_symbol *&)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,float &)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_float,a_null); }  // method+float
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,int &)) { AddMethod(ThMeths(),inlet,tag,(methfun)m,a_int,a_null); } // method+int

	//! Set Max/MSP style of distributing list elements over (message) inlets
	void SetDist(bool d = true) { distmsgs = d; }

//!		@} FLEXT_C_ADDMETHOD

	/*!	\defgroup FLEXT_C_CADDMETHOD Method handling (class scope)
		\internal
		@{ 
	*/

	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,int,t_atom *)) { AddMethod(ClMeths(c),inlet,"list",(methfun)m,a_list,a_null); }
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,int,const t_atom *)) { AddMethod(ClMeths(c),inlet,"list",(methfun)m,a_list,a_null); }
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_null); }  // pure method
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,t_symbol *,int,t_atom *)) { AddMethod(ClMeths(c),inlet,"anything",(methfun)m,a_any,a_null); } // anything
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,const t_symbol *,int,const t_atom *)) { AddMethod(ClMeths(c),inlet,"anything",(methfun)m,a_any,a_null); } // anything
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,t_symbol *&)) { AddMethod(ClMeths(c),inlet,"symbol",(methfun)m,a_symbol,a_null); } // single symbol
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,const t_symbol *&)) { AddMethod(ClMeths(c),inlet,"symbol",(methfun)m,a_symbol,a_null); } // single symbol
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,float &)) { AddMethod(ClMeths(c),inlet,"float",(methfun)m,a_float,a_null); }  // single float
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,float &,float &)) { AddMethod(ClMeths(c),inlet,"list",(methfun)m,a_float,a_float,a_null); } // list of 2 floats
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,float &,float &,float &)) { AddMethod(ClMeths(c),inlet,"list",(methfun)m,a_float,a_float,a_float,a_null); } // list of 3 floats
#if FLEXT_SYS == FLEXT_SYS_PD
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,int &)) { AddMethod(ClMeths(c),inlet,"float",(methfun)m,a_int,a_null); }  // single integer
#elif FLEXT_SYS == FLEXT_SYS_MAX || FLEXT_SYS == FLEXT_SYS_JMAX
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,int &)) { AddMethod(ClMeths(c),inlet,"int",(methfun)m,a_int,a_null); }  // single integer
#else
#error
#endif
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,int &,int &)) { AddMethod(ClMeths(c),inlet,"list",(methfun)m,a_int,a_int,a_null); } // list of 2 floats
	static void AddMethod(t_classid c,int inlet,bool (*m)(flext_base *,int &,int &,int &)) { AddMethod(ClMeths(c),inlet,"list",(methfun)m,a_int,a_int,a_int,a_null); } // list of 3 floats
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,int,t_atom *)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_list,a_null); } // method+gimme
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,int,const t_atom *)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_list,a_null); } // method+gimme
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,t_symbol *,int,t_atom *)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_any,a_null); } // method+gimme 
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,const t_symbol *,int,const t_atom *)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_any,a_null); } // method+gimme 
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,t_symbol *&)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,const t_symbol *&)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,float &)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_float,a_null); }  // method+float
	static void AddMethod(t_classid c,int inlet,const char *tag,bool (*m)(flext_base *,int &)) { AddMethod(ClMeths(c),inlet,tag,(methfun)m,a_int,a_null); } // method+int

//!		@} FLEXT_C_CADDMETHOD

// --- bind/unbind ---------------------------------------

	/*!	\defgroup FLEXT_C_BIND Methods for binding a flext class to a symbol

		@{ 
	*/

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX 
	//! Bind object to a symbol
	bool Bind(const t_symbol *sym);
	//! Unbind object from a symbol
	bool Unbind(const t_symbol *sym);
#endif

#if FLEXT_SYS == FLEXT_SYS_JMAX
	//! Bind object to a symbol (as string)
	bool Bind(const char *sym); // ** TODO **  
	//! Unbind object from a symbol (as string)
	bool Unbind(const char *sym);  // ** TODO **   
#else
	//! Bind object to a symbol (as string)
	bool Bind(const char *sym) { return Bind(MakeSymbol(sym)); }  
	//! Unbind object from a symbol (as string)
	bool Unbind(const char *sym) { return Unbind(MakeSymbol(sym)); }  
#endif

    /*! \brief Bind a method to a symbol
        \param sym Symbol to bind to
        \param meth Function to bind
        \param data User data that is passed to the function
        \return true on success
    */
	bool BindMethod(const t_symbol *sym,bool (*meth)(flext_base *obj,t_symbol *sym,int argc,t_atom *argv,void *data),void *data = NULL);
    /*! \brief Unbind a method from a symbol
        \param sym Symbol to unbind from (if NULL... unbind all functions)
        \param meth Method to unbind (if NULL ... unbind all functions bound to symbol)
        \param data returns data pointer specified with BindMethod
        \return true on success
    */
	bool UnbindMethod(const t_symbol *sym,bool (*meth)(flext_base *obj,t_symbol *sym,int argc,t_atom *argv,void *data) = NULL,void **data = NULL);

    //! \brief Bind a method to a symbol (as string)
    bool BindMethod(const char *sym,bool (*meth)(flext_base *obj,t_symbol *sym,int argc,t_atom *argv,void *data),void *data = NULL) { return BindMethod(MakeSymbol(sym),meth,data); }
    //! \brief Unbind a method from a symbol (as string)
    bool UnbindMethod(const char *sym,bool (*meth)(flext_base *obj,t_symbol *sym,int argc,t_atom *argv,void *data) = NULL,void **data = NULL) { return UnbindMethod(MakeSymbol(sym),meth,data); }

	/*! Unbind all symbol bindings
		\note Memory associated to data pointers passed by BindMethod will not be freed!
	*/
	bool UnbindAll();

//!		@} FLEXT_C_BIND

// --- thread stuff -----------------------------------------------

#ifdef FLEXT_THREADS
	/*!	\defgroup FLEXT_C_THREAD Thread handling 

		@{ 
	*/

	//! Start a thread for this object
	bool StartThread(void (*meth)(thr_params *p),thr_params *p,const char *) { p->cl = this; return flext::LaunchThread(meth,p); }

	//! Terminate all threads of this object
	bool StopThreads();

	//! Check if current thread should terminate
	bool ShouldExit() const;

#endif // FLEXT_THREADS

//!		@}  FLEXT_C_THREAD

// xxx internal stuff xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

protected:

	FLEXT_CLASSDEF(flext_base)();
	virtual ~FLEXT_CLASSDEF(flext_base)();

	/*! \brief Set up inlets and outlets, method and attribute lists
	*/
	virtual bool Init();
	
	//! \brief This represents either an inlet or outlet
	struct xlet {	
		enum type {
			tp_none = 0,
			tp_float,tp_int,tp_sym,tp_list,tp_any,
			tp_LIST,tp_ANY, // use AtomList and AtomAnything
			tp_sig
		};

		xlet(type t,const char *desc = NULL);
		~xlet();
		
		char *desc;
		type tp;
		xlet *nxt;
	};

	/*!	\defgroup FLEXT_C_ATTR Attribute handling methods (object scope)
		@{ 
	*/

	void AddAttrib(const char *attr,bool (*get)(flext_base *,float &),bool (*set)(flext_base *,float &)) { AddAttrib(attr,a_float,(methfun)get,(methfun)set); }
	void AddAttrib(const char *attr,bool (*get)(flext_base *,int &),bool (*set)(flext_base *,int &)) { AddAttrib(attr,a_int,(methfun)get,(methfun)set); }
	void AddAttrib(const char *attr,bool (*get)(flext_base *,const t_symbol *&),bool (*set)(flext_base *,const t_symbol *&)) { AddAttrib(attr,a_symbol,(methfun)get,(methfun)set); }
	void AddAttrib(const char *attr,bool (*get)(flext_base *,t_symptr &),bool (*set)(flext_base *,t_symptr &)) { AddAttrib(attr,a_symbol,(methfun)get,(methfun)set); }
	void AddAttrib(const char *attr,bool (*get)(flext_base *,AtomList *&),bool (*set)(flext_base *,AtomList *&)) { AddAttrib(attr,a_LIST,(methfun)get,(methfun)set); }
	void AddAttrib(const char *attr,bool (*get)(flext_base *,AtomAnything *&),bool (*set)(flext_base *,AtomAnything *&)) { AddAttrib(attr,a_ANY,(methfun)get,(methfun)set); }

//!		@} FLEXT_C_ATTR

	/*!	\defgroup FLEXT_C_CATTR Attribute handling methods (class scope)
		@{ 
	*/

	static void AddAttrib(t_classid c,const char *attr,bool (*get)(flext_base *,float &),bool (*set)(flext_base *,float &)) { AddAttrib(c,attr,a_float,(methfun)get,(methfun)set); }
	static void AddAttrib(t_classid c,const char *attr,bool (*get)(flext_base *,int &),bool (*set)(flext_base *,int &)) { AddAttrib(c,attr,a_int,(methfun)get,(methfun)set); }
	static void AddAttrib(t_classid c,const char *attr,bool (*get)(flext_base *,const t_symbol *&),bool (*set)(flext_base *,const t_symbol *&)) { AddAttrib(c,attr,a_symbol,(methfun)get,(methfun)set); }
	static void AddAttrib(t_classid c,const char *attr,bool (*get)(flext_base *,t_symptr &),bool (*set)(flext_base *,t_symptr &)) { AddAttrib(c,attr,a_symbol,(methfun)get,(methfun)set); }
	static void AddAttrib(t_classid c,const char *attr,bool (*get)(flext_base *,AtomList *&),bool (*set)(flext_base *,AtomList *&)) { AddAttrib(c,attr,a_LIST,(methfun)get,(methfun)set); }
	static void AddAttrib(t_classid c,const char *attr,bool (*get)(flext_base *,AtomAnything *&),bool (*set)(flext_base *,AtomAnything *&)) { AddAttrib(c,attr,a_ANY,(methfun)get,(methfun)set); }

//!		@} FLEXT_C_CATTR

	//! Dump an attribute to the attribute outlet
	bool DumpAttrib(const t_symbol *attr) const;
    //! Dump an attribute to the attribute outlet
	bool DumpAttrib(const char *attr) const { return DumpAttrib(MakeSymbol(attr)); }

	/*!	\addtogroup FLEXT_C_INOUT 
		@{ 
	*/

	//! \brief get a code for a list of inlets or outlets
	unsigned long XletCode(xlet::type tp = xlet::tp_none,...); // end list with 0 (= tp_none) !!

	/*! \brief Add some inlets by a special code representing the types
		\remark use XletCode function to get code value
	*/
	void AddInlets(unsigned long code); 

	//! \brief Add one or more inlet(s)
	void AddInlet(xlet::type tp,int mult = 1,const char *desc = NULL) { AddXlet(tp,mult,desc,inlist); }

	/*! \brief Add some inlets by a special code representing the types
		\remark use XletCode function to get code value
	*/
	void AddOutlets(unsigned long code); 

	//! \brief Add one or more outlet(s)
	void AddOutlet(xlet::type tp,int mult = 1,const char *desc = NULL) { AddXlet(tp,mult,desc,outlist); }

	//! \brief Set the description of an indexed inlet
	void DescInlet(int ix,const char *desc) { DescXlet(ix,desc,inlist); }

	//! \brief Set the description of an indexed outlet
	void DescOutlet(int ix,const char *desc) { DescXlet(ix,desc,outlist); }

//!		@} FLEXT_C_INOUT

// method handling

	public:

	class attritem;

	class item {
	public:
		item(const t_symbol *t,int inl,attritem *a);
		virtual ~item();
		
		bool IsAttr() const { return attr != NULL; }

		const t_symbol *tag;
		int inlet;
		attritem *attr;
		item *nxt;
	};

    //! This class holds hashed item entries
	class itemarr {
	public:
		itemarr();
		~itemarr();

        //! Add an entry
		void Add(item *it);
        //! Remove an entry
		bool Remove(item *it);
        //! Find an entry in the item array
		item *Find(const t_symbol *tag,int inlet = 0) const;

        //! Create hash table out of the preliminary linked lists
		void Finalize();
		
        //! Query whether the array has been finalized
		bool Ready() const { return bits >= 0; }
        //! Number of items in the array
		int Count() const { return cnt; }
        //! Number of array slots (0 if not finalized)
		int Size() const { return bits?1<<bits:0; }

        //! Get an array slot
		item *Item(int ix) { return arr[ix]; }
	
	protected:		
        //! Calculate a hash value
		static int Hash(const t_symbol *,int inlet,int bits);
	
		item **arr;
		int cnt,bits;
	};

	//! \brief This represents an item of the method list
	class methitem:
		public item { 
	public:
		methitem(int inlet,const t_symbol *tg,attritem *conn = NULL);
		virtual ~methitem();
		
		void SetArgs(methfun fun,int argc,metharg *args);

		int argc;
		metharg *args;
		methfun fun;
	};
	
	//! \brief This represents an item of the attribute list
	class attritem:
		public item { 
	public:
		attritem(const t_symbol *tag,metharg tp,methfun fun,int flags);
		virtual ~attritem();

		enum { 
			afl_getset = 0x01, afl_get = 0x00, afl_set = 0x01,
			afl_bothexist = 0x02,
			afl_save = 0x04
		};

		bool IsGet() const { return (flags&afl_getset) == afl_get; }
		bool IsSet() const { return (flags&afl_getset) == afl_set; }
		bool BothExist() const { return (flags&afl_bothexist) != 0; }

		int flags;
		metharg argtp;
		methfun fun;
	};

	// these outlet functions don't check for thread but send directly to the real-time system
	void ToSysBang(int n) const; 
	void ToSysFloat(int n,float f) const; 
	void ToSysInt(int n,int f) const; 
	void ToSysBool(int n,bool f) const { ToSysInt(n,f?1:0); }
	void ToSysSymbol(int n,const t_symbol *s) const; 
	void ToSysList(int n,int argc,const t_atom *argv) const;
	void ToSysAnything(int n,const t_symbol *s,int argc,const t_atom *argv)  const; 

private:
	class pxbnd_object;
public:

	//! \brief This represents an item of the symbol-bound method list
	class binditem:
		public item { 
	public:
		binditem(int inlet,const t_symbol *sym,bool (*f)(flext_base *,t_symbol *s,int,t_atom *,void *),pxbnd_object *px);
		virtual ~binditem();
		
		bool (*fun)(flext_base *,t_symbol *s,int,t_atom *,void *);
        pxbnd_object *px;
	};
	
//!		@} FLEXT_CLASS

	itemarr *ThMeths() { return methhead; }
	static itemarr *ClMeths(t_classid c) { return GetClassArr(c,0); }

	static void AddMethod(itemarr *ma,int inlet,const char *tag,methfun fun,metharg tp,...); 

	itemarr *ThAttrs() { return attrhead; }
	static itemarr *ClAttrs(t_classid c) { return GetClassArr(c,1); }

	static void AddAttrib(itemarr *aa,itemarr *ma,const char *attr,metharg tp,methfun gfun,methfun sfun);
	void AddAttrib(const char *attr,metharg tp,methfun gfun,methfun sfun);
	static void AddAttrib(t_classid c,const char *attr,metharg tp,methfun gfun,methfun sfun);

private:

	static void Setup(t_classid c);

	xlet *inlist,*outlist;
	const t_symbol *curtag;
	int incnt,outcnt,insigs,outsigs;
	bool distmsgs;
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	outlet **outlets;
#endif

	void AddXlet(xlet::type tp,int mult,const char *desc,xlet *&root);	
	void DescXlet(int ix,const char *desc,xlet *&root);	

	union t_any {
		float ft;
		int it;
		t_symbol *st;
#if FLEXT_SYS == FLEXT_SYS_PD
		t_gpointer *pt;
#endif
		void *vt;
	};

	typedef bool (*methfun_V)(flext_base *c,int argc,t_atom *argv);
	typedef bool (*methfun_A)(flext_base *c,const t_symbol *s,int argc,t_atom *argv);
	typedef bool (*methfun_0)(flext_base *c);
	typedef bool (*methfun_1)(flext_base *c,t_any &);
	typedef bool (*methfun_2)(flext_base *c,t_any &,t_any &);
	typedef bool (*methfun_3)(flext_base *c,t_any &,t_any &,t_any &);
	typedef bool (*methfun_4)(flext_base *c,t_any &,t_any &,t_any &,t_any &);
	typedef bool (*methfun_5)(flext_base *c,t_any &,t_any &,t_any &,t_any &,t_any &);

	static itemarr *GetClassArr(t_classid,int ix);

	itemarr *methhead,*clmethhead;
	itemarr *bindhead;
	
	bool CallMeth(const methitem &m,int argc,const t_atom *argv);
	bool FindMeth(int inlet,const t_symbol *s,int argc,const t_atom *argv);
	bool TryMethTag(const methitem *m,int inlet,const t_symbol *t,int argc,const t_atom *argv);
	bool TryMethSym(const methitem *m,int inlet,const t_symbol *t,const t_symbol *s);
	bool TryMethAny(const methitem *m,int inlet,const t_symbol *t,const t_symbol *s,int argc,const t_atom *argv);

	itemarr *attrhead,*clattrhead;

	attritem *FindAttr(const t_symbol *tag,bool get) const;
	int ListAttr(AtomList &a) const;
	int ListMeth(AtomList &a,int inlet = 0) const;

	static int CheckAttrib(int argc,const t_atom *argv);
	bool InitAttrib(int argc,const t_atom *argv);

	bool ListMethods(int inlet = 0) const;
	bool ListAttrib() const;
	bool GetAttrib(attritem *a);
	bool SetAttrib(const t_symbol *s,int argc,const t_atom *argv);
	bool SetAttrib(attritem *a,int argc,const t_atom *argv);

	static bool cb_ListMethods(flext_base *c,int argc,const t_atom *argv);
	static bool cb_ListAttrib(flext_base *c) { return c->ListAttrib(); }

	// queue stuff

//	class qmsg;
//	static qmsg *qhead,*qtail;

	//! Flush messages in the queue
	static void QFlush(flext_base *th = NULL);

	//! Queue worker function
//	static void QWork(bool qlock,bool syslock);

	//! Start message queue
	static void StartQueue();
#if FLEXT_SYS == FLEXT_SYS_JMAX
//	static void QTick(int winlet = 0, fts_symbol_t s = NULL, int ac = 0, const fts_atom_t *at = NULL);
#else // PD or Max
//	static void QTick();
#ifndef FLEXT_QTHR
//	static t_qelem *qclk;
#else
	//! Queue worker thread function
//	static void *QWorker(void *);
	//! Queue worker thread conditional
//	static ThrCond qthrcond;
#endif
#endif

//	static void Queue(qmsg *m);
#ifdef FLEXT_THREADS
//	static ThrMutex qmutex;
#endif


#if FLEXT_SYS == FLEXT_SYS_PD
	// proxy object (for additional inlets) stuff
	static t_class *px_class;

	struct px_object  // no virtual table!
	{ 
		t_object obj;			// MUST reside at memory offset 0
		flext_base *base;
		int index;

		void init(flext_base *b,int ix) { base = b; index = ix; }
		static void px_method(px_object *c,const t_symbol *s,int argc,t_atom *argv);
	};

#elif FLEXT_SYS == FLEXT_SYS_MAX
	typedef object px_object;
	static void cb_px_float(t_class *c,float f);
	static void cb_px_int(t_class *c,int v);
	static void cb_px_bang(t_class *c);

	static void cb_px_in1(t_class *c,int v);
	static void cb_px_in2(t_class *c,int v);
	static void cb_px_in3(t_class *c,int v);
	static void cb_px_in4(t_class *c,int v);
	static void cb_px_in5(t_class *c,int v);
	static void cb_px_in6(t_class *c,int v);
	static void cb_px_in7(t_class *c,int v);
	static void cb_px_in8(t_class *c,int v);
	static void cb_px_in9(t_class *c,int v);
#endif

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	static void cb_px_anything(t_class *c,const t_symbol *s,int argc,t_atom *argv);

	static void cb_px_ft1(t_class *c,float f);
	static void cb_px_ft2(t_class *c,float f);
	static void cb_px_ft3(t_class *c,float f);
	static void cb_px_ft4(t_class *c,float f);
	static void cb_px_ft5(t_class *c,float f);
	static void cb_px_ft6(t_class *c,float f);
	static void cb_px_ft7(t_class *c,float f);
	static void cb_px_ft8(t_class *c,float f);
	static void cb_px_ft9(t_class *c,float f);

	px_object **inlets;
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	static void jmax_proxy(fts_object_t *c, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
#endif

	// --------- symbol-bound proxy

	static t_class *pxbnd_class;

	class pxbnd_object  // no virtual table!
	{ 
    public:
		t_object obj;			// MUST reside at memory offset 0
		flext_base *base;
		binditem *item;
        void *data;

		void init(flext_base *b,binditem *it,void *d) { base = b; item = it; data = d; }
		static void px_method(pxbnd_object *c,const t_symbol *s,int argc,t_atom *argv);
	};
	
    //! create proxy class for symbol binding
	static void SetupBindProxy();

	// ---------

    //! set up inlet proxies
	static void SetProxies(t_class *c);

    //! initialize inlets (according to class or object constructor definitions)
	bool InitInlets();

    //! initialize outlets (according to class or object constructor definitions)
	bool InitOutlets();

	// callback functions

#if FLEXT_SYS == FLEXT_SYS_JMAX
	static void	cb_help(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
#else
	static void cb_help(t_class *c);
	static void cb_loadbang(t_class *c);
#endif

#if FLEXT_SYS == FLEXT_SYS_MAX
	char **indesc,**outdesc;

	static void cb_assist(t_class *c,void *b,long msg,long arg,char *s);
#endif
};

#endif
