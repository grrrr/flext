/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __CPPEXT_H
#define __CPPEXT_H

#include <_cpp.h>


// ----------------------------

class ext_obj:
	public CPPExtern
{
	CPPEXTERN_HEADER(ext_obj,CPPExtern)
	
public:
	ext_obj();
	virtual ~ext_obj();
	
	virtual V m_help();
	
#ifdef MAXMSP
	virtual V m_loadbang() {}
	virtual V m_assist(L msg,L arg,C *s) = 0;
#endif	

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

	V Inlet_def() { AddInlet(xlet::tp_def,1); }
	V Inlet_float(I m = 1) { AddInlet(xlet::tp_float,m); }
	V Inlet_flint(I m = 1) { AddInlet(xlet::tp_flint,m); }
	V Inlet_signal(I m = 1) { AddInlet(xlet::tp_sig,m); }
	
	V Outlet_float(I m = 1) { AddOutlet(xlet::tp_float,m); }
	V Outlet_flint(I m = 1) { AddOutlet(xlet::tp_flint,m); }
	V Outlet_sym(I m = 1) { AddOutlet(xlet::tp_sym,m); }
	V Outlet_list(I m = 1) { AddOutlet(xlet::tp_list,m); }
	V Outlet_signal(I m = 1) { AddOutlet(xlet::tp_sig,m); }
	
	// must be called to actually set up the defined inlets/outlets
	BL SetupInOut(); 

	// get pointer _after_wards	
	t_outlet *Outlet(I ix) { return (out && ix < outcnt)?out[ix]:NULL; }

private:

	struct xlet {	
		enum type {
			tp_none = 0,tp_def,tp_float,tp_flint,tp_sym,tp_list,tp_sig
		};

		xlet(type t): tp(t),nxt(NULL) {}
		~xlet();
		
		type tp;
		xlet *nxt;
	} *inlist,*outlist;
	
	I outcnt;
	t_outlet **out;
	
	V AddXlet(xlet::type tp,I mult,xlet *&root);	
	V AddInlet(xlet::type tp,I mult) { AddXlet(tp,mult,inlist); }
	V AddOutlet(xlet::type tp,I mult) { AddXlet(tp,mult,outlist); }

	static V cb_help(V *c);

#ifdef MAXMSP
	static V cb_loadbang(V *c);
	static V cb_assist(V *c,V *b,L msg,L arg,C *s);
#endif
};


// ----------------------------


// ----------------------------

class dsp_obj:
	public ext_obj
{
	CPPEXTERN_HEADER(dsp_obj,ext_obj)
	
public:
	dsp_obj();
	
	virtual V m_dsp(t_signal **s) = 0;
	virtual V m_enable(BL on);
	
protected:
	BL enable;

private:

	static V cb_dsp(V *c,t_signal **s);
	static V cb_enable(V *c,FI on);
};


#endif
