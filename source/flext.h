/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __FLEXT_H
#define __FLEXT_H

#define FLEXT_VERSION 200
#define FLEXT_VERSTR "0.2.0"

#include <flbase.h> 

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include <list> // C++ STL lists 

 
// a few type definitions for the lazy

#define F float 
#define D double
#define I int
#define L long
#define C char
#define BL bool
#define V void
#define FI t_flint


// === flext_base ==================================================

// message only base object


class flext_base:
	public flext_obj
{
	FLEXT_HEADER_S(flext_base,flext_obj)
	
public:

// ---  compatibility mode ----------------------------------------

	// if set flext allows only operations valid for all platforms
	// true by default!
	static BL compatibility;  


// --- inheritable virtual methods --------------------------------

	// called on "help" message: should post some text
	virtual V m_help();
	
	// called on patcher load (not on mere object creation!)
	virtual V m_loadbang() {}

	// quickhelp for inlets/outlets (Max/MSP only)
	virtual V m_assist(L /*msg*/,L /*arg*/,C * /*s*/) {}

	// called for every incoming message 
	// all method handling is done in there
	// returns true if a handler was found and called)
	virtual BL m_methodmain(I inlet,const t_symbol *s,I argc,t_atom *argv);

	// called for every unhandled message (by m_methodmain)
	virtual V m_method_(I inlet,const t_symbol *s,I argc,t_atom *argv);


// --- buffer/array stuff -----------------------------------------	

	// class for platform independent buffer handling
	class buffer
	{
	public:
		// construct buffer 
		// delayed = true: needs another Set(NULL) to initialize the buffer 
		//                 (in Max the externals are often created prior to the buffer objects)
		buffer(t_symbol *s = NULL,BL delayed = false);
		
		~buffer();
		
		// set to another buffer 
		// nameonly = true: sets name only, no buffer data 
		I Set(t_symbol *s = NULL,BL nameonly = false);
		
		// dirty buffer content
		// refr = true: forces immediate graphics refresh
		V Dirty(BL refr = false);
		
		// get symbol (or literal name) of buffer 
		t_symbol *Symbol() const { return sym; }
		const C *Name() const { return sym?sym->s_name:""; }
		
		// get pointer to buffer, channel and frame count
		F *Data() { return data; }
		I Channels() const { return chns; }
		I Frames() const { return frames; }
		
		// graphic auto refresh interval
		V SetRefrIntv(F intv);

	protected:
		t_symbol *sym;
		F *data;
		I chns,frames;
#ifdef PD
		F interval;
		BL isdirty,ticking;
		t_clock *tick;

	private:
		static V cb_tick(buffer *b);
#endif
	};


// --- inlet/outlet stuff -----------------------------------------	

	// define inlets/outlets - all (also default) inlets must be defined
	// argument m specifies multiple inlet/outlet count
	
//	V add_in_def() { AddInlet(xlet::tp_def,1); }
	V add_in_anything(I m = 1) { AddInlet(xlet::tp_any,m); } // leftmost or via proxy
	V add_in_float(I m = 1) { AddInlet(xlet::tp_float,m); }
	V add_in_flint(I m = 1) { AddInlet(xlet::tp_flint,m); }
	V add_in_symbol(I m = 1) { AddInlet(xlet::tp_sym,m); }
	V add_in_list(I m = 1) { AddInlet(xlet::tp_list,m); }  // via proxy
	
	V add_out_anything(I m = 1) { AddOutlet(xlet::tp_any,m); }
	V add_out_float(I m = 1) { AddOutlet(xlet::tp_float,m); }
	V add_out_flint(I m = 1) { AddOutlet(xlet::tp_flint,m); }
	V add_out_symbol(I m = 1) { AddOutlet(xlet::tp_sym,m); }
	V add_out_list(I m = 1) { AddOutlet(xlet::tp_list,m); }
	
	// must be called to actually set up the defined inlets/outlets 
	// only ONCE!!!
	// returns true on successful creation of all inlets and outlets
	BL setup_inout(); 

	I cnt_in() const { return incnt; }
	I cnt_out() const { return outcnt; }
	I cnt_insig() const { return insigs; }
	I cnt_outsig() const { return outsigs; }


	class outlet;

	// get pointer _after_ calling setup_inout()
	outlet *get_out(I ix) { return (outlets && ix < outcnt)?outlets[ix]:NULL; }

	// output messages (n starts with 0)
	V to_out_float(outlet *o,F f); 
	V to_out_float(I n,F f) { outlet *o = get_out(n); if(o) to_out_float(o,f); }
	V to_out_flint(outlet *o,FI f); 
	V to_out_flint(I n,FI f) { outlet *o = get_out(n); if(o) to_out_flint(o,f); }
	V to_out_symbol(outlet *o,const t_symbol *s); 
	V to_out_symbol(I n,const t_symbol *s) { outlet *o = get_out(n); if(o) to_out_symbol(o,const_cast<t_symbol *>(s)); }
	V to_out_list(outlet *o,I argc,t_atom *argv); 
	V to_out_list(I n,I argc,t_atom *argv)  { outlet *o = get_out(n); if(o) to_out_list(o,argc,argv); }
	V to_out_anything(outlet *o,const t_symbol *s,I argc,t_atom *argv); 
	V to_out_anything(I n,const t_symbol *s,I argc,t_atom *argv)  { outlet *o = get_out(n); if(o) to_out_anything(o,const_cast<t_symbol *>(s),argc,argv); }
		
		
// --- message handling -------------------------------------------

	enum metharg {
		a_null = 0,
		a_float,a_int, //a_bool,
		a_symbol,a_pointer,
		a_gimme
	};

	typedef V (*methfun)(t_class *c);

	V add_meth_def(I inlet); // call virtual function for inlet
	V add_meth_def(I inlet,const C *tag); // call virtual function for tag && inlet
	V add_meth_one(I inlet,const C *tag,methfun fun,metharg tp,...); 
//	V add_meth_ixd(I inlet,const C *tag,methfun fun,metharg tp,...); 

	V add_meth(I inlet,V (*m)(flext_base *,I argc,t_atom *argv)) { add_meth_one(inlet,"list",(methfun)m,a_gimme,a_null); }
	V add_meth(I inlet,const C *tag,V (*m)(flext_base *)) { add_meth_one(inlet,tag,(methfun)m,a_null); }  // pure method
	V add_meth(I inlet,V (*m)(flext_base *,t_symbol *s,I argc,t_atom *argv)) { add_meth_one(inlet,"anything",(methfun)m,a_gimme,a_null); } // anything
	V add_meth(I inlet,V (*m)(flext_base *,t_symbol *s)) { add_meth_one(inlet,"symbol",(methfun)m,a_symbol,a_null); } // anything
	V add_meth(I inlet,V (*m)(flext_base *,F)) { add_meth_one(inlet,"float",(methfun)m,a_float,a_null); }  // single float
	V add_meth(I inlet,V (*m)(flext_base *,F,F)) { add_meth_one(inlet,"list",(methfun)m,a_float,a_float,a_null); } // list of 2 floats
	V add_meth(I inlet,V (*m)(flext_base *,F,F,F)) { add_meth_one(inlet,"list",(methfun)m,a_float,a_float,a_float,a_null); } // list of 3 floats
	V add_meth(I inlet,V (*m)(flext_base *,I)) { add_meth_one(inlet,"int",(methfun)m,a_int,a_null); }  // single float
	V add_meth(I inlet,V (*m)(flext_base *,I,I)) { add_meth_one(inlet,"list",(methfun)m,a_int,a_int,a_null); } // list of 2 floats
	V add_meth(I inlet,V (*m)(flext_base *,I,I,I)) { add_meth_one(inlet,"list",(methfun)m,a_int,a_int,a_int,a_null); } // list of 3 floats
	V add_meth(I inlet,const C *tag,V (*m)(flext_base *,I argc,t_atom *argv)) { add_meth_one(inlet,tag,(methfun)m,a_gimme,a_null); } // method+gimme
//	V add_meth(I inlet,const C *tag,V (*m)(flext_base *,BL b)) { add_meth_one(inlet,tag,(methfun)m,a_bool,a_gimme,a_null); } // method+boolean
	V add_meth(I inlet,const C *tag,V (*m)(flext_base *,t_symbol *s)) { add_meth_one(inlet,tag,(methfun)m,a_symbol,a_null); } // method+symbol
	V add_meth(I inlet,const C *tag,V (*m)(flext_base *,F)) { add_meth_one(inlet,tag,(methfun)m,a_float,a_null); }  // method+float
	V add_meth(I inlet,const C *tag,V (*m)(flext_base *,I)) { add_meth_one(inlet,tag,(methfun)m,a_int,a_null); } // method+int


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

// --- argument list stuff ----------------------------------------
		
	static BL is_float(const t_atom &a) { return a.a_type == A_FLOAT; }
	static F get_float(const t_atom &a) { return a.a_w.w_float; }
	static F geta_float(const t_atom &a) { return is_float(a)?get_float(a):0; }
	static V set_float(t_atom &a,F v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }

	static BL is_symbol(const t_atom &a) { return a.a_type == A_SYMBOL; }
	static t_symbol *get_symbol(const t_atom &a) { return a.a_w.w_symbol; }
	static t_symbol *geta_symbol(const t_atom &a) { return is_symbol(a)?get_symbol(a):NULL; }  // NULL or empty symbol?
	static V set_symbol(t_atom &a,const t_symbol *s) { a.a_type = A_SYMBOL; a.a_w.w_symbol = const_cast<t_symbol *>(s); }

#ifdef PD
	static BL is_pointer(const t_atom &a) { return a.a_type == A_POINTER; }
	static t_gpointer *get_pointer(const t_atom &a) { return a.a_w.w_gpointer; }
	static t_gpointer *geta_pointer(const t_atom &a) { return is_pointer(a)?get_pointer(a):NULL; }
	static V set_pointer(t_atom &a,t_gpointer *p) { a.a_type = A_POINTER; a.a_w.w_gpointer = p; }

	static BL is_int(const t_atom &) { return false; }
	static I get_int(const t_atom &) { return 0; }
	static I geta_int(const t_atom &) { return 0; }
//	static V set_int(const t_atom &,I) { }

	static BL is_flint(const t_atom &a) { return is_float(a); }
	static F geta_flint(const t_atom &a) { return get_float(a); }
	static V set_flint(t_atom &a,I v) { a.a_type = A_FLOAT; a.a_w.w_float = v; }
#elif defined(MAXMSP)
	static BL is_pointer(const t_atom &) { return false; }
	static V *get_pointer(const t_atom &) { return NULL; }
	static V *geta_pointer(const t_atom &) { return NULL; }
//	static V set_pointer(t_atom &,V *) {}
	static BL is_int(const t_atom &a) { return a.a_type == A_INT; }
	static I get_int(const t_atom &a) { return a.a_w.w_long; }
	static I geta_int(const t_atom &a) { return is_int(a)?get_int(a):0; }
	static V set_int(t_atom &a,I v) { a.a_type = A_INT, a.a_w.w_long = v; }

	static BL is_flint(const t_atom &a) { return is_float(a) || is_int(a); }
	static F geta_flint(const t_atom &a) { return is_float(a)?get_float(a):(is_int(a)?get_int(a):0); }
	static V set_flint(t_atom &a,I v) { a.a_type = A_INT; a.a_w.w_long = v; }
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
//			tp_def,
			tp_float,tp_flint,tp_sym,tp_list,tp_sig,tp_any
		};

		xlet(type t): tp(t),nxt(NULL) {}
		~xlet();
		
		type tp;
		xlet *nxt;
	};
	
	V AddInlet(xlet::type tp,I mult) { AddXlet(tp,mult,inlist); }
	V AddOutlet(xlet::type tp,I mult) { AddXlet(tp,mult,outlist); }

// method handling

	class methitem { 
	public:
		methitem(I inlet,t_symbol *t);
		methitem(I inlet,t_symbol *t,metharg &arg,methfun fun /*,BL ixd*/);
		~methitem();

		t_symbol *tag;
		I inlet;
		I argc;
		metharg *args;

		methfun fun;
//		BL iix; // function gets inlet index as 1st argument
	};
	
	std::list<methitem *> mlst;

private:

	xlet *inlist,*outlist;
	I incnt,outcnt,insigs,outsigs;
	outlet **outlets;

	V AddXlet(xlet::type tp,I mult,xlet *&root);	

#ifdef PD
	// proxy object (for additional inlets) stuff
	struct px_object;
	friend struct px_object;
	static V cb_px_anything(t_class *c,const t_symbol *s,I argc,t_atom *argv);
#elif defined(MAXMSP)
	typedef object px_object;
	static V cb_px_anything(t_class *c,const t_symbol *s,I argc,t_atom *argv);
	static V cb_px_int(t_class *c,I v);
	static V cb_px_float(t_class *c,F f);
	static V cb_px_bang(t_class *c);

	static V cb_px_in1(t_class *c,I v);
	static V cb_px_in2(t_class *c,I v);
	static V cb_px_in3(t_class *c,I v);
	static V cb_px_in4(t_class *c,I v);
	static V cb_px_in5(t_class *c,I v);
	static V cb_px_in6(t_class *c,I v);
	static V cb_px_in7(t_class *c,I v);
	static V cb_px_in8(t_class *c,I v);
	static V cb_px_in9(t_class *c,I v);
#endif

	static V cb_px_ft1(t_class *c,F f);
	static V cb_px_ft2(t_class *c,F f);
	static V cb_px_ft3(t_class *c,F f);
	static V cb_px_ft4(t_class *c,F f);
	static V cb_px_ft5(t_class *c,F f);
	static V cb_px_ft6(t_class *c,F f);
	static V cb_px_ft7(t_class *c,F f);
	static V cb_px_ft8(t_class *c,F f);
	static V cb_px_ft9(t_class *c,F f);

	px_object **inlets;

	// callback functions

	static V cb_help(t_class *c);

	static V cb_loadbang(t_class *c);
#ifdef MAXMSP
	static V cb_assist(t_class *c,V *b,L msg,L arg,C *s);
#endif
};



// === flext_dsp ==================================================

// dsp enabled base object

class flext_dsp:
	public flext_base
{
	FLEXT_HEADER_S(flext_dsp,flext_base)
	
public:

	// returns current sample rate
	F samplerate() const { return srate; }
	

// --- inheritable virtual methods --------------------------------

	// called on every dsp init (don't expect valid data in the signal vectors!)
	//   n: frames (aka samples) in one signal vector
	//   insigs: array of input vectors  (get number with function cnt_insig())
	//   outsigs: array of output vectors  (get number with function cnt_outsig())
	virtual V m_dsp(I n,F *const *insigs,F *const *outsigs);

	// called with every signal vector (here you do the dsp calculation)
	//   n: frames (aka samples) in one signal vector
	//   insigs: array of input vectors  (get number with function cnt_insig())
	//   outsigs: array of output vectors  (get number with function cnt_outsig())
	virtual V m_signal(I n,F *const *insigs,F *const *outsigs) = 0;

	// called with "enable" message: pauses/resumes dsp
	virtual V m_dspon(BL on);
	

// --- inlet/outlet stuff -----------------------------------------	

	// add signal inlet
	V add_in_signal(I m = 1) { AddInlet(xlet::tp_sig,m); }

	// add signal outlet
	V add_out_signal(I m = 1) { AddOutlet(xlet::tp_sig,m); }


protected:
	
	flext_dsp();
	~flext_dsp();
	
private:

	F srate;
	BL dspon;


	// callback functions

	static V cb_dsp(t_class *c,t_signal **s);
	static V cb_dspon(t_class *c,FI on);

	// dsp stuff

	static t_int *dspmeth(t_int *w); 
	F **invecs,**outvecs;
};




///////////////////////////////////////////////////////////
// These should be the used in the class definition
/////////////////////////////////////////////////////////

#define FLEXT_CALLBACK(M_FUN) \
static void cb_ ## M_FUN(flext_base *c) { dynamic_cast<thisType *>(c)->M_FUN(); }

#define FLEXT_CALLBACK_A(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,t_symbol *s,int argc,t_atom *argv) { dynamic_cast<thisType *>(c)->M_FUN(s,argc,argv); }

#define FLEXT_CALLBACK_G(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int argc,t_atom *argv) { dynamic_cast<thisType *>(c)->M_FUN(argc,argv); }

// converting t_flint to bool
#define FLEXT_CALLBACK_B(M_FUN) \
static void cb_ ## M_FUN(flext_base *c,int arg1) { dynamic_cast<thisType *>(c)->M_FUN(arg1 != 0); }

// converting t_flint to enum
#define FLEXT_CALLBACK_E(M_FUN,TPE) \
static void cb_ ## M_FUN(flext_base *c,int arg1) { dynamic_cast<thisType *>(c)->M_FUN((TPE)(int)arg1); }

#define FLEXT_CALLBACK_1(M_FUN,TP1) \
static void cb_ ## M_FUN(flext_base *c,TP1 arg1) { dynamic_cast<thisType *>(c)->M_FUN(arg1); }

#define FLEXT_CALLBACK_2(M_FUN,TP1,TP2) \
static void cb_ ## M_FUN(flext_base *c,TP1 arg1,TP2 arg2) { dynamic_cast<thisType *>(c)->M_FUN(arg1,arg2); }

#define FLEXT_CALLBACK_3(M_FUN,TP1,TP2,TP3) \
static void cb_ ## M_FUN(flext_base *c,TP1 arg1,TP2 arg2,TP3 arg3) { dynamic_cast<thisType *>(c)->M_FUN(arg1,arg2,arg3); }

#define FLEXT_CALLBACK_4(M_FUN,TP1,TP2,TP3,TP4) \
static void cb_ ## M_FUN(flext_base *c,TP1 arg1,TP2 arg2,TP3 arg3.TP4 arg4) { dynamic_cast<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4); }

#define FLEXT_CALLBACK_5(M_FUN,TP1,TP2,TP3,TP4,TP5) \
static void cb_ ## M_FUN(flext_base *c,TP1 arg1,TP2 arg2,TP3 arg3.TP4 arg4,TP5 arg5) { dynamic_cast<thisType *>(c)->M_FUN(arg1,arg2,arg3,arg4,arg5); }


// Shortcuts for method arguments
#define FLEXTARG_float a_float
#define FLEXTARG_int a_int
//#define FLEXTARG_bool a_bool
#define FLEXTARG_bool a_int
#define FLEXTARG_t_float a_float
//#define FLEXTARG_t_flint a_int
#define FLEXTARG_t_symtype a_symbol
#define FLEXTARG_t_ptrtype a_pointer

#define FLEXTARG(TP) FLEXTARG_ ## TP


///////////////////////////////////////////////////////////
// These should be the used in the class' constructor
///////////////////////////////////////////////////////////

// add handler for bang 
#define FLEXT_ADDBANG(IX,M_FUN) \
add_meth(IX,"bang",cb_ ## M_FUN)	

// add handler for method with no args
#define FLEXT_ADDMETHOD(IX,M_FUN) \
add_meth(IX,cb_ ## M_FUN)	

// add handler for method with no args
#define FLEXT_ADDMETHOD_(IX,M_TAG,M_FUN) \
add_meth(IX,M_TAG,cb_ ## M_FUN)	

// add handler for method with 1 enum type arg
#define FLEXT_ADDMETHOD_E(IX,M_TAG,M_FUN) \
add_meth_one(IX,M_TAG,(methfun)(cb_ ## M_FUN),a_int,a_null)

// add handler for method with 1 arg
#define FLEXT_ADDMETHOD_1(IX,M_TAG,M_FUN,TP1) \
add_meth_one(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),a_null)	

// add handler for method with 2 args
#define FLEXT_ADDMETHOD_2(IX,M_TAG,M_FUN,TP1,TP2) \
add_meth_one(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),a_null)

// add handler for method with 3 args
#define FLEXT_ADDMETHOD_3(IX,M_TAG,M_FUN,TP1,TP2,TP3) \
add_meth_one(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),a_null)

// add handler for method with 4 args
#define FLEXT_ADDMETHOD_4(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4) \
add_meth_one(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),a_null)

// add handler for method with 5 args
#define FLEXT_ADDMETHOD_5(IX,M_TAG,M_FUN,TP1,TP2,TP3,TP4,TP5) \
add_meth_one(IX,M_TAG,(methfun)(cb_ ## M_FUN),FLEXTARG(TP1),FLEXTARG(TP2),FLEXTARG(TP3),FLEXTARG(TP4),FLEXTARG(TP5),a_null)


#endif

