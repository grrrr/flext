/* 

flext - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __FLEXT_H
#define __FLEXT_H

#include <flbase.h> 


// ----------------------------

class flext_base:
	public flext_obj
{
	FLEXT_HEADER(flext_base,flext_obj)
	
public:
	// compatibility mode: flext allows only operations valid for all platforms
	// true by default
	static BL compatibility;  

	flext_base();
	virtual ~flext_base();
	
	// called on "help" message: should post some text
	virtual V m_help();
	
	// called on patcher load (Max/MSP only)
	virtual V m_loadbang() {}

	// quickhelp for inlets/outlets (Max/MSP only)
	virtual V m_assist(L /*msg*/,L /*arg*/,C * /*s*/) {}

	// class for platform independent buffer handling
	class buffer
	{
	public:
		buffer(t_symbol *s = NULL);
		~buffer();
		
		I Set(t_symbol *s = NULL);
		V Dirty(BL force = false);
		
		t_symbol *Symbol() { return sym; }
		const C *Name() const { return sym?sym->s_name:""; }
		F *Data() { return data; }
		I Channels() const { return chns; }
		I Frames() const { return frames; }
		
	#ifdef PD
		V SetRefrIntv(F intv) { interval = intv; }
	#else
		V SetRefrIntv(F) {}
	#endif

	protected:
		const C *thisName() const { return typeid(*this).name(); }

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

protected:

	// inlets/outlets - all (also default) inlets must be defined
	V add_in_def() { AddInlet(xlet::tp_def,1); }
	V add_in_float(I m = 1) { AddInlet(xlet::tp_float,m); }
	V add_in_flint(I m = 1) { AddInlet(xlet::tp_flint,m); }
	V add_in_symbol(I m = 1) { AddInlet(xlet::tp_sym,m); }
	
	V add_out_float(I m = 1) { AddOutlet(xlet::tp_float,m); }
	V add_out_flint(I m = 1) { AddOutlet(xlet::tp_flint,m); }
	V add_out_symbol(I m = 1) { AddOutlet(xlet::tp_sym,m); }
	V add_out_list(I m = 1) { AddOutlet(xlet::tp_list,m); }
	
	// must be called to actually set up the defined inlets/outlets 
	// only ONCE!!!
	BL setup_inout(); 

	I cnt_in() const { return incnt; }
	I cnt_out() const { return outcnt; }
	I cnt_insig() const { return insigs; }
	I cnt_outsig() const { return outsigs; }

	// get pointer _after_wards	
	t_outlet *get_out(I ix) { return (outlets && ix < outcnt)?outlets[ix]:NULL; }

	struct xlet {	
		enum type {
			tp_none = 0,tp_def,tp_float,tp_flint,tp_sym,tp_list,tp_sig
		};

		xlet(type t): tp(t),nxt(NULL) {}
		~xlet();
		
		type tp;
		xlet *nxt;
	};
	
	V AddInlet(xlet::type tp,I mult) { AddXlet(tp,mult,inlist); }
	V AddOutlet(xlet::type tp,I mult) { AddXlet(tp,mult,outlist); }

private:

	xlet *inlist,*outlist;
	I incnt,outcnt,insigs,outsigs;
	t_outlet **outlets;
	
	V AddXlet(xlet::type tp,I mult,xlet *&root);	

	static V cb_help(V *c);

#ifdef MAXMSP
	static V cb_loadbang(V *c);
	static V cb_assist(V *c,V *b,L msg,L arg,C *s);
#endif
};


// ----------------------------


// ----------------------------

class flext_dsp:
	public flext_base
{
	FLEXT_HEADER(flext_dsp,flext_base)
	
public:
	flext_dsp();
	~flext_dsp();
	
	// called on every dsp init
	virtual V m_dsp(I n,F *const *insigs,F *const *outsigs);

	// called with every signal vector
	virtual V m_signal(I n,F *const *insigs,F *const *outsigs) = 0;

	// called with "enable" message: pauses/resumes dsp
	virtual V m_enable(BL on);

	// returns current sample rate
	F samplerate() const { return srate; }
	
protected:

	// add signal inlet
	V add_in_signal(I m = 1) { AddInlet(xlet::tp_sig,m); }

	// add signal outlet
	V add_out_signal(I m = 1) { AddOutlet(xlet::tp_sig,m); }
	
private:

	F srate;
	BL enable;

	static V cb_dsp(V *c,t_signal **s);
	static V cb_enable(V *c,FI on);

	static t_int *dspmeth(t_int *w); 
	F **invecs,**outvecs;
};


#endif
