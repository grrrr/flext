/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>


// === flext_base ============================================

BL flext_base::compatibility = true;

flext_base::flext_base():
	inlist(NULL),outlist(NULL),
	incnt(0),outcnt(0),
	insigs(0),outsigs(0),
	outlets(NULL)
{}

flext_base::~flext_base()
{
	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(outlets) delete[] outlets;
}

flext_base::xlet::~xlet() { if(nxt) delete nxt; }

V flext_base::AddXlet(xlet::type tp,I mult,xlet *&root)
{
	if(!root && mult) { root = new xlet(tp); --mult; }
	if(mult) {
		xlet *xi = root; 
		while(xi->nxt) xi = xi->nxt;
		while(mult--) xi = xi->nxt = new xlet(tp);
	}
}

BL flext_base::setup_inout()
{
	BL ok = true;
	
	incnt = insigs = 0; 

	if(inlist) {
		xlet *xi;
		incnt = 0;
		for(xi = inlist; xi; xi = xi->nxt) ++incnt;
		xlet::type *list = new xlet::type[incnt];
		I i;
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete inlist; inlist = NULL;
		
		// type info is now in list array
#ifdef PD
		{
			I ix;
			if(incnt == 0) {
				;
			}
			else if(incnt >= 1) {
				switch(list[0]) {
					case xlet::tp_def:
						break;
					case xlet::tp_sig:
						++insigs;
						break;
					default:
						error("%s: Leftmost inlet must be of type signal or default",thisName());
						ok = false;
				} 
			}		
			for(ix = 1; ix < incnt; ++ix) {
				switch(list[ix]) {
					case xlet::tp_float:
					case xlet::tp_flint: {
						C sym[] = "ft??";
						if(ix >= 10) { 
							if(compatibility) {
								// Max allows max. 9 inlets
								post("%s: Only 9 float inlets allowed in compatibility mode",thisName());
								ok = false;
							}
							else 
								sym[2] = '0'+ix/10,sym[3] = '0'+ix%10;
						}
						else 
							sym[2] = '0'+ix,sym[3] = 0;  
					    if(ok) inlet_new(x_obj, &x_obj->ob_pd, &s_float, gensym(sym)); 
						break;
					}
					case xlet::tp_sym: 
						if(compatibility) {
							post("%s: No symbol inlets (apart from leftmost) in compatibility mode",thisName());
							ok = false;
						}
					    else
					    	inlet_new(x_obj, &x_obj->ob_pd, &s_symbol, &s_symbol); 
						break;
					case xlet::tp_sig:
	    				inlet_new(x_obj, &x_obj->ob_pd, &s_signal, &s_signal);  
						++insigs;
						break;
					default:
						error("%s: Wrong type for inlet #%i",thisName(),ix);
						ok = false;
				} 
			}
		}
#elif defined(MAXMSP)
		{
			I ix;
			// count leftmost signal inlets
			while(insigs < incnt && list[insigs] == xlet::tp_sig) ++insigs;
			
			for(ix = incnt-1; ix >= insigs; --ix) {
				switch(list[ix]) {
					case xlet::tp_float:
						if(ix >= 10) { 
							post("%s: Only 9 float inlets possible",thisName());
							ok = false;
						}
						else
							floatin(x_obj,ix);  
						break;
					case xlet::tp_flint:
						if(ix >= 10) { 
							post("%s: Only 9 flint inlets possible",thisName());
							ok = false;
						}
						else
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
			
			if(insigs) {
				dsp_setup(x_obj,insigs); // signal inlets	
			}
			else {
				if(incnt && list[0] != xlet::tp_def) {
					error("%s: Leftmost inlet must be of type signal or default",thisName());
					ok = false;
				}
			}
		}
#endif
		delete[] list;
	}
	
	if(outlets) { 
		delete[] outlets; outlets = NULL; 
	}
	outcnt = outsigs = 0; 
	
	if(outlist) {
		xlet *xi;
		outcnt = 0;
		for(xi = outlist; xi; xi = xi->nxt) ++outcnt;
		xlet::type *list = new xlet::type[outcnt];
		I i;
		for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete outlist; outlist = NULL;
		
		outlets = new t_outlet *[outcnt];

		// type info is now in list array
#ifdef PD
		for(I ix = 0; ix < outcnt; ++ix) 
#elif defined(MAXMSP)
		for(I ix = outcnt-1; ix >= 0; --ix) 
#endif
		{
			switch(list[ix]) {
				case xlet::tp_float:
					outlets[ix] = newout_float(x_obj);
					break;
				case xlet::tp_flint: 
					outlets[ix] = newout_flint(x_obj);
					break;
				case xlet::tp_sig:
					outlets[ix] = newout_signal(x_obj);
					++outsigs;
					break;
				case xlet::tp_sym:
					outlets[ix] = newout_symbol(x_obj);
					break;
				case xlet::tp_list:
					outlets[ix] = newout_list(x_obj);
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


V flext_base::cb_setup(t_class *c)
{
	add_method(c,cb_help,"help");
	
	add_loadbang(c,cb_loadbang);
#ifdef MAXMSP
	add_assist(c,cb_assist);
#endif
}

V flext_base::cb_help(V *c) { thisObject(c)->m_help(); }	

V flext_base::cb_loadbang(V *c) { thisObject(c)->m_loadbang(); }	
#ifdef MAXMSP
V flext_base::cb_assist(V *c,V * /*b*/,L msg,L arg,C *s) { thisObject(c)->m_assist(msg,arg,s); }
#endif

V flext_base::m_help()
{
	// This should better be overloaded
	post("%s (using flext) - compiled on %s %s",thisName(),__DATE__,__TIME__);
}


// === flext_dsp ==============================================

V flext_dsp::cb_setup(t_class *c)
{
#ifdef PD
	CLASS_MAINSIGNALIN(c,flext_hdr,defsig);
#elif defined(MAXMSP)
	dsp_initclass();
#endif
	
	add_dsp(c,cb_dsp);
	add_method1(c,cb_enable,"enable",A_FLINT);
}

flext_dsp::flext_dsp(): 
	enable(true),
	srate(sys_getsr()),
	invecs(NULL),outvecs(NULL)
{}


flext_dsp::~flext_dsp()
{
	if(invecs) delete[] invecs;
	if(outvecs) delete[] outvecs;
}

t_int *flext_dsp::dspmeth(t_int *w) 
{ 
	flext_dsp *obj = (flext_dsp *)w[1];
	if(obj->enable) 
		obj->m_signal((I)w[2],obj->invecs,obj->outvecs); 
	return w+3;
}

V flext_dsp::m_dsp(I /*n*/,F *const * /*insigs*/,F *const * /*outsigs*/) {}

V flext_dsp::cb_dsp(V *c,t_signal **sp) 
{ 
	flext_dsp *obj = thisObject(c); 

	// store current sample rate
	obj->srate = sp[0]->s_sr;

	// store in and out signal vectors
	I i,in = obj->cnt_insig(),out = obj->cnt_outsig();
	if(obj->invecs) delete[] obj->invecs;
	obj->invecs = new F *[in];
	for(i = 0; i < in; ++i) obj->invecs[i] = sp[i]->s_vec;

	if(obj->outvecs) delete[] obj->outvecs;
	obj->outvecs = new F *[out];
	for(i = 0; i < out; ++i) obj->outvecs[i] = sp[in+i]->s_vec;

	// with the following call derived classes can do their eventual DSP setup
	obj->m_dsp(sp[0]->s_n,obj->invecs,obj->outvecs);

	// set the DSP function
	dsp_add(dspmeth,2,obj,sp[0]->s_n);  
}

V flext_dsp::cb_enable(V *c,FI on) { thisObject(c)->m_enable(on != 0); }

V flext_dsp::m_enable(BL en) { enable = en; }



