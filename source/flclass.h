/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
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

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#ifdef __BORLANDC__
#pragma warn -8008 // Condition is always false
#pragma warn -8057 // Parameter is never used
#pragma warn -8066 // Unreachable code
#endif

// === flext_base ==================================================

//class qmsg;
//class thr_entry;

/*! \brief Flext message only base object
*/

class flext_base:
	public flext_obj
{
	FLEXT_HEADER_S(flext_base,flext_obj,Setup)
	
public:

	/*!	\defgroup FLEXT_C_BASE Flext basic class functionality

		@{ 
	*/

// ---  compatibility mode ----------------------------------------

	/*! \brief Cross-platform compatibility flag.
		If set flext allows only operations valid for all platforms.
		Set to true by default!
	*/
	static bool compatibility;  

//!		@} 


// --- inheritable virtual methods --------------------------------

	/*!	\defgroup FLEXT_C_VIRTUAL Flext virtual base class functions

		@{ 
	*/

	//! called on "help" message: should post some text
	virtual void m_help();
	
	//! called on patcher load (not on mere object creation!)
	virtual void m_loadbang() {}

	//! quickhelp for inlets/outlets (gets calles in MaxMSP only)
	virtual void m_assist(long /*msg*/,long /*arg*/,char * /*s*/) {}

	/*!	\brief Called for every incoming message.
		All method handling is done in there
		\return True if a handler was found and called
	*/
	virtual bool m_methodmain(int inlet,const t_symbol *s,int argc,t_atom *argv);

	//! called for every unhandled message (by m_methodmain)
	virtual bool m_method_(int inlet,const t_symbol *s,int argc,t_atom *argv);

//!		@} 

// --- buffer/array stuff -----------------------------------------	

	/*!	\defgroup FLEXT_C_BUFFER Flext buffer handling

		@{ 
	*/

	//! Class for platform independent buffer handling
	class buffer
	{
	public:
		/*! \brief Construct buffer.
			\param delayed = true: only sets name, needs another Set(NULL) to really initialize the buffer 
			\remark As externals can be created prior to the buffer objects they are pointing to, initialization should be done at loadbang!
		*/
		buffer(const t_symbol *s = NULL,bool delayed = false);
		
		//! Destroy buffer
		~buffer();

		/*! \brief Check if the data is valid
		*/
		bool Ok() const { return sym != NULL && data != NULL; }
		
		/*! \brief Check and update if the buffer has been changed (e.g. resized)
		*/
		bool Update();
		
		/*! \brief Set to specified buffer.
			\param nameonly: if true sets name only, but doesn't look at buffer actually
		*/
		int Set(const t_symbol *s = NULL,bool nameonly = false);
		
		/*! \brief Declare buffer content as dirty.
			\param refr: if true forces immediate graphics refresh
		*/
		void Dirty(bool refr = false);
		
		//! Get symbol of buffer 
		t_symbol *Symbol() const { return const_cast<t_symbol *>(sym); }

		//! Get literal name of buffer 
		const char *Name() const { return sym?sym->s_name:""; }
		
		/*! \brief Get pointer to buffer, channel and frame count.
			\remark Channels are interleaved
		*/
		t_sample *Data() { return data; }

		//! Get channel count
		int Channels() const { return chns; }
		//! Get frame count
		int Frames() const { return frames; }
		//! Set frame count
		void Frames(int fr,bool keep = false);
		
		//! Graphic auto refresh interval
		void SetRefrIntv(float intv);

	protected:
		const t_symbol *sym;
		t_sample *data;
		int chns,frames;
#ifdef PD
		t_garray *arr;
		float interval;
		bool isdirty,ticking;
		t_clock *tick;

	private:
		static void cb_tick(buffer *b);
#endif
	};

//!		@} 

// --- help -------------------------------------------------------	

	/*!	\defgroup FLEXT_C_HELP Flext help/assistance functionality

		@{ 
	*/

	void DefineHelp(const char *ref,const char *dir = NULL);

//!		@} 


// --- inlet/outlet stuff -----------------------------------------	

	/*!	\defgroup FLEXT_C_INOUT Flext in-/outlet functions
		\note These must be called in the class' constructor
		\note All (also default) inlets must be defined

		@{ 
	*/

	// argument m specifies multiple inlet/outlet count
	
//	void AddInDef() { AddInlet(xlet::tp_def,1); }

	/*! \brief Add inlet(s) for anythings
		That's the one to choose for the left-most (first) inlet unless it's a signal inlet.
	*/
	void AddInAnything(int m = 1) { AddInlet(xlet::tp_any,m); } // leftmost or via proxy
	void AddInAnything(const char *desc,int m = 1) { AddInlet(xlet::tp_any,m,desc); } // leftmost or via proxy
	//! Add inlet(s) for floats
	void AddInFloat(int m = 1) { AddInlet(xlet::tp_float,m); }
	void AddInFloat(const char *desc,int m = 1) { AddInlet(xlet::tp_float,m,desc); }
	//! Add inlet(s) for ints
	void AddInInt(int m = 1) { AddInlet(xlet::tp_int,m); }
	void AddInInt(const char *desc,int m = 1) { AddInlet(xlet::tp_int,m,desc); }
	//! Add inlet(s) for symbols
	void AddInSymbol(int m = 1) { AddInlet(xlet::tp_sym,m); }
	void AddInSymbol(const char *desc,int m = 1) { AddInlet(xlet::tp_sym,m,desc); }
	//! Add inlet(s) for bang
	void AddInBang(int m = 1) { AddInlet(xlet::tp_sym,m); }
	void AddInBang(const char *desc,int m = 1) { AddInlet(xlet::tp_sym,m,desc); }
	//! Add inlet(s) for lists
	void AddInList(int m = 1) { AddInlet(xlet::tp_list,m); }  // via proxy
	void AddInList(const char *desc,int m = 1) { AddInlet(xlet::tp_list,m,desc); }  // via proxy
	
	//! Add outlet(s) for anythings
	void AddOutAnything(int m = 1) { AddOutlet(xlet::tp_any,m); }
	void AddOutAnything(const char *desc,int m = 1) { AddOutlet(xlet::tp_any,m,desc); }
	//! Add outlet(s) for floats
	void AddOutFloat(int m = 1) { AddOutlet(xlet::tp_float,m); }
	void AddOutFloat(const char *desc,int m = 1) { AddOutlet(xlet::tp_float,m,desc); }
	//! Add outlet(s) for ints
	void AddOutInt(int m = 1) { AddOutlet(xlet::tp_int,m); }
	void AddOutInt(const char *desc,int m = 1) { AddOutlet(xlet::tp_int,m,desc); }
	//! Add outlet(s) for symbols
	void AddOutSymbol(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	void AddOutSymbol(const char *desc,int m = 1) { AddOutlet(xlet::tp_sym,m,desc); }
	//! Add outlet(s) for bangs
	void AddOutBang(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	void AddOutBang(const char *desc,int m = 1) { AddOutlet(xlet::tp_sym,m,desc); }
	//! Add outlet(s) for lists
	void AddOutList(int m = 1) { AddOutlet(xlet::tp_list,m); }
	void AddOutList(const char *desc,int m = 1) { AddOutlet(xlet::tp_list,m,desc); }
	
	/*! \brief Set up inlets and outlets
		\remark Must be called ONCE to actually set up the defined inlets/outlets.
		\return True on successful creation of all inlets and outlets
	*/
	bool SetupInOut(); 

	//! Get number of inlets
	int CntIn() const { return incnt; }
	//! Get number of outlets
	int CntOut() const { return outcnt; }
	//! Get number of signal inlets
	int CntInSig() const { return insigs; }
	//! Get number of signal outlets
	int CntOutSig() const { return outsigs; }


	class outlet;
	class AtomList;
	class AtomAnything;

	//! Get pointer to outlet (_after_ calling setup_inout()!)
	outlet *GetOut(int ix) { return (outlets && ix < outcnt)?outlets[ix]:NULL; }

	// output messages 

	void ToOutBang(outlet *o); 
	//! Output bang (index n starts with 0)
	void ToOutBang(int n) { outlet *o = GetOut(n); if(o) ToOutBang(o); }

	void ToOutFloat(outlet *o,float f); 
	//! Output float (index n starts with 0)
	void ToOutFloat(int n,float f) { outlet *o = GetOut(n); if(o) ToOutFloat(o,f); }

	void ToOutInt(outlet *o,int f); 
	//! Output int (index n starts with 0)
	void ToOutInt(int n,int f) { outlet *o = GetOut(n); if(o) ToOutInt(o,f); }
	
	void ToOutSymbol(outlet *o,const t_symbol *s); 
	//! Output symbol (index n starts with 0)
	void ToOutSymbol(int n,const t_symbol *s) { outlet *o = GetOut(n); if(o) ToOutSymbol(o,s); }

	void ToOutString(outlet *o,const char *s) { ToOutSymbol(o,gensym(const_cast<char *>(s))); }
	//! Output string (index n starts with 0)
	void ToOutString(int n,const char *s) { outlet *o = GetOut(n); if(o) ToOutString(o,s); }

	void ToOutList(outlet *o,int argc,const t_atom *argv); 
	//! Output list (index n starts with 0)
	void ToOutList(int n,int argc,const t_atom *argv)  { outlet *o = GetOut(n); if(o) ToOutList(o,argc,argv); }
	//! Output list (index n starts with 0)
	void ToOutList(int n,const AtomList &list)  { ToOutList(n,list.Count(),list.Atoms()); }
	
	void ToOutAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv); 
	//! Output anything (index n starts with 0)
	void ToOutAnything(int n,const t_symbol *s,int argc,const t_atom *argv)  { outlet *o = GetOut(n); if(o) ToOutAnything(o,const_cast<t_symbol *>(s),argc,argv); }
	//! Output anything (index n starts with 0)
	void ToOutAnything(int n,const AtomAnything &any)  { ToOutAnything(n,any.Header(),any.Count(),any.Atoms()); }
	
//!		@} 


// --- message handling -------------------------------------------

	enum metharg {
		a_null = 0,
		a_float,a_int, 
		a_symbol,a_pointer,
		a_gimme,a_xgimme
	};

	typedef bool (*methfun)(t_class *c);

	/*!	\defgroup FLEXT_C_ADDMETHOD Flext method handling
		\internal

		@{ 
	*/

	void AddMethodDef(int inlet); // call virtual function for inlet
	void AddMethodDef(int inlet,const char *tag); // call virtual function for tag && inlet
	void AddMethod(int inlet,const char *tag,methfun fun,metharg tp,...); 

	void AddMethod(int inlet,bool (*m)(flext_base *,int,t_atom *)) { AddMethod(inlet,"list",(methfun)m,a_gimme,a_null); }
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *)) { AddMethod(inlet,tag,(methfun)m,a_null); }  // pure method
	void AddMethod(int inlet,bool (*m)(flext_base *,const t_symbol *,int,t_atom *)) { AddMethod(inlet,"anything",(methfun)m,a_xgimme,a_null); } // anything
	void AddMethod(int inlet,bool (*m)(flext_base *,t_symbol *&)) { AddMethod(inlet,"symbol",(methfun)m,a_symbol,a_null); } // single symbol
	void AddMethod(int inlet,bool (*m)(flext_base *,float &)) { AddMethod(inlet,"float",(methfun)m,a_float,a_null); }  // single float
	void AddMethod(int inlet,bool (*m)(flext_base *,float &,float &)) { AddMethod(inlet,"list",(methfun)m,a_float,a_float,a_null); } // list of 2 floats
	void AddMethod(int inlet,bool (*m)(flext_base *,float &,float &,float &)) { AddMethod(inlet,"list",(methfun)m,a_float,a_float,a_float,a_null); } // list of 3 floats
#ifdef PD
	void AddMethod(int inlet,bool (*m)(flext_base *,int &)) { AddMethod(inlet,"float",(methfun)m,a_int,a_null); }  // single float
#else
	void AddMethod(int inlet,bool (*m)(flext_base *,int &)) { AddMethod(inlet,"int",(methfun)m,a_int,a_null); }  // single float
#endif
	void AddMethod(int inlet,bool (*m)(flext_base *,int &,int &)) { AddMethod(inlet,"list",(methfun)m,a_int,a_int,a_null); } // list of 2 floats
	void AddMethod(int inlet,bool (*m)(flext_base *,int &,int &,int &)) { AddMethod(inlet,"list",(methfun)m,a_int,a_int,a_int,a_null); } // list of 3 floats
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,int,t_atom *)) { AddMethod(inlet,tag,(methfun)m,a_gimme,a_null); } // method+gimme
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,const t_symbol *,int,t_atom *)) { AddMethod(inlet,tag,(methfun)m,a_xgimme,a_null); } // method+gimme 
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,t_symbol *&)) { AddMethod(inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,float &)) { AddMethod(inlet,tag,(methfun)m,a_float,a_null); }  // method+float
	void AddMethod(int inlet,const char *tag,bool (*m)(flext_base *,int &)) { AddMethod(inlet,tag,(methfun)m,a_int,a_null); } // method+int

	//! Set MaxMSP style of distributing list elements over (message) inlets
	void SetDist(bool d = true) { distmsgs = d; }

//!		@} 

// --- various symbols --------------------------------------------

	/*!	\defgroup FLEXT_C_SYMBOL Flext atom/symbol handling 

		@{ 
	*/

	//! Symbol constant for "float"
	static const t_symbol *sym_float;
	//! Symbol constant for "symbol"
	static const t_symbol *sym_symbol;
	//! Symbol constant for "bang"
	static const t_symbol *sym_bang;
	//! Symbol constant for "list"
	static const t_symbol *sym_list;
	//! Symbol constant for "anything"
	static const t_symbol *sym_anything;
	/*! \brief Symbol constant for "int"
		\note Only the Max/MSP system has this defined as an internal type
	*/
	static const t_symbol *sym_int;
	//! Symbol constant for "pointer"
	static const t_symbol *sym_pointer;

#ifdef PD
	/*! \brief Symbol constant for "signal"
		\note PD only
	*/
	static const t_symbol *sym_signal;
#endif

	//! Make a symbol from a string
	static const t_symbol *MakeSymbol(const char *s);

	//! Get symbol string
	static const char *GetString(const t_symbol *s) { return s->s_name; }  
	//! Check for symbol and get string
	static const char *GetAString(const t_symbol *s) { return s?s->s_name:""; }  
	

// --- bind/unbind ---------------------------------------

#ifdef PD
	void Bind(const t_symbol *s) { pd_bind(&thisHdr()->ob_pd,const_cast<t_symbol *>(s)); }
	void Bind(const char *c) { Bind(MakeSymbol(c)); }  
	void Unbind(const t_symbol *s) { pd_unbind(&thisHdr()->ob_pd,const_cast<t_symbol *>(s)); }
	void Unbind(const char *c) { Unbind(MakeSymbol(c)); }  
#else
//#pragma message("Bind/Unbind not implemented!");
#endif


	// Low level

	//! Bind object to a symbol
	static void DoUnbind(t_symbol *s,flext_obj *o);
	//! Unbind object from a symbol
	static void DoBind(const t_symbol *s,flext_obj *o);
	//! Get bound object of a symbol
	static t_class **GetBound(const t_symbol *s) { return (t_class **)s->s_thing; }

// --- atom stuff ----------------------------------------
		
	//! Set atom from another atom
	static void SetAtom(t_atom &a,const t_atom &b) { CopyAtom(&a,&b); }

	//! Check whether the atom is a float
	static bool IsFloat(const t_atom &a) { return a.a_type == A_FLOAT; }
	//! Check whether the atom can be represented as a float
	static bool CanbeFloat(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	//! Access the float value (without type check)
	static float GetFloat(const t_atom &a) { return a.a_w.w_float; }
	//! Check for a float and get its value 
	static float GetAFloat(const t_atom &a) { return GetAFlint(a); }
	//! Set the atom to represent a float 
	static void SetFloat(t_atom &a,float v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }

	//! Check whether the atom is a symbol
	static bool IsSymbol(const t_atom &a) { return a.a_type == A_SYMBOL; }
	//! Access the symbol value (without type check)
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_symbol; }
	//! Check for a symbol and get its value 
	static t_symbol *GetASymbol(const t_atom &a) { return IsSymbol(a)?GetSymbol(a):NULL; }  // NULL or empty symbol?
	//! Set the atom to represent a symbol
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_symbol = const_cast<t_symbol *>(s); }

	//! Check whether the atom is a string
	static bool IsString(const t_atom &a) { return IsSymbol(a); }
	//! Access the string value (without type check)
	static const char *GetString(const t_atom &a) { t_symbol *s = GetSymbol(a); return s?GetString(s):NULL; }  
	//! Check for a string and get its value 
	static void GetAString(const t_atom &a,char *buf,int szbuf);
	//! Set the atom to represent a string
	static void SetString(t_atom &a,const char *c) { SetSymbol(a,gensym(const_cast<char *>(c))); }

	//! Check whether the atom can be represented as an integer
	static bool CanbeInt(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	//! Check for an integer and get its value 
	static int GetAInt(const t_atom &a) { return (int)GetAFlint(a); }

	//! Check whether the atom is a float or integer (depending on the system)
	static bool IsFlint(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	//! Check for a float or integer and get its value 
	static float GetAFlint(const t_atom &a) { return IsFloat(a)?GetFloat(a):(IsInt(a)?GetInt(a):0); }
#ifdef PD
	//! Check whether the atom is an integer
	static bool IsInt(const t_atom &) { return false; }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &) { return 0; }
//	static void SetInt(const t_atom &,int) { }

	//! Set the atom to represent a float or integer (depending on the system)
	static void SetFlint(t_atom &a,int v) { a.a_type = A_FLOAT; a.a_w.w_float = (float)v; }

	//! Check whether the atom is a pointer
	static bool IsPointer(const t_atom &a) { return a.a_type == A_POINTER; }
	//! Access the pointer value (without type check)
	static t_gpointer *GetPointer(const t_atom &a) { return a.a_w.w_gpointer; }
	//! Check for a pointer and get its value 
	static t_gpointer *GetAPointer(const t_atom &a) { return IsPointer(a)?GetPointer(a):NULL; }
	//! Set the atom to represent a pointer
	static void SetPointer(t_atom &a,t_gpointer *p) { a.a_type = A_POINTER; a.a_w.w_gpointer = p; }

#elif defined(MAXMSP)
	//! Check whether the atom is an int
	static bool IsInt(const t_atom &a) { return a.a_type == A_INT; }
	//! Access the integer value (without type check)
	static int GetInt(const t_atom &a) { return a.a_w.w_long; }
	//! Set the atom to represent an integer
	static void SetInt(t_atom &a,int v) { a.a_type = A_INT, a.a_w.w_long = v; }

	//! Set the atom to represent a float or integer (depending on the system)
	static void SetFlint(t_atom &a,int v) { a.a_type = A_INT; a.a_w.w_long = v; }

	//! Check whether the atom is a pointer
	static bool IsPointer(const t_atom &) { return false; }
	//! Access the pointer value (without type check)
	static void *GetPointer(const t_atom &) { return NULL; }
	//! Check for a pointer and get its value 
	static void *GetAPointer(const t_atom &) { return NULL; }
//	static void SetPointer(t_atom &,void *) {}
#endif

//!		@} 


	/*!	\defgroup FLEXT_C_THREAD Flext thread handling 

		@{ 
	*/

#ifdef FLEXT_THREADS
// --- thread stuff -----------------------------------------------

	/*! \brief Check if current thread should terminate
		\todo Check for currently running thread
	*/
	bool ShouldExit() const { return shouldexit; }

	//! Check if current thread is the realtime system's thread
	bool IsSystemThread() const { pthread_t cur = pthread_self(); return pthread_equal(cur,thrid) != 0; }

	/*! \brief Yield to other threads
		\remark A call to this is only needed for systems with cooperative multitasking like MacOS<=9
	*/
	void Yield() 
#ifdef MAXMSP
	{}
#else
	;
#endif

	typedef pthread_t thrid_t;

	/*! \brief Increase/Decrease priority of a thread
	*/
	static bool ChangePriority(int dp,thrid_t thr = GetThreadId());

	/*! \brief Get priority of a thread
	*/
	static int GetPriority(thrid_t thr = GetThreadId());

	/*! \brief Set priority of a thread
	*/
	static bool SetPriority(int p,thrid_t thr = GetThreadId());

	/*! \brief Get current thread id
	*/
	static thrid_t GetThreadId();


	/*! \brief Thread mutex
		\sa pthreads documentation
	*/
	class ThrMutex 
	{
	public:
		//! Construct thread mutex
		ThrMutex(): cnt(0) { pthread_mutex_init(&mutex,NULL); }
		//! Destroy thread mutex
		~ThrMutex() { pthread_mutex_destroy(&mutex); }

		//! Lock thread mutex
		int Lock() { cnt = 1; return pthread_mutex_lock(&mutex); }
		//! Try to lock, but don't wait
		int TryLock() { return pthread_mutex_trylock(&mutex); }
		//! Unlock thread mutex
		int Unlock() { cnt = 0; return pthread_mutex_unlock(&mutex); }

		//! Lock thread mutex (increase lock count by one)
		void Push() { if(!cnt++) Lock(); }
		//! Unlock thread mutex if lock count reaches zero
		void Pop() { if(!--cnt) Unlock(); }
	protected:
		pthread_mutex_t mutex;
		int cnt;
	};

	/*! \brief Thread conditional
		\sa pthreads documentation
	*/
	class ThrCond:
		public ThrMutex
	{
	public:
		//! Construct thread conditional
		ThrCond() { pthread_cond_init(&cond,NULL); }
		//! Destroy thread conditional
		~ThrCond() { pthread_cond_destroy(&cond); }

		//! Wait for condition 
		int Wait() { return pthread_cond_wait(&cond,&mutex); }

		/*! \brief Wait for condition (for a certain time)
			\param time Wait time in seconds
		*/
		int TimedWait(float time) 
		{ 
			timespec tm; tm.tv_sec = (long)time; tm.tv_nsec = (long)((time-(long)time)*1.e9);
			return pthread_cond_timedwait(&cond,&mutex,&tm); 
		}

		//! Signal condition
		int Signal() { return pthread_cond_signal(&cond); }
		//! Broadcast condition
		int Broadcast() { return pthread_cond_broadcast(&cond); }
	protected:
		pthread_cond_t cond;
	};
#endif // FLEXT_THREADS

//!		@} 

// --- atom list stuff -------------------------------------------

	/*!	\defgroup FLEXT_C_ATOM Flext atom/list handling

		@{ 
	*/

	//! Class representing a list of atoms
	class AtomList
	{
	public:
		//! Construct list
		AtomList(int argc = 0,const t_atom *argv = NULL);
		//! Construct list
		AtomList(const AtomList &a);
		//! Destroy list
		~AtomList();

		//! Clear list
		AtomList &Clear() { return operator()(); }

		//! Set list
		AtomList &operator()(int argc = 0,const t_atom *argv = NULL);
		//! Set list by another AtomList
		AtomList &operator =(const AtomList &a) { return operator()(a.Count(),a.Atoms()); }

		//! Get number of atoms in the list
		int Count() const { return cnt; }
		//! Get a reference to an indexed atom
		t_atom &operator [](int ix) { return lst[ix]; }
		//! Get a reference to an indexed atom
		const t_atom &operator [](int ix) const { return lst[ix]; }

		//! Get a pointer to the list of atoms
		t_atom *Atoms() { return lst; }
		//! Get a pointer to the list of atoms
		const t_atom *Atoms() const { return lst; }
		
	protected:
		int cnt;
		t_atom *lst;
	};


	//! Class representing an "anything"
	class AtomAnything: 
		public AtomList
	{
	public:
		//! Construct anything
		AtomAnything(const t_symbol *h = NULL,int argc = 0,t_atom *argv = NULL): AtomList(argc,argv),hdr(h) {}
		//! Construct anything
		AtomAnything(const char *h,int argc = 0,t_atom *argv = NULL): AtomList(argc,argv),hdr(MakeSymbol(h)) {}
		//! Construct anything
		AtomAnything(const AtomAnything &a): AtomList(a),hdr(a.hdr) {}

		//! Clear anything
		AtomAnything &Clear() { return operator()(); }

		//! Get header symbol of anything
		const t_symbol *Header() const { return hdr; }
		
		//! Set anything
		AtomAnything &operator()(const t_symbol *h = NULL,int argc = 0,const t_atom *argv = NULL)
		{ 
			hdr = h; AtomList::operator()(argc,argv);	
			return *this; 
		}

		//! Set list by another AtomAnything
		AtomAnything &operator =(const AtomAnything &a) { return operator()(a.Header(),a.Count(),a.Atoms()); }

	protected:
		const t_symbol *hdr;
	};

//!		@} 

// --- clock stuff ------------------------------------------------


	/*!	\defgroup FLEXT_C_CLOCK Flext clock functions

		At the moment there are none

		@{ 
	*/

//!		@} 

// --- utilities --------------------------------------------------

	/*!	\defgroup FLEXT_C_UTIL Flext utility functions

		@{ 
	*/

	//! Copy an atom
	static void CopyAtom(t_atom *dst,const t_atom *src) { *dst = *src; }
	//! Copy a list of atoms
	static t_atom *CopyList(int argc,const t_atom *argv);
	//! Copy a memory region
	static void CopyMem(void *dst,const void *src,int bytes);
	//! Sleep for an amount of time
	static void Sleep(float s);

//!		@} 

// xxx internal stuff xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// --- thread stuff -----------------------------------------------

#ifdef FLEXT_THREADS
	class thr_params;
	static bool StartThread(void *(*)(thr_params *p),thr_params *p,char *methname);
	bool PushThread();
	void PopThread();
#endif

protected:

	flext_base();
	virtual ~flext_base();
		
// inlets and outlets
		
	struct xlet {	
		enum type {
			tp_none = 0,
			tp_float,tp_int,tp_sym,tp_list,tp_sig,tp_any
		};

		xlet(type t,const char *desc = NULL);
		~xlet();
		
		char *desc;
		type tp;
		xlet *nxt;
	};

	/*!	\addtogroup FLEXT_C_INOUT 

		@{ 
	*/

	unsigned long XletCode(xlet::type tp = xlet::tp_none,...); // end list with 0 (= tp_none) !!

	void AddInlets(unsigned long code); // use XletCode to get code value
	void AddInlet(xlet::type tp,int mult = 1,const char *desc = NULL) { AddXlet(tp,mult,desc,inlist); }
	void AddOutlets(unsigned long code); // use XletCode to get code value
	void AddOutlet(xlet::type tp,int mult = 1,const char *desc = NULL) { AddXlet(tp,mult,desc,outlist); }

	void DescInlet(int ix,const char *desc) { DescXlet(ix,desc,inlist); }
	void DescOutlet(int ix,const char *desc) { DescXlet(ix,desc,outlist); }

//!		@} 


#ifdef FLEXT_THREADS
	void QueueBang(outlet *o); 
	void QueueFloat(outlet *o,float f); 
	void QueueInt(outlet *o,int f); 
	void QueueSymbol(outlet *o,const t_symbol *s); 
	void QueueList(outlet *o,int argc,t_atom *argv); 
	void QueueAnything(outlet *o,const t_symbol *s,int argc,t_atom *argv); 
#endif

// method handling

	class methitem { 
	public:
		methitem(int inlet,t_symbol *t);
		~methitem();

		void SetArgs(methfun fun,int argc,metharg *args);

		t_symbol *tag;
		int inlet;
		int argc;
		metharg *args;
		methfun fun;
		
		methitem *nxt;
	};
	
	void AddMethItem(methitem *m);
	
private:

	static void Setup(t_class *c);

	xlet *inlist,*outlist;
	int incnt,outcnt,insigs,outsigs;
	outlet **outlets;
	bool distmsgs;

	void AddXlet(xlet::type tp,int mult,const char *desc,xlet *&root);	
	void DescXlet(int ix,const char *desc,xlet *&root);	

	methitem *mlst;

#ifdef FLEXT_THREADS
	bool shouldexit;
	int thrcount;
	
	pthread_t thrid;  // the thread that created the object (the system thread)

	class qmsg;
	qmsg *qhead,*qtail;
	t_qelem *qclk;
	ThrMutex qmutex;
#ifdef MAXMSP
	t_clock *yclk;
	static void YTick(flext_base *th);
#endif

	static void QTick(flext_base *th);
	void Queue(qmsg *m);

	class thr_entry;
	thr_entry *thrhead,*thrtail;
	ThrMutex tlmutex;
#endif

#ifdef PD
	// proxy object (for additional inlets) stuff
	struct px_object;
	friend struct px_object;
#elif defined(MAXMSP)
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

	// callback functions

	static void cb_help(t_class *c);

	static void cb_loadbang(t_class *c);
#ifdef MAXMSP
	static void cb_assist(t_class *c,void *b,long msg,long arg,char *s);
#endif
};

#endif
