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
	virtual void m_method_(int inlet,const t_symbol *s,int argc,t_atom *argv);


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
		
		//! Graphic auto refresh interval
		void SetRefrIntv(float intv);

	protected:
		t_symbol *sym;
		t_sample *data;
		int chns,frames;
#ifdef PD
		float interval;
		bool isdirty,ticking;
		t_clock *tick;

	private:
		static void cb_tick(buffer *b);
#endif
	};


// --- inlet/outlet stuff -----------------------------------------	

	// define inlets/outlets - all (also default) inlets must be defined
	// argument m specifies multiple inlet/outlet count
	
//	void AddInDef() { AddInlet(xlet::tp_def,1); }

	//! add inlet(s) for anythings
	void AddInAnything(int m = 1) { AddInlet(xlet::tp_any,m); } // leftmost or via proxy
	//! add inlet(s) for floats
	void AddInFloat(int m = 1) { AddInlet(xlet::tp_float,m); }
	//! add inlet(s) for ints
	void AddInInt(int m = 1) { AddInlet(xlet::tp_int,m); }
	//! add inlet(s) for symbols
	void AddInSymbol(int m = 1) { AddInlet(xlet::tp_sym,m); }
	//! add inlet(s) for bang
	void AddInBang(int m = 1) { AddInlet(xlet::tp_sym,m); }
	//! add inlet(s) for lists
	void AddInList(int m = 1) { AddInlet(xlet::tp_list,m); }  // via proxy
	
	//! add outlet(s) for anythings
	void AddOutAnything(int m = 1) { AddOutlet(xlet::tp_any,m); }
	//! add outlet(s) for floats
	void AddOutFloat(int m = 1) { AddOutlet(xlet::tp_float,m); }
	//! add outlet(s) for ints
	void AddOutInt(int m = 1) { AddOutlet(xlet::tp_int,m); }
	//! add outlet(s) for symbols
	void AddOutSymbol(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	//! add outlet(s) for bangs
	void AddOutBang(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	//! add outlet(s) for lists
	void AddOutList(int m = 1) { AddOutlet(xlet::tp_list,m); }
	
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

//	void ToOutFlint(outlet *o,t_flint f); 
	//! output flint (index n starts with 0)
//	void ToOutFlint(int n,t_flint f) { outlet *o = GetOut(n); if(o) ToOutFlint(o,f); }
	
	void ToOutInt(outlet *o,int f); 
	//! output int (index n starts with 0)
	void ToOutInt(int n,int f) { outlet *o = GetOut(n); if(o) ToOutInt(o,f); }
	
	void ToOutSymbol(outlet *o,const t_symbol *s); 
	//! output symbol (index n starts with 0)
	void ToOutSymbol(int n,const t_symbol *s) { outlet *o = GetOut(n); if(o) ToOutSymbol(o,const_cast<t_symbol *>(s)); }

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

	void AddMethod(int inlet,void (*m)(flext_base *,int argc,t_atom *argv)) { AddMethod(inlet,"list",(methfun)m,a_gimme,a_null); }
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *)) { AddMethod(inlet,tag,(methfun)m,a_null); }  // pure method
	void AddMethod(int inlet,void (*m)(flext_base *,t_symbol *s,int argc,t_atom *argv)) { AddMethod(inlet,"anything",(methfun)m,a_xgimme,a_null); } // anything
	void AddMethod(int inlet,void (*m)(flext_base *,t_symbol *s)) { AddMethod(inlet,"symbol",(methfun)m,a_symbol,a_null); } // anything
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
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,int argc,t_atom *argv)) { AddMethod(inlet,tag,(methfun)m,a_gimme,a_null); } // method+gimme
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,t_symbol *s,int argc,t_atom *argv)) { AddMethod(inlet,tag,(methfun)m,a_xgimme,a_null); } // method+gimme 
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,t_symbol *s)) { AddMethod(inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
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

// --- list creation stuff ----------------------------------------


// --- clock stuff ------------------------------------------------


// xxx internal stuff xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

protected:

	flext_base();
	virtual ~flext_base();
		
// inlets and outlets
		
	struct xlet {	
		enum type {
			tp_none = 0,
			tp_float,/*tp_flint,*/ tp_int,tp_sym,tp_list,tp_sig,tp_any
		};

		xlet(type t): tp(t),nxt(NULL) {}
		~xlet();
		
		type tp;
		xlet *nxt;
	};
	
	void AddInlet(xlet::type tp,int mult) { AddXlet(tp,mult,inlist); }
	void AddOutlet(xlet::type tp,int mult) { AddXlet(tp,mult,outlist); }

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

	void AddXlet(xlet::type tp,int mult,xlet *&root);	

	methitem *mlst;

#ifdef PD
	// proxy object (for additional inlets) stuff
	struct px_object;
	friend struct px_object;
	static void cb_px_anything(t_class *c,const t_symbol *s,int argc,t_atom *argv);
#elif defined(MAXMSP)
	typedef object px_object;
	static void cb_px_anything(t_class *c,const t_symbol *s,int argc,t_atom *argv);
	static void cb_px_int(t_class *c,int v);
	static void cb_px_float(t_class *c,float f);
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



// === flext_dsp ==================================================

/*! \class flext_dsp
	\brief dsp enabled base object
*/

class flext_dsp:
	public flext_base
{
	FLEXT_HEADER_S(flext_dsp,flext_base,Setup)
	
public:

	//! returns current sample rate
	float Samplerate() const { return srate; }
	
	//! returns current block size
	int Blocksize() const { return blksz; }
	
	//! returns number of audio system input channels
	int ChannelsIn() const { return chnsin; }
	//! returns number of audio system output channels
	int ChannelsOut() const { return chnsout; }
	

// --- inheritable virtual methods --------------------------------

	/*! \brief Called on every dsp init.
		\remark Don't expect any valid data in the signal vectors!
		@param n: frames (aka samples) in one signal vector
		@param insigs: array of input vectors  (get number with function CntInSig())
		@param outsigs: array of output vectors  (get number with function CntOutSig())
	*/
	virtual void m_dsp(int n,t_sample *const *insigs,t_sample *const *outsigs);

	/*! \brief Called with every signal vector.
		Here you do the dsp calculation
		@param n: frames (aka samples) in one signal vector
		@param insigs: array of input vectors  (get number with function CntInSig())
		@param outsigs: array of output vectors  (get number with function CntOutSig())
	*/
	virtual void m_signal(int n,t_sample *const *insigs,t_sample *const *outsigs) = 0;

#ifndef MAXMSP
	//! called with "enable" message: pauses/resumes dsp - implicitely defined in MaxMSP
	virtual void m_enable(bool on);
#endif

// --- inlet/outlet stuff -----------------------------------------	

	//! add signal inlet
	void AddInSignal(int m = 1) { AddInlet(xlet::tp_sig,m); }

	//! add signal outlet
	void AddOutSignal(int m = 1) { AddOutlet(xlet::tp_sig,m); }


protected:
	
	flext_dsp();
	~flext_dsp();
	
private:

	// not static, could be different in different patchers..
	float srate; 
	int blksz;
	int chnsin,chnsout;

	// setup function
	static void Setup(t_class *c);

	// callback functions

	static void cb_dsp(t_class *c,t_signal **s);
#ifndef MAXMSP
	static void cb_enable(t_class *c,t_flint on);
	bool dspon;
#endif

	// dsp stuff

	static t_int *dspmeth(t_int *w); 
	t_sample **invecs,**outvecs;
};

#endif
