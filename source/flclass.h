/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flclass.h
    \brief Declares the user accessible flext base classes
    
    !Lacking Details.!
*/

#ifndef __FLCLASS_H
#define __FLCLASS_H

//! include the header file declaring the base classes
#include <flbase.h>

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#ifdef __BORLANDC__
#pragma warn -8008 // Condition is always false
#pragma warn -8057 // Parameter is never used
#pragma warn -8066 // Unreachable code
#endif

// === flext_base ==================================================

/*! \class flext_base
	\brief message only base object
*/

//class qmsg;
//class thr_entry;

class flext_base:
	public flext_obj
{
	FLEXT_HEADER_S(flext_base,flext_obj,Setup)
	
public:

// ---  compatibility mode ----------------------------------------

	/*! \brief Cross-platform compatibility flag.
		If set flext allows only operations valid for all platforms.
		Set to true by default!
	*/
	static bool compatibility;  


// --- inheritable virtual methods --------------------------------

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


// --- buffer/array stuff -----------------------------------------	

	//! \class class for platform independent buffer handling
	class buffer
	{
	public:
		/*! \brief Construct buffer.
			@param delayed = true: only sets name, needs another Set(NULL) to really initialize the buffer 
			\remark As externals can be created prior to the buffer objects they are pointing to, initialization should be done at loadbang!
		*/
		buffer(t_symbol *s = NULL,bool delayed = false);
		
		//! Destroy buffer
		~buffer();

		/*! \brief Check if the data is valid
		*/
		bool Ok() const { return sym != NULL && data != NULL; }
		
		/*! \brief Check and update if the buffer has been changed (e.g. resized)
		*/
		bool Update();
		
		/*! \brief Set to specified buffer.
			@param nameonly: if true sets name only, but doesn't look at buffer actually
		*/
		int Set(t_symbol *s = NULL,bool nameonly = false);
		
		/*! \brief Declare buffer content as dirty.
			@param refr: if true forces immediate graphics refresh
		*/
		void Dirty(bool refr = false);
		
		//! Get symbol of buffer 
		t_symbol *Symbol() const { return sym; }
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
		t_symbol *sym;
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


// --- help -------------------------------------------------------	

	void DefineHelp(const char *ref);


// --- inlet/outlet stuff -----------------------------------------	

	// define inlets/outlets - all (also default) inlets must be defined
	// argument m specifies multiple inlet/outlet count
	
//	void AddInDef() { AddInlet(xlet::tp_def,1); }

	//! add inlet(s) for anythings
	void AddInAnything(int m = 1) { AddInlet(xlet::tp_any,m); } // leftmost or via proxy
	void AddInAnything(const char *desc,int m = 1) { AddInlet(xlet::tp_any,m,desc); } // leftmost or via proxy
	//! add inlet(s) for floats
	void AddInFloat(int m = 1) { AddInlet(xlet::tp_float,m); }
	void AddInFloat(const char *desc,int m = 1) { AddInlet(xlet::tp_float,m,desc); }
	//! add inlet(s) for ints
	void AddInInt(int m = 1) { AddInlet(xlet::tp_int,m); }
	void AddInInt(const char *desc,int m = 1) { AddInlet(xlet::tp_int,m,desc); }
	//! add inlet(s) for symbols
	void AddInSymbol(int m = 1) { AddInlet(xlet::tp_sym,m); }
	void AddInSymbol(const char *desc,int m = 1) { AddInlet(xlet::tp_sym,m,desc); }
	//! add inlet(s) for bang
	void AddInBang(int m = 1) { AddInlet(xlet::tp_sym,m); }
	void AddInBang(const char *desc,int m = 1) { AddInlet(xlet::tp_sym,m,desc); }
	//! add inlet(s) for lists
	void AddInList(int m = 1) { AddInlet(xlet::tp_list,m); }  // via proxy
	void AddInList(const char *desc,int m = 1) { AddInlet(xlet::tp_list,m,desc); }  // via proxy
	
	//! add outlet(s) for anythings
	void AddOutAnything(int m = 1) { AddOutlet(xlet::tp_any,m); }
	void AddOutAnything(const char *desc,int m = 1) { AddOutlet(xlet::tp_any,m,desc); }
	//! add outlet(s) for floats
	void AddOutFloat(int m = 1) { AddOutlet(xlet::tp_float,m); }
	void AddOutFloat(const char *desc,int m = 1) { AddOutlet(xlet::tp_float,m,desc); }
	//! add outlet(s) for ints
	void AddOutInt(int m = 1) { AddOutlet(xlet::tp_int,m); }
	void AddOutInt(const char *desc,int m = 1) { AddOutlet(xlet::tp_int,m,desc); }
	//! add outlet(s) for symbols
	void AddOutSymbol(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	void AddOutSymbol(const char *desc,int m = 1) { AddOutlet(xlet::tp_sym,m,desc); }
	//! add outlet(s) for bangs
	void AddOutBang(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	void AddOutBang(const char *desc,int m = 1) { AddOutlet(xlet::tp_sym,m,desc); }
	//! add outlet(s) for lists
	void AddOutList(int m = 1) { AddOutlet(xlet::tp_list,m); }
	void AddOutList(const char *desc,int m = 1) { AddOutlet(xlet::tp_list,m,desc); }
	
	/*! \brief set up inlets and outlets
		\remark Must be called ONCE to actually set up the defined inlets/outlets.
		\return True on successful creation of all inlets and outlets
	*/
	bool SetupInOut(); 

	//! get number of inlets
	int CntIn() const { return incnt; }
	//! get number of outlets
	int CntOut() const { return outcnt; }
	//! get number of signal inlets
	int CntInSig() const { return insigs; }
	//! get number of signal outlets
	int CntOutSig() const { return outsigs; }


	class outlet;

	//! get pointer to outlet (_after_ calling setup_inout()!)
	outlet *GetOut(int ix) { return (outlets && ix < outcnt)?outlets[ix]:NULL; }

	// output messages (index n starts with 0)

	void ToOutBang(outlet *o); 
	//! output bang (index n starts with 0)
	void ToOutBang(int n) { outlet *o = GetOut(n); if(o) ToOutBang(o); }

	void ToOutFloat(outlet *o,float f); 
	//! output float (index n starts with 0)
	void ToOutFloat(int n,float f) { outlet *o = GetOut(n); if(o) ToOutFloat(o,f); }

	void ToOutInt(outlet *o,int f); 
	//! output int (index n starts with 0)
	void ToOutInt(int n,int f) { outlet *o = GetOut(n); if(o) ToOutInt(o,f); }
	
	void ToOutSymbol(outlet *o,const t_symbol *s); 
	//! output symbol (index n starts with 0)
	void ToOutSymbol(int n,const t_symbol *s) { outlet *o = GetOut(n); if(o) ToOutSymbol(o,s); }

	void ToOutString(outlet *o,const char *s) { ToOutSymbol(o,gensym(const_cast<char *>(s))); }
	//! output string (index n starts with 0)
	void ToOutString(int n,const char *s) { outlet *o = GetOut(n); if(o) ToOutString(o,s); }

	void ToOutList(outlet *o,int argc,t_atom *argv); 
	//! output list (index n starts with 0)
	void ToOutList(int n,int argc,t_atom *argv)  { outlet *o = GetOut(n); if(o) ToOutList(o,argc,argv); }
	
	void ToOutAnything(outlet *o,const t_symbol *s,int argc,t_atom *argv); 
	//! output anything (index n starts with 0)
	void ToOutAnything(int n,const t_symbol *s,int argc,t_atom *argv)  { outlet *o = GetOut(n); if(o) ToOutAnything(o,const_cast<t_symbol *>(s),argc,argv); }
		

// --- message handling -------------------------------------------

	enum metharg {
		a_null = 0,
		a_float,a_int, 
		a_symbol,a_pointer,
		a_gimme,a_xgimme
	};

	typedef void (*methfun)(t_class *c);

	void AddMethodDef(int inlet); // call virtual function for inlet
	void AddMethodDef(int inlet,const char *tag); // call virtual function for tag && inlet
	void AddMethod(int inlet,const char *tag,methfun fun,metharg tp,...); 

	void AddMethod(int inlet,void (*m)(flext_base *,int,t_atom *)) { AddMethod(inlet,"list",(methfun)m,a_gimme,a_null); }
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *)) { AddMethod(inlet,tag,(methfun)m,a_null); }  // pure method
	void AddMethod(int inlet,void (*m)(flext_base *,t_symbol *,int,t_atom *)) { AddMethod(inlet,"anything",(methfun)m,a_xgimme,a_null); } // anything
	void AddMethod(int inlet,void (*m)(flext_base *,t_symbol *&)) { AddMethod(inlet,"symbol",(methfun)m,a_symbol,a_null); } // single symbol
	void AddMethod(int inlet,void (*m)(flext_base *,float &)) { AddMethod(inlet,"float",(methfun)m,a_float,a_null); }  // single float
	void AddMethod(int inlet,void (*m)(flext_base *,float &,float &)) { AddMethod(inlet,"list",(methfun)m,a_float,a_float,a_null); } // list of 2 floats
	void AddMethod(int inlet,void (*m)(flext_base *,float &,float &,float &)) { AddMethod(inlet,"list",(methfun)m,a_float,a_float,a_float,a_null); } // list of 3 floats
#ifdef PD
	void AddMethod(int inlet,void (*m)(flext_base *,int &)) { AddMethod(inlet,"float",(methfun)m,a_int,a_null); }  // single float
#else
	void AddMethod(int inlet,void (*m)(flext_base *,int &)) { AddMethod(inlet,"int",(methfun)m,a_int,a_null); }  // single float
#endif
	void AddMethod(int inlet,void (*m)(flext_base *,int &,int &)) { AddMethod(inlet,"list",(methfun)m,a_int,a_int,a_null); } // list of 2 floats
	void AddMethod(int inlet,void (*m)(flext_base *,int &,int &,int &)) { AddMethod(inlet,"list",(methfun)m,a_int,a_int,a_int,a_null); } // list of 3 floats
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,int,t_atom *)) { AddMethod(inlet,tag,(methfun)m,a_gimme,a_null); } // method+gimme
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,t_symbol *,int,t_atom *)) { AddMethod(inlet,tag,(methfun)m,a_xgimme,a_null); } // method+gimme 
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,t_symbol *&)) { AddMethod(inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,float &)) { AddMethod(inlet,tag,(methfun)m,a_float,a_null); }  // method+float
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,int &)) { AddMethod(inlet,tag,(methfun)m,a_int,a_null); } // method+int

	//! Set MaxMSP style of distributing list elements over (message) inlets
	void SetDist(bool d = true) { distmsgs = d; }

// --- various symbols --------------------------------------------

	static const t_symbol *sym_float;
	static const t_symbol *sym_symbol;
	static const t_symbol *sym_bang;
	static const t_symbol *sym_list;
	static const t_symbol *sym_anything;
	static const t_symbol *sym_int;
	static const t_symbol *sym_pointer;

#ifdef PD
	static const t_symbol *sym_signal;
#endif

	static const t_symbol *MakeSymbol(const char *s);

	// get symbol string
	static const char *GetString(const t_symbol *s) { return s->s_name; }  
	static const char *GetAString(const t_symbol *s) { return s?s->s_name:""; }  
	
	static void *GetThing(t_symbol *s) { return s->s_thing; }  
	static void SetThing(t_symbol *s,void *dt) { s->s_thing = (t_thing)dt; }  

// --- argument list stuff ----------------------------------------
		
	static bool IsFloat(const t_atom &a) { return a.a_type == A_FLOAT; }
	static bool CanbeFloat(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	static float GetFloat(const t_atom &a) { return a.a_w.w_float; }
//	static float GetAFloat(const t_atom &a) { return IsFloat(a)?GetFloat(a):0; }
	static float GetAFloat(const t_atom &a) { return GetAFlint(a); }
	static void SetFloat(t_atom &a,float v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }

	static bool IsSymbol(const t_atom &a) { return a.a_type == A_SYMBOL; }
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_symbol; }
	static t_symbol *GetASymbol(const t_atom &a) { return IsSymbol(a)?GetSymbol(a):NULL; }  // NULL or empty symbol?
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_symbol = const_cast<t_symbol *>(s); }

	static bool IsString(const t_atom &a) { return IsSymbol(a); }
	static const char *GetString(const t_atom &a) { t_symbol *s = GetSymbol(a); return s?GetString(s):NULL; }  
	static void GetAString(const t_atom &a,char *buf,int szbuf);
	static void SetString(t_atom &a,const char *c) { SetSymbol(a,gensym(const_cast<char *>(c))); }

	static bool CanbeInt(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	static float GetAInt(const t_atom &a) { return GetAFlint(a); }

	static bool IsFlint(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	static float GetAFlint(const t_atom &a) { return IsFloat(a)?GetFloat(a):(IsInt(a)?GetInt(a):0); }
#ifdef PD
	static bool IsInt(const t_atom &) { return false; }
	static int GetInt(const t_atom &) { return 0; }
//	static void SetInt(const t_atom &,int) { }

	static void SetFlint(t_atom &a,int v) { a.a_type = A_FLOAT; a.a_w.w_float = (float)v; }

	static bool IsPointer(const t_atom &a) { return a.a_type == A_POINTER; }
	static t_gpointer *GetPointer(const t_atom &a) { return a.a_w.w_gpointer; }
	static t_gpointer *GetAPointer(const t_atom &a) { return IsPointer(a)?GetPointer(a):NULL; }
	static void SetPointer(t_atom &a,t_gpointer *p) { a.a_type = A_POINTER; a.a_w.w_gpointer = p; }

#elif defined(MAXMSP)
	static bool IsInt(const t_atom &a) { return a.a_type == A_INT; }
	static int GetInt(const t_atom &a) { return a.a_w.w_long; }
	static void SetInt(t_atom &a,int v) { a.a_type = A_INT, a.a_w.w_long = v; }

	static void SetFlint(t_atom &a,int v) { a.a_type = A_INT; a.a_w.w_long = v; }

	static bool IsPointer(const t_atom &) { return false; }
	static void *GetPointer(const t_atom &) { return NULL; }
	static void *GetAPointer(const t_atom &) { return NULL; }
//	static void SetPointer(t_atom &,void *) {}
#endif

#ifdef FLEXT_THREADS
// --- thread stuff -----------------------------------------------

	bool ShouldExit() const { return shouldexit; }

	bool IsSystemThread() const { pthread_t cur = pthread_self(); return pthread_equal(cur,thrid) != 0; }

	void Yield() // yield to other threads (for cooperative multitasking)
#ifndef MAXMSP
	{}
#else
	;
#endif

	void NormalPriority();
	void LowerPriority();
	void LowestPriority();

	class ThrMutex 
	{
	public:
		ThrMutex() { pthread_mutex_init(&mutex,NULL); }
		~ThrMutex() { pthread_mutex_destroy(&mutex); }

		int Lock() { return pthread_mutex_lock(&mutex); }
		int TryLock() { return pthread_mutex_trylock(&mutex); }
		int Unlock() { return pthread_mutex_unlock(&mutex); }
	protected:
		pthread_mutex_t mutex;
	};

	class ThrCond:
		public ThrMutex
	{
	public:
		ThrCond() { pthread_cond_init(&cond,NULL); }
		~ThrCond() { pthread_cond_destroy(&cond); }

		int Wait() { return pthread_cond_wait(&cond,&mutex); }

		int TimedWait(float time) 
		{ 
			timespec tm; tm.tv_sec = (long)time; tm.tv_nsec = (long)((time-(long)time)*1.e9);
			return pthread_cond_timedwait(&cond,&mutex,&tm); 
		}

		int Signal() { return pthread_cond_signal(&cond); }
		int Broadcast() { return pthread_cond_broadcast(&cond); }
	protected:
		pthread_cond_t cond;
	};
#endif // FLEXT_THREADS

// --- atom list stuff -------------------------------------------

	class AtomList
	{
	public:
		AtomList(int argc,t_atom *argv = NULL);
		~AtomList();

		int Count() const { return cnt; }
		t_atom &operator [](int ix) { return lst[ix]; }
		const t_atom &operator [](int ix) const { return lst[ix]; }

		t_atom *Atoms() { return lst; }
		const t_atom *Atoms() const { return lst; }
		
	protected:
		int cnt;
		t_atom *lst;
	};


	class AtomAnything: 
		public AtomList
	{
	public:
		AtomAnything(const t_symbol *h,int argc,t_atom *argv): AtomList(argc,argv),hdr(h) {}

		const t_symbol *Header() const { return hdr; }
		
	protected:
		const t_symbol *hdr;
	};

// --- clock stuff ------------------------------------------------


// --- utilities --------------------------------------------------

	static void CopyAtom(t_atom *dst,const t_atom *src) { *dst = *src; }
	static t_atom *CopyList(int argc,const t_atom *argv);
	static void CopyMem(void *dst,const void *src,int bytes);
	static void Sleep(float s);

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

	unsigned long XletCode(xlet::type tp = xlet::tp_none,...); // end list with 0 (= tp_none) !!

	void AddInlets(unsigned long code); // use XletCode to get code value
	void AddInlet(xlet::type tp,int mult = 1,const char *desc = NULL) { AddXlet(tp,mult,desc,inlist); }
	void AddOutlets(unsigned long code); // use XletCode to get code value
	void AddOutlet(xlet::type tp,int mult = 1,const char *desc = NULL) { AddXlet(tp,mult,desc,outlist); }

	void DescInlet(int ix,const char *desc) { DescXlet(ix,desc,inlist); }
	void DescOutlet(int ix,const char *desc) { DescXlet(ix,desc,outlist); }


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
