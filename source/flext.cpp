/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>

#ifdef PROXYIN
// === proxy class for flext_base ============================

#ifdef PD

static t_class *px_class;

struct flext_base::px_object  // no virtual table!
{ 
	t_object x_obj; 
	flext_base *base;
	I index;

	V init(flext_base *b,I ix) { base = b; index = ix; }
	static V px_method(px_object *c,t_symbol *s,I argc,t_atom *argv);
};


V flext_base::px_object::px_method(px_object *obj,t_symbol *s,I argc,t_atom *argv)
{
	obj->base->m_anything_n(obj->index,s,argc,argv);
}

#elif defined(MAXMSP)

V flext_base::cb_px_anything(V *c,t_symbol *s,I argc,t_atom *argv)
{
	// check if inlet allows anything (or list)
	flext_base *o = thisObject(c);
	I ci = ((flext_hdr *)o->x_obj)->curinlet;
	o->m_anything_n(ci,s,argc,argv);
}

V flext_base::cb_px_int(V *c,I v)
{
	// check if inlet allows int type
	t_atom atom;
	SETINT(&atom,v);  
	cb_px_anything(c,gensym("int"),1,&atom);
}

V flext_base::cb_px_float(V *c,F v)
{
	// check if inlet allows float type
	t_atom atom;
	SETFLOAT(&atom,v);  
	cb_px_anything(c,gensym("float"),1,&atom);
}

V flext_base::cb_px_bang(V *c)
{
	// check if inlet allows bang
	cb_px_anything(c,gensym("bang"),0,NULL);
}

#endif

#endif

// === flext_base ============================================

BL flext_base::compatibility = true;


flext_base::flext_base():
	inlist(NULL),outlist(NULL),
	incnt(0),outcnt(0),
	insigs(0),outsigs(0),
	outlets(NULL)
#ifdef PROXYIN
	,inlets(NULL)
#endif
{}

flext_base::~flext_base()
{
	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(outlets) delete[] outlets;

#ifdef PROXYIN
	if(inlets) {
		for(I ix = 0; ix < incnt; ++ix)
			if(inlets[ix]) {
#ifdef PD
				pd_free(&inlets[ix]->x_obj.ob_pd);
#elif defined(MAXMSP)
				freeobject((object *)inlets[ix]);
#endif
			}
		delete[] inlets;
	}
#endif
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

#ifdef PROXYIN
	if(inlets) { 
		for(I ix = 0; ix < incnt; ++ix) 
			if(inlets[ix]) {
#ifdef PD
				pd_free(&inlets[ix]->x_obj.ob_pd);
#elif defined(MAXMSP)
				freeobject(inlets[ix]);
#endif
			}
		delete[] inlets; 
		inlets = NULL; 
	}
#endif

	if(inlist) {
		xlet *xi;
		incnt = 0;
		for(xi = inlist; xi; xi = xi->nxt) ++incnt;
		xlet::type *list = new xlet::type[incnt];
		I i;
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete inlist; inlist = NULL;
		
#ifdef PROXYIN
		inlets = new px_object *[incnt];
		for(i = 0; i < incnt; ++i) inlets[i] = NULL;
		
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
						error("%s: Leftmost inlet must be of type default or signal",thisName());
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
					case xlet::tp_any:
					case xlet::tp_list:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(x_obj,&inlets[ix]->x_obj.ob_pd, 0, 0);  // create proxy inlet 
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
						inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  // proxy for 2nd inlet messages 
						break;
					case xlet::tp_flint:
						inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  // proxy for 2nd inlet messages 
						break;
					case xlet::tp_any:
					case xlet::tp_list:
						inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  // proxy for 2nd inlet messages 
						break;
					case xlet::tp_sig:
						error("%s: All signal inlets must be at the left side",thisName());
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

#else // PROXYIN

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
						error("%s: Leftmost inlet must be of type default or signal",thisName());
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
						error("%s: All signal inlets must be at the left side",thisName());
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

#endif // PROXYIN
		delete[] list;
	}
	
	if(outlets) { delete[] outlets; outlets = NULL; }
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
				case xlet::tp_any:
					outlets[ix] = newout_anything(x_obj);
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

#ifdef PROXYIN
	// proxy for extra inlets
#ifdef PD
    px_class = class_new(gensym("flext_base proxy"),NULL,NULL,sizeof(px_object),CLASS_PD|CLASS_NOINLET, A_NULL);
	add_anything(px_class,px_object::px_method);
#elif defined(MAXMSP) 
	add_anything(c,cb_px_anything);
	add_bang(c,cb_px_bang);
	add_method1(c,cb_px_int,"int",A_INT);
	add_method1(c,cb_px_float,"float",A_FLOAT);
#endif
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


#ifdef PROXYIN
V flext_base::m_anything_n(I inlet,t_symbol *s,I argc,t_atom *argv)
{
}
#endif


// === flext_dsp ==============================================

V flext_dsp::cb_setup(t_class *c)
{
#ifdef PD
	CLASS_MAINSIGNALIN(c,flext_hdr,defsig);
#elif defined(MAXMSP)
	dsp_initclass();
#endif
	
	add_dsp(c,cb_dsp);
	add_method1(c,cb_dspon,"dspon",A_FLINT);
}

flext_dsp::flext_dsp(): 
	dspon(true),
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
	if(obj->dspon) 
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

V flext_dsp::cb_dspon(V *c,FI on) { thisObject(c)->m_dspon(on != 0); }

V flext_dsp::m_dspon(BL en) { 
	dspon = en; 
}



