/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>
#include <stdarg.h>

// === proxy class for flext_base ============================

#ifdef PD

static t_class *px_class;

struct flext_base::px_object  // no virtual table!
{ 
	t_object x_obj; 
	flext_base *base;
	I index;

	V init(flext_base *b,I ix) { base = b; index = ix; }
	static V px_method(px_object *c,const t_symbol *s,I argc,t_atom *argv);
};


V flext_base::px_object::px_method(px_object *obj,const t_symbol *s,I argc,t_atom *argv)
{
	obj->base->m_methodmain(obj->index,s,argc,argv);
}

V flext_base::cb_px_anything(t_class *c,const t_symbol *s,I argc,t_atom *argv)
{
	thisObject(c)->m_methodmain(0,s,argc,argv);
}

#define DEF_IN_FT(IX) \
V flext_base::cb_px_ft ## IX(t_class *c,F v) { \
	t_atom atom; SETFLOAT(&atom,v);  \
	thisObject(c)->m_methodmain(IX,&s_float,1,&atom); \
}

#define ADD_IN_FT(IX) \
add_method1(c,cb_px_ft ## IX,"ft" #IX,A_FLOAT)

#elif defined(MAXMSP)

V flext_base::cb_px_anything(t_class *c,const t_symbol *s,I argc,t_atom *argv)
{
	// check if inlet allows anything (or list)
	
	flext_base *o = thisObject(c);
	I ci = ((flext_hdr *)o->x_obj)->curinlet;
	o->m_methodmain(ci,s,argc,argv);
}

V flext_base::cb_px_int(t_class *c,I v)
{
	// check if inlet allows int type
	t_atom atom;
	SETINT(&atom,v);  
	cb_px_anything(c,sym_int,1,&atom);
}

V flext_base::cb_px_float(t_class *c,F v)
{
	// check if inlet allows float type
	t_atom atom;
	SETFLOAT(&atom,v);  
	cb_px_anything(c,sym_float,1,&atom);
}

V flext_base::cb_px_bang(t_class *c)
{
	// check if inlet allows bang
	cb_px_anything(c,sym_bang,0,NULL);
}

#define DEF_IN_FT(IX) \
V flext_base::cb_px_in ## IX(t_class *c,I v) { L &ci = ((flext_hdr *)thisObject(c)->x_obj)->curinlet; ci = IX; cb_px_int(c,v); ci = 0; } \
V flext_base::cb_px_ft ## IX(t_class *c,F v) { L &ci = ((flext_hdr *)thisObject(c)->x_obj)->curinlet; ci = IX; cb_px_float(c,v); ci = 0; }

#define ADD_IN_FT(IX) \
add_method1(c,cb_px_in ## IX,"in" #IX,A_INT); \
add_method1(c,cb_px_ft ## IX,"ft" #IX,A_FLOAT)

#endif // MAXMSP


DEF_IN_FT(1)
DEF_IN_FT(2)
DEF_IN_FT(3)
DEF_IN_FT(4)
DEF_IN_FT(5)
DEF_IN_FT(6)
DEF_IN_FT(7)
DEF_IN_FT(8)
DEF_IN_FT(9)



// === flext_base ============================================

BL flext_base::compatibility = true;


const t_symbol *flext_base::sym_float;
const t_symbol *flext_base::sym_symbol;
const t_symbol *flext_base::sym_bang;
const t_symbol *flext_base::sym_list;
const t_symbol *flext_base::sym_anything;

#ifdef PD
const t_symbol *flext_base::sym_pointer;
const t_symbol *flext_base::sym_signal;
#endif
#ifdef MAXMSP
const t_symbol *flext_base::sym_int;
#endif


flext_base::flext_base():
	inlist(NULL),outlist(NULL),
	incnt(0),outcnt(0),
	insigs(0),outsigs(0),
	outlets(NULL),inlets(NULL)
{
	LOG("Logging is on");
}

flext_base::~flext_base()
{
	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(outlets) delete[] outlets;

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

	// do i have to destroy the method list elements?
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

	if(inlist) {
		xlet *xi;
		incnt = 0;
		for(xi = inlist; xi; xi = xi->nxt) ++incnt;
		xlet::type *list = new xlet::type[incnt];
		I i;
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete inlist; inlist = NULL;
		
		inlets = new px_object *[incnt];
		for(i = 0; i < incnt; ++i) inlets[i] = NULL;
		
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
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(x_obj,&inlets[ix]->x_obj.ob_pd, &s_symbol, &s_symbol);  
//				    	inlet_new(x_obj, &x_obj->ob_pd, &s_symbol, &s_symbol); 
						break;
					case xlet::tp_list:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(x_obj,&inlets[ix]->x_obj.ob_pd, &s_list, &s_list);  
						break;
					case xlet::tp_any:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(x_obj,&inlets[ix]->x_obj.ob_pd, 0, 0);  
						break;
					case xlet::tp_sig:
	    				inlet_new(x_obj, &x_obj->ob_pd, &s_signal, &s_signal);  
						++insigs;
						break;
					default:
						error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(I)list[ix]);
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
					case xlet::tp_sig:
						error("%s: All signal inlets must be at the left side",thisName());
						ok = false;
						break;
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
					case xlet::tp_sym:
					case xlet::tp_any:
					case xlet::tp_list:
						inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  
						break;
					case xlet::tp_def:
						if(ix) error("%s: Default inlet type reserved for inlet #0",thisName());
						break;
					default:
						error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(I)list[ix]);
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
#ifdef PD
	sym_anything = &s_anything;
	sym_pointer = &s_gpointer;
	sym_float = &s_float;
	sym_symbol = &s_symbol;
	sym_bang = &s_bang;
	sym_list = &s_list;
	sym_signal = &s_signal;
#elif defined(MAXMSP)
	sym_int = gensym("int");
	sym_float = gensym("float");
	sym_symbol = gensym("symbol");
	sym_bang = gensym("bang");
	sym_list = gensym("list");
	sym_anything = gensym("anything");
#endif

	add_method(c,cb_help,"help");
	
	add_loadbang(c,cb_loadbang);
#ifdef MAXMSP
	add_assist(c,cb_assist);
#endif

	// proxy for extra inlets
#ifdef PD
	add_anything(c,cb_px_anything); // for leftmost inlet
    px_class = class_new(gensym("flext_base proxy"),NULL,NULL,sizeof(px_object),CLASS_PD|CLASS_NOINLET, A_NULL);
	add_anything(px_class,px_object::px_method); // for other inlets
#elif defined(MAXMSP) 
	add_bang(c,cb_px_bang);
	add_method1(c,cb_px_int,"int",A_INT);  
	add_method1(c,cb_px_float,"float",A_FLOAT);  
	add_methodG(c,cb_px_anything,"list");  
	add_anything(c,cb_px_anything);
#endif	

	// setup non-leftmost ints and floats
	ADD_IN_FT(1);
	ADD_IN_FT(2);
	ADD_IN_FT(3);
	ADD_IN_FT(4);
	ADD_IN_FT(5);
	ADD_IN_FT(6);
	ADD_IN_FT(7);
	ADD_IN_FT(8);
	ADD_IN_FT(9);
}

V flext_base::cb_help(t_class *c) { thisObject(c)->m_help(); }	

V flext_base::cb_loadbang(t_class *c) { thisObject(c)->m_loadbang(); }	
#ifdef MAXMSP
V flext_base::cb_assist(t_class *c,V * /*b*/,L msg,L arg,C *s) { thisObject(c)->m_assist(msg,arg,s); }
#endif

V flext_base::m_help()
{
	// This should better be overloaded
	post("%s (using flext " FLEXT_VERSTR ") - compiled on %s %s",thisName(),__DATE__,__TIME__);
}


typedef V (*methfun_G)(flext_base *c,I argc,t_atom *argv);
typedef V (*methfun_A)(flext_base *c,const t_symbol *s,I argc,t_atom *argv);
typedef V (*methfun_0)(flext_base *c);

#define MAXARGS 5

// the args MUST all have 32 bits (float, t_symbol *)
typedef V (*methfun_1)(flext_base *c,I);
typedef V (*methfun_2)(flext_base *c,I,I);
typedef V (*methfun_3)(flext_base *c,I,I,I);
typedef V (*methfun_4)(flext_base *c,I,I,I,I);
typedef V (*methfun_5)(flext_base *c,I,I,I,I,I);


BL flext_base::m_methodmain(I inlet,const t_symbol *s,I argc,t_atom *argv)
{
	BL ret = false;
	
	LOG3("methodmain inlet:%i args:%i symbol:%s",inlet,argc,s?s->s_name:"");
	
	std::list<methitem *>::const_iterator it(mlst.begin());
	while(!ret) {
		const methitem *m = *it;
		if(!m) break;
		if(m->tag == sym_anything && m->argc == 1 && m->args[0] == a_gimme) {
			// any
			LOG4("found any method for %s: inlet=%i, symbol=%s, argc=%i",m->tag->s_name,inlet,s->s_name,argc);

			((methfun_A)m->fun)(this,s,argc,argv);
			ret = true;
		}
		else
		if(m->tag == s && (inlet == m->inlet || m->inlet < 0 )) {
			// tag fits
			LOG4("found method tag %s: inlet=%i, symbol=%s, argc=%i",m->tag->s_name,inlet,s->s_name,argc);

			if(m->argc == 1 && m->args[0] == a_gimme) {
				((methfun_G)m->fun)(this,argc,argv);
				ret = true;
			}
			else if(argc == m->argc) {
				I ix,iargs[MAXARGS];
				BL ok = true;
				for(ix = 0; ix < argc && ok; ++ix) {
					switch(m->args[ix]) {
					case a_float: {
						ASSERT(sizeof(F) == sizeof(I));

						F a;
						if(is_float(argv[ix])) iargs[ix] = *(I *)&(a = get_float(argv[ix]));
						else if(is_int(argv[ix])) iargs[ix] = *(I *)&(a = get_int(argv[ix]));
						else ok = false;
						break;
					}
					case a_int: {
						I a;
						if(is_float(argv[ix])) iargs[ix] = *(I *)&(a = get_float(argv[ix]));
						else if(is_int(argv[ix])) iargs[ix] = *(I *)&(a = get_int(argv[ix]));
						else ok = false;
						break;
					}
/*
					case a_bool: {
						I a;
						if(ISFLOAT(argv[ix])) iargs[ix] = *(I *)&(a = (argv[ix].a_w.w_float != 0));
#ifdef MAXMSP
						else if(ISINT(argv[ix])) iargs[ix] = *(I *)&(a = (argv[ix].a_w.w_int != 0));
#endif
						else ok = false;
						break;
					}
*/
					case a_symbol: {
						ASSERT(sizeof(t_symbol *) == sizeof(I));

						t_symbol *a;
						if(is_symbol(argv[ix])) iargs[ix] = *(I *)&(a = get_symbol(argv[ix]));
						else ok = false;
						break;
					}
#ifdef PD					
					case a_pointer: {
						ASSERT(sizeof(t_gpointer *) == sizeof(I));

						t_gpointer *a;
						if(is_pointer(argv[ix])) iargs[ix] = *(I *)&(a = get_pointer(argv[ix]));
						else ok = false;
						break;
					}
#endif
					default:
						error("Argument type illegal");
						ok = false;
					}
				}

				if(ix == argc) {
					switch(argc) {
					case 0:	((methfun_0)m->fun)(this); break;
					case 1:	((methfun_1)m->fun)(this,iargs[0]); break;
					case 2:	((methfun_2)m->fun)(this,iargs[0],iargs[1]); break;
					case 3:	((methfun_3)m->fun)(this,iargs[0],iargs[1],iargs[2]); break;
					case 4:	((methfun_4)m->fun)(this,iargs[0],iargs[1],iargs[2],iargs[3]); break;
					case 5:	((methfun_5)m->fun)(this,iargs[0],iargs[1],iargs[2],iargs[3],iargs[4]); break;
					}
					ret = true;	
				}
			}
		} 
		if(it == mlst.end()) break;
		else ++it;
	}
	
	if(!ret) LOG("Did not find any method tag");
	
	return ret; // true if appropriate handler was found and called
}


flext_base::methitem::methitem(I in,t_symbol *t): 
	inlet(in),tag(t),
	fun(NULL), //iix(false),
	argc(0),args(NULL)
{}

flext_base::methitem::methitem(I in,t_symbol *t,metharg &argl,methfun f /*,BL ixd*/): 
	inlet(in),tag(t),
	fun(f), //iix(ixd),
	argc(0),args(NULL)
{ 
	va_list marker;
	va_start(marker,argl);
	argc = 0;
	metharg arg = argl;
	for(; arg != a_null; ++argc) arg = va_arg(marker,metharg);
	va_end(marker);
	
	if(argc > 0) {
		if(argc > MAXARGS) {
			error("Only %i arguments are type-checkable: use GIMME for more",MAXARGS);
			argc = MAXARGS;
		}

		args = new metharg[argc];

		va_start(marker,argl);
		metharg a = argl;
		for(I ix = 0; ix < argc; ++ix) {
			if(a == a_gimme && ix > 0) {
				error("GIMME argument must be the first and only one");
			}
#ifdef PD
			if(a == a_pointer && flext_base::compatibility) {
				post("Pointer arguments are not allowed in compatibility mode"); 
			}
#endif
			args[ix] = a;
			a = va_arg(marker,metharg);
		}
		va_end(marker);
	}
}

flext_base::methitem::~methitem() { if(args) delete[] args; }

V flext_base::add_meth_def(I inlet)
{
	mlst.push_back(new methitem(inlet,NULL));
}

V flext_base::add_meth_def(I inlet,const C *tag)
{
	mlst.push_back(new methitem(inlet,gensym(const_cast<C *>(tag))));
}



V flext_base::add_meth_one(I inlet,const C *tag,methfun fun,metharg tp,...)
{
	mlst.push_back(new methitem(inlet,gensym(const_cast<C *>(tag)),tp,fun /*,false*/));
}

/*
V flext_base::add_meth_ixd(I inlet,const C *tag,methfun fun,metharg tp,...)
{
	mlst.push_back(new methitem(inlet,gensym(const_cast<C *>(tag)),tp,fun,true));
}
*/


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

V flext_dsp::cb_dsp(t_class *c,t_signal **sp) 
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

V flext_dsp::cb_dspon(t_class *c,FI on) { thisObject(c)->m_dspon(on != 0); }

V flext_dsp::m_dspon(BL en) { dspon = en; }



