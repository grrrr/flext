/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __FLEXT_H
#define __FLEXT_H

#define FLEXT_VERSION 202
#define FLEXT_VERSTR "0.2.2"

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

// message only base object


class flext_base:
	public flext_obj
{
	FLEXT_HEADER_S(flext_base,flext_obj,Setup)
	
public:

// ---  compatibility mode ----------------------------------------

	// if set flext allows only operations valid for all platforms
	// true by default!
	static bool compatibility;  


// --- inheritable virtual methods --------------------------------

	// called on "help" message: should post some text
	virtual void m_help();
	
	// called on patcher load (not on mere object creation!)
	virtual void m_loadbang() {}

	// quickhelp for inlets/outlets (Max/MSP only)
	virtual void m_assist(long /*msg*/,long /*arg*/,char * /*s*/) {}

	// called for every incoming message 
	// all method handling is done in there
	// returns true if a handler was found and called)
	virtual bool m_methodmain(int inlet,const t_symbol *s,int argc,t_atom *argv);

	// called for every unhandled message (by m_methodmain)
	virtual void m_method_(int inlet,const t_symbol *s,int argc,t_atom *argv);


// --- buffer/array stuff -----------------------------------------	

	// class for platform independent buffer handling
	class buffer
	{
	public:
		// construct buffer 
		// delayed = true: needs another Set(NULL) to initialize the buffer 
		//                 (in Max the externals are often created prior to the buffer objects)
		buffer(t_symbol *s = NULL,bool delayed = false);
		
		~buffer();
		
		// set to another buffer 
		// nameonly = true: sets name only, no buffer data 
		int Set(t_symbol *s = NULL,bool nameonly = false);
		
		// dirty buffer content
		// refr = true: forces immediate graphics refresh
		void Dirty(bool refr = false);
//		bool IsDirty() const { return isdirty; }
		
		// get symbol (or literal name) of buffer 
		t_symbol *Symbol() const { return sym; }
		const char *Name() const { return sym?sym->s_name:""; }
		
		// get pointer to buffer, channel and frame count
		// Channels are interleaved
		float *Data() { return data; }
		int Channels() const { return chns; }
		int Frames() const { return frames; }
		
		// graphic auto refresh interval
		void SetRefrIntv(float intv);

	protected:
		t_symbol *sym;
		float *data;
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
	void AddInAnything(int m = 1) { AddInlet(xlet::tp_any,m); } // leftmost or via proxy
	void AddInFloat(int m = 1) { AddInlet(xlet::tp_float,m); }
	void AddInFlint(int m = 1) { AddInlet(xlet::tp_flint,m); }
	void AddInSymbol(int m = 1) { AddInlet(xlet::tp_sym,m); }
	void AddInList(int m = 1) { AddInlet(xlet::tp_list,m); }  // via proxy
	
	void AddOutAnything(int m = 1) { AddOutlet(xlet::tp_any,m); }
	void AddOutFloat(int m = 1) { AddOutlet(xlet::tp_float,m); }
	void AddOutFlint(int m = 1) { AddOutlet(xlet::tp_flint,m); }
	void AddOutSymbol(int m = 1) { AddOutlet(xlet::tp_sym,m); }
	void AddOutList(int m = 1) { AddOutlet(xlet::tp_list,m); }
	
	// must be called to actually set up the defined inlets/outlets 
	// only ONCE!!!
	// returns true on successful creation of all inlets and outlets
	bool SetupInOut(); 

	int CntIn() const { return incnt; }
	int CntOut() const { return outcnt; }
	int CntInSig() const { return insigs; }
	int CntOutSig() const { return outsigs; }


	class outlet;

	// get pointer _after_ calling setup_inout()
	outlet *GetOut(int ix) { return (outlets && ix < outcnt)?outlets[ix]:NULL; }

	// output messages (n starts with 0)
	void ToOutFloat(outlet *o,float f); 
	void ToOutFloat(int n,float f) { outlet *o = GetOut(n); if(o) ToOutFloat(o,f); }
	void ToOutFlint(outlet *o,t_flint f); 
	void ToOutFlint(int n,t_flint f) { outlet *o = GetOut(n); if(o) ToOutFlint(o,f); }
	void ToOutSymbol(outlet *o,const t_symbol *s); 
	void ToOutSymbol(int n,const t_symbol *s) { outlet *o = GetOut(n); if(o) ToOutSymbol(o,const_cast<t_symbol *>(s)); }
	void ToOutList(outlet *o,int argc,t_atom *argv); 
	void ToOutList(int n,int argc,t_atom *argv)  { outlet *o = GetOut(n); if(o) ToOutList(o,argc,argv); }
	void ToOutAnything(outlet *o,const t_symbol *s,int argc,t_atom *argv); 
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
	void AddMethod(int inlet,void (*m)(flext_base *,int &)) { AddMethod(inlet,"int",(methfun)m,a_int,a_null); }  // single float
	void AddMethod(int inlet,void (*m)(flext_base *,int &,int &)) { AddMethod(inlet,"list",(methfun)m,a_int,a_int,a_null); } // list of 2 floats
	void AddMethod(int inlet,void (*m)(flext_base *,int &,int &,int &)) { AddMethod(inlet,"list",(methfun)m,a_int,a_int,a_int,a_null); } // list of 3 floats
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,int argc,t_atom *argv)) { AddMethod(inlet,tag,(methfun)m,a_gimme,a_null); } // method+gimme
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,t_symbol *s,int argc,t_atom *argv)) { AddMethod(inlet,tag,(methfun)m,a_xgimme,a_null); } // method+gimme 
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,t_symbol *s)) { AddMethod(inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,float &)) { AddMethod(inlet,tag,(methfun)m,a_float,a_null); }  // method+float
	void AddMethod(int inlet,const char *tag,void (*m)(flext_base *,int &)) { AddMethod(inlet,tag,(methfun)m,a_int,a_null); } // method+int

	// MaxMSP style of distributing list elements over (message) inlets
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
	static float GetFloat(const t_atom &a) { return a.a_w.w_float; }
	static float GetAFloat(const t_atom &a) { return IsFloat(a)?GetFloat(a):0; }
	static void SetFloat(t_atom &a,float v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }

	static bool IsSymbol(const t_atom &a) { return a.a_type == A_SYMBOL; }
	static t_symbol *GetSymbol(const t_atom &a) { return a.a_w.w_symbol; }
	static t_symbol *GetASymbol(const t_atom &a) { return IsSymbol(a)?GetSymbol(a):NULL; }  // NULL or empty symbol?
	static void SetSymbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_symbol = const_cast<t_symbol *>(s); }

	static bool IsString(const t_atom &a) { return IsSymbol(a); }
	static const char *GetString(const t_atom &a) { t_symbol *s = GetSymbol(a); return s?GetString(s):NULL; }  
	static void GetAString(const t_atom &a,char *buf,int szbuf);
	static void SetString(t_atom &a,const char *c) { SetSymbol(a,gensym(const_cast<char *>(c))); }

#ifdef PD
	static bool IsPointer(const t_atom &a) { return a.a_type == A_POINTER; }
	static t_gpointer *GetPointer(const t_atom &a) { return a.a_w.w_gpointer; }
	static t_gpointer *GetAPointer(const t_atom &a) { return IsPointer(a)?GetPointer(a):NULL; }
	static void SetPointer(t_atom &a,t_gpointer *p) { a.a_type = A_POINTER; a.a_w.w_gpointer = p; }

	static bool IsInt(const t_atom &) { return false; }
	static int GetInt(const t_atom &) { return 0; }
	static int GetAInt(const t_atom &) { return 0; }
//	static void SetInt(const t_atom &,int) { }

	static bool IsFlint(const t_atom &a) { return IsFloat(a); }
	static float GetAFlint(const t_atom &a) { return GetFloat(a); }
	static void SetFlint(t_atom &a,int v) { a.a_type = A_FLOAT; a.a_w.w_float = (float)v; }
#elif defined(MAXMSP)
	static bool IsPointer(const t_atom &) { return false; }
	static void *GetPointer(const t_atom &) { return NULL; }
	static void *GetAPointer(const t_atom &) { return NULL; }
//	static void SetPointer(t_atom &,void *) {}
	static bool IsInt(const t_atom &a) { return a.a_type == A_INT; }
	static int GetInt(const t_atom &a) { return a.a_w.w_long; }
	static int GetAInt(const t_atom &a) { return IsInt(a)?GetInt(a):0; }
	static void SetInt(t_atom &a,int v) { a.a_type = A_INT, a.a_w.w_long = v; }

	static bool IsFlint(const t_atom &a) { return IsFloat(a) || IsInt(a); }
	static float GetAFlint(const t_atom &a) { return IsFloat(a)?GetFloat(a):(IsInt(a)?GetInt(a):0); }
	static void SetFlint(t_atom &a,int v) { a.a_type = A_INT; a.a_w.w_long = v; }
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
			tp_float,tp_flint,tp_sym,tp_list,tp_sig,tp_any
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

// dsp enabled base object

class flext_dsp:
	public flext_base
{
	FLEXT_HEADER_S(flext_dsp,flext_base,Setup)
	
public:

	// returns current sample rate
	float Samplerate() const { return srate; }
	
	// returns current block size
	int Blocksize() const { return blksz; }
	
	// returns number of overall input and output channels
	int ChannelsIn() const { return chnsin; }
	int ChannelsOut() const { return chnsout; }
	

// --- inheritable virtual methods --------------------------------

	// called on every dsp init (don't expect valid data in the signal vectors!)
	//   n: frames (aka samples) in one signal vector
	//   insigs: array of input vectors  (get number with function cnt_insig())
	//   outsigs: array of output vectors  (get number with function cnt_outsig())
	virtual void m_dsp(int n,float *const *insigs,float *const *outsigs);

	// called with every signal vector (here you do the dsp calculation)
	//   n: frames (aka samples) in one signal vector
	//   insigs: array of input vectors  (get number with function cnt_insig())
	//   outsigs: array of output vectors  (get number with function cnt_outsig())
	virtual void m_signal(int n,float *const *insigs,float *const *outsigs) = 0;

	// called with "enable" message: pauses/resumes dsp - implicitely defined in MaxMSP
#ifndef MAXMSP
	virtual void m_enable(bool on);
#endif

// --- inlet/outlet stuff -----------------------------------------	

	// add signal inlet
	void AddInSignal(int m = 1) { AddInlet(xlet::tp_sig,m); }

	// add signal outlet
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
	float **invecs,**outvecs;
};




///////////////////////////////////////////////////////////
// These should be the used in the class definition
/////////////////////////////////////////////////////////

#define FL_CBCAST static_cast // should be dynamic_cast, but for speed and as gcc 3.0.4 has some bugs in it

// no arguments
#define FLEXT_CALLBACK(M_FUN) \
static void cb_ ## M_FUN(flext_base *c) { FL_CBCAST<thisType *>(c)->M_FUN(); }

// for anything
#define FLEXT_CALLBACK_A(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,t_symbol *s,int argc,t_atom *argv) { FL_CBCAST<thisType *>(c)->M_FUN(s,argc,argv); }

// for gimme
#define FLEXT_CALLBACK_G(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int argc,t_atom *argv) { FL_CBCAST<thisType *>(c)->M_FUN(argc,argv); }

// for boolean argument
#define FLEXT_CALLBACK_B(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int &arg1) { FL_CBCAST<thisType *>(c)->M_FUN(arg1 != 0); }

// 1 argument
#define FLEXT_CALLBACK_1(M_FUN,TP1) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1) { FL_CBCAST<thisType *>(c)->M_FUN(arg1); }

// 2 arguments
#define FLEXT_CALLBACK_2(M_FUN,TP1,TP2) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2) { FL_CBCAST<thisType *>(c)->M_FUN(arg1,arg2); }

// 3 arguments
#define FLEXT_CALLBACK_3(M_FUN,TP1,TP2,TP3) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3) { FL_CBCAST<thisType *>(c)->M_FUN(arg1,arg2,arg3); }

// 4 arguments
#define FLEXT_CALLBACK_4(M_FUN,TP1,TP2,TP3,TP4) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4) { FL_CBCAST<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4); }

// 5 arguments
#define FLEXT_CALLBACK_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static void cb_ ## M_FUN(flext_base *c,TP1 &arg1,TP2 &arg2,TP3 &arg3,TP4 &arg4,TP5 &arg5) { FL_CBCAST<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4,arg5); }

// Shortcuts

// for float argument(s)
#define FLEXT_CALLBACK_F(M_FUN) FLEXT_CALLBACK_1(M_FUN,float)
#define FLEXT_CALLBACK_FF(M_FUN) FLEXT_CALLBACK_2(M_FUN,float,float)
#define FLEXT_CALLBACK_FFF(M_FUN) FLEXT_CALLBACK_3(M_FUN,float,float,float)

// for int argument(s)
#define FLEXT_CALLBACK_I(M_FUN) FLEXT_CALLBACK_1(M_FUN,int)
#define FLEXT_CALLBACK_II(M_FUN) FLEXT_CALLBACK_2(M_FUN,int,int)
#define FLEXT_CALLBACK_III(M_FUN) FLEXT_CALLBACK_3(M_FUN,int,int,int)


// Shortcuts for method arguments
#define FLEXTARG_float a_float
#define FLEXTARG_int a_int
#define FLEXTARG_bool a_int
#define FLEXTARG_t_float a_float
#define FLEXTARG_t_symtype a_symbol
#define FLEXTARG_t_ptrtype a_pointer

#define FLEXTARG(TP) FLEXTARG_ ## TP


///////////////////////////////////////////////////////////
// These should be the used in the class' constructor
///////////////////////////////////////////////////////////

// enable list element distribution over inlets (if no better handler found)
#define FLEXT_ADDDIST() \
SetDist(true)	

// add handler for bang 
#define FLEXT_ADDBANG(IX,M_FUN) \
AddMethod(IX,"bang",cb_ ## M_FUN)	

// add handler for method with no args
#define FLEXT_ADDMETHOD(IX,M_FUN) \
AddMethod(IX,cb_ ## M_FUN)	

// add handler for method with no args
#define FLEXT_ADDMETHOD_(IX,M_TAG,M_FUN) \
AddMethod(IX,M_TAG,cb_ ## M_FUN)	

// add handler for method with 1 enum type arg
#define FLEXT_ADDMETHOD_E(IX,M_TAG,M_FUN) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),a_int,a_null)

// add handler for method with 1 arg
#define FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,TP1) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),a_null)	

// add handler for method with 2 args
#define FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,TP1,TP2) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),a_null)

// add handler for method with 3 args
#define FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,TP1,TP2,TP3) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),a_null)

// add handler for method with 4 args
#define FLEXT_ADDMETHOD_4(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),a_null)

// add handler for method with 5 args
#define FLEXT_ADDMETHOD_5(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4,TP5) \
AddMethod(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),FLEXTARG(TP5),a_null)

// shortcuts
// boolean argument
#define FLEXT_ADDMETHOD_B(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,bool)

// float argument(s)
#define FLEXT_ADDMETHOD_F(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,float)
#define FLEXT_ADDMETHOD_FF(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,float,float)
#define FLEXT_ADDMETHOD_FFF(IX,M_TAG,M_FUN) FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,float,float,float)

#endif


