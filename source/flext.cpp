/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <_cppext.h>

// -- ext_obj --------------------------

ext_obj::ext_obj():
	inlist(NULL),outlist(NULL),
	out(NULL),outcnt(0)
{}

ext_obj::~ext_obj()
{
	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(out) delete[] out;
}

ext_obj::xlet::~xlet() { if(nxt) delete nxt; }

V ext_obj::AddXlet(xlet::type tp,I mult,xlet *&root)
{
	if(!root && mult) { root = new xlet(tp); --mult; }
	if(mult) {
		xlet *xi = root; 
		while(xi->nxt) xi = xi->nxt;
		while(mult--) xi = xi->nxt = new xlet(tp);
	}
}

BL ext_obj::SetupInOut()
{
	BL ok = true;
	
	if(inlist) {
		xlet *xi;
		I i,cnt = 0;
		for(xi = inlist; xi; xi = xi->nxt) ++cnt;
		xlet::type *list = new xlet::type[cnt];
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete inlist; inlist = NULL;
		
		// type info is now in list array
#ifdef PD
		{
			I ix;
			if(cnt == 0) {
				;
			}
			else if(cnt >= 1) {
				switch(list[0]) {
					case xlet::tp_def:
						break;
					case xlet::tp_sig:
						CLASS_MAINSIGNALIN(thisClass(), Obj_header, defsig);
						break;
					default:
						error("%s: Leftmost inlet must be of type signal or default",thisName());
						ok = false;
				} 
			}		
			for(ix = 1; ix < cnt; ++ix) {
				switch(list[ix]) {
					case xlet::tp_float:
					case xlet::tp_flint: {
						C sym[] = "ft?"; 
						sym[2] = '0'+ix;  // what if ix > 9????
					    inlet_new(x_obj, &x_obj->ob_pd, &s_float, gensym(sym)); 
						break;
					}
					case xlet::tp_sig:
	    				inlet_new(x_obj, &x_obj->ob_pd, &s_signal, &s_signal);  
						break;
					default:
						error("%s: Wrong type for inlet #%i",thisName(),ix);
						ok = false;
				} 
			}
		}
#elif defined(MAXMSP)
		{
			I ix,sigs = 0;
			// count leftmost signal inlets
			while(sigs < cnt && list[sigs] == xlet::tp_sig) ++sigs;
			
			for(ix = cnt-1; ix >= sigs; --ix) {
				switch(list[ix]) {
					case xlet::tp_float:
						floatin(x_obj,ix);  
						break;
					case xlet::tp_flint:
						intin(x_obj,ix);  
						break;
					case xlet::tp_sig:
						error("%s: Signal inlets must be at the left side",thisName());
						ok = false;
						break;
					default:
						error("%s: Wrong type for inlet #%i",thisName(),ix);
						ok = false;
				} 
			}
			
			if(sigs) {
				dsp_setup(x_obj,sigs); // signal inlets	
			}
			else {
				if(cnt && list[0] != xlet::tp_def) {
					error("%s: Leftmost inlet must be of type signal or default",thisName());
					ok = false;
				}
			}
		}
#endif
		delete[] list;
	}
	
	if(out) { delete[] out; out = NULL; outcnt = 0; }
	
	if(outlist) {
		xlet *xi;
		outcnt = 0;
		for(xi = outlist; xi; xi = xi->nxt) ++outcnt;
		xlet::type *list = new xlet::type[outcnt];
		I i;
		for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete outlist; outlist = NULL;
		
		out = new t_outlet *[outcnt];

		// type info is now in list array
#ifdef PD
		for(I ix = 0; ix < outcnt; ++ix) 
#elif defined(MAXMSP)
		for(I ix = outcnt-1; ix >= 0; --ix) 
#endif
		{
			switch(list[ix]) {
				case xlet::tp_float:
					out[ix] = newout_float(x_obj);
					break;
				case xlet::tp_flint: 
					out[ix] = newout_flint(x_obj);
					break;
				case xlet::tp_sig:
					out[ix] = newout_signal(x_obj);
					break;
				case xlet::tp_sym:
					out[ix] = newout_symbol(x_obj);
					break;
				case xlet::tp_list:
					out[ix] = newout_list(x_obj);
					break;
				default:
					error("%s: Wrong type for outlet #%i",thisName(),ix);
					ok = false;
			} 
		}
		
		delete[] list;
	}
	
	return ok;
}



V ext_obj::cb_setup(t_class *c)
{
	add_method0(c,cb_help,"help");
	
#ifdef MAXMSP
	add_loadbang(c,cb_loadbang);
	add_assist(c,cb_assist);
#endif
}

V ext_obj::cb_help(V *c) { thisObject(c)->m_help(); }	

#ifdef MAXMSP
V ext_obj::cb_loadbang(V *c) { thisObject(c)->m_loadbang(); }	
V ext_obj::cb_assist(V *c,V *b,L msg,L arg,C *s) { thisObject(c)->m_assist(msg,arg,s); }
#endif

V ext_obj::m_help()
{
	post("Loaded object '%s' - compiled on %s %s",thisName(),__DATE__,__TIME__);
}

// -- dsp_obj --------------------------

V dsp_obj::cb_setup(t_class *c)
{
	enable_signal(c);
	
	add_dsp(c,cb_dsp);
	add_method1(c,cb_enable,"enable",A_FLINT);
}

dsp_obj::dsp_obj(): enable(true) {}

V dsp_obj::cb_dsp(V *c,t_signal **s) { thisObject(c)->m_dsp(s); }
V dsp_obj::cb_enable(V *c,FI on) { thisObject(c)->m_enable(on != 0); }

V dsp_obj::m_enable(BL en) { enable = en; }

