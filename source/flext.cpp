/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>
#include <flinternal.h>
#include <stdarg.h>
#include <string.h>

// === proxy class for flext_base ============================

#ifdef PD

static t_class *px_class;

struct flext_base::px_object  // no virtual table!
{ 
	t_object obj;			// MUST reside at memory offset 0
	flext_base *base;
	int index;

	void init(flext_base *b,int ix) { base = b; index = ix; }
	static void px_method(px_object *c,const t_symbol *s,int argc,t_atom *argv);
};


void flext_base::px_object::px_method(px_object *obj,const t_symbol *s,int argc,t_atom *argv)
{
	obj->base->m_methodmain(obj->index,s,argc,argv);
}

void flext_base::cb_px_anything(t_class *c,const t_symbol *s,int argc,t_atom *argv)
{
	thisObject(c)->m_methodmain(0,s,argc,argv);
}

#define DEF_IN_FT(IX) \
void flext_base::cb_px_ft ## IX(t_class *c,float v) { \
	t_atom atom; SETFLOAT(&atom,v);  \
	thisObject(c)->m_methodmain(IX,&s_float,1,&atom); \
}

#define ADD_IN_FT(IX) \
add_method1(c,cb_px_ft ## IX,"ft" #IX,A_FLOAT)

#elif defined(MAXMSP)

void flext_base::cb_px_anything(t_class *c,const t_symbol *s,int argc,t_atom *argv)
{
	// check if inlet allows anything (or list)
	
	flext_base *o = thisObject(c);
	int ci = ((flext_hdr *)o->x_obj)->curinlet;

	o->m_methodmain(ci,s,argc,argv);
}

void flext_base::cb_px_int(t_class *c,int v)
{
	// check if inlet allows int type
	t_atom atom;
	SETINT(&atom,v);  
	cb_px_anything(c,sym_int,1,&atom);
}

void flext_base::cb_px_float(t_class *c,float v)
{
	// check if inlet allows float type
	t_atom atom;
	SETFLOAT(&atom,v);  
	cb_px_anything(c,sym_float,1,&atom);
}

void flext_base::cb_px_bang(t_class *c)
{
	// check if inlet allows bang
	cb_px_anything(c,sym_bang,0,NULL);
}


#define DEF_IN_FT(IX) \
void flext_base::cb_px_in ## IX(t_class *c,int v) { long &ci = ((flext_hdr *)thisObject(c)->x_obj)->curinlet; ci = IX; cb_px_int(c,v); ci = 0; } \
void flext_base::cb_px_ft ## IX(t_class *c,float v) { long &ci = ((flext_hdr *)thisObject(c)->x_obj)->curinlet; ci = IX; cb_px_float(c,v); ci = 0; }

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

bool flext_base::compatibility = true;


const t_symbol *flext_base::sym_float = NULL;
const t_symbol *flext_base::sym_symbol = NULL;
const t_symbol *flext_base::sym_bang = NULL;
const t_symbol *flext_base::sym_list = NULL;
const t_symbol *flext_base::sym_anything = NULL;
const t_symbol *flext_base::sym_pointer = NULL;
const t_symbol *flext_base::sym_int = NULL;

#ifdef PD
const t_symbol *flext_base::sym_signal = NULL;
#endif

const t_symbol *flext_base::MakeSymbol(const char *s) { return gensym(const_cast<char *>(s)); }



flext_base::flext_base():
	inlist(NULL),outlist(NULL),
	incnt(0),outcnt(0),
	insigs(0),outsigs(0),
	outlets(NULL),inlets(NULL),
	mlst(NULL),
	distmsgs(false)
{
	LOG("Logging is on");
}

flext_base::~flext_base()
{
	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(outlets) delete[] outlets;

	if(inlets) {
		for(int ix = 0; ix < incnt; ++ix)
			if(inlets[ix]) {
#ifdef PD
				pd_free(&inlets[ix]->obj.ob_pd);
#elif defined(MAXMSP)
				freeobject((object *)inlets[ix]);
#endif
			}
		delete[] inlets;
	}

	if(mlst) delete mlst;
}

flext_base::xlet::xlet(type t,const char *d): 
	tp(t),nxt(NULL)
{ 
	if(d) {
		int ln = strlen(d);
		desc = new char[ln];
		strncpy(desc,d,ln);
	}
	else desc = NULL;
}

flext_base::xlet::~xlet() 
{ 
	if(desc) delete[] desc;
	if(nxt) delete nxt; 
}

void flext_base::AddXlet(xlet::type tp,int mult,const char *desc,xlet *&root)
{
	if(!root && mult) { root = new xlet(tp,desc); --mult; }
	if(mult) {
		xlet *xi = root; 
		while(xi->nxt) xi = xi->nxt;
		while(mult--) xi = xi->nxt = new xlet(tp,desc);
	}
}

unsigned long flext_base::XletCode(xlet::type tp,...)
{
	unsigned long code = 0;

	va_list marker;
	va_start(marker,tp);
	int cnt = 0;
	xlet::type *args = NULL,arg = tp;
	for(; arg; ++cnt) {
		code = code*10+(int)arg;
		arg = (xlet::type)va_arg(marker,int);
	}
	va_end(marker);

	return code;
}

void flext_base::AddInlets(unsigned long code) 
{ 
	for(; code; code /= 10) AddInlet((xlet::type)(code%10));
}

void flext_base::AddOutlets(unsigned long code) 
{ 
	for(; code; code /= 10) AddOutlet((xlet::type)(code%10));
}


#ifndef FLEXT_THREADS
void flext_base::ToOutBang(outlet *o) { outlet_bang((t_outlet *)o); }
void flext_base::ToOutFloat(outlet *o,float f) { outlet_float((t_outlet *)o,f); }
void flext_base::ToOutInt(outlet *o,int f) { outlet_flint((t_outlet *)o,f); }
void flext_base::ToOutSymbol(outlet *o,const t_symbol *s) { outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(s)); }
void flext_base::ToOutList(outlet *o,int argc,t_atom *argv) { outlet_list((t_outlet *)o,gensym("list"),argc,argv); }
void flext_base::ToOutAnything(outlet *o,const t_symbol *s,int argc,t_atom *argv) { outlet_anything((t_outlet *)o,const_cast<t_symbol *>(s),argc,argv); }
#else
void flext_base::ToOutBang(outlet *o) { if(IsSystemThread()) outlet_bang((t_outlet *)o); else QueueBang(o); }
void flext_base::ToOutFloat(outlet *o,float f) { if(IsSystemThread()) outlet_float((t_outlet *)o,f); else QueueFloat(o,f); }
void flext_base::ToOutInt(outlet *o,int f) { if(IsSystemThread()) outlet_flint((t_outlet *)o,f); else QueueInt(o,f); }
void flext_base::ToOutSymbol(outlet *o,const t_symbol *s) { if(IsSystemThread()) outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(s)); else QueueSymbol(o,s); }
void flext_base::ToOutList(outlet *o,int argc,t_atom *argv) { if(IsSystemThread()) outlet_list((t_outlet *)o,gensym("list"),argc,argv); else QueueList(o,argc,argv); }
void flext_base::ToOutAnything(outlet *o,const t_symbol *s,int argc,t_atom *argv) { if(IsSystemThread()) outlet_anything((t_outlet *)o,const_cast<t_symbol *>(s),argc,argv); else QueueAnything(o,s,argc,argv); }
#endif

bool flext_base::SetupInOut()
{
	bool ok = true;
	
	incnt = insigs = 0; 

	if(inlets) { 
		for(int ix = 0; ix < incnt; ++ix) 
			if(inlets[ix]) {
#ifdef PD
				pd_free(&inlets[ix]->obj.ob_pd);
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
		int i;
		for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete inlist; inlist = NULL;
		
		inlets = new px_object *[incnt];
		for(i = 0; i < incnt; ++i) inlets[i] = NULL;
		
		// type info is now in list array
#ifdef PD
		{
			int cnt = 0;

			if(incnt >= 1) {
#if 1
				switch(list[0]) {
					case xlet::tp_sig:
						++insigs; ++cnt;
						break;
					default:
						// leftmost inlet is already there...
						++cnt;
				} 
#else
				switch(list[0]) {
					case xlet::tp_any:
						// leftmost inlet is already there...
						++cnt;
						break;
					case xlet::tp_sig:
						++insigs; ++cnt;
						break;
					default:
						error("%s: Leftmost inlet must be of type anything or signal",thisName());
						ok = false;
				} 
#endif
			}		

			for(int ix = 1; ix < incnt; ++ix,++cnt) {
				switch(list[ix]) {
					case xlet::tp_float:
					case xlet::tp_int: {
						char sym[] = "ft??";
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
					    if(ok) inlet_new(&x_obj->obj, &x_obj->obj.ob_pd, &s_float, gensym(sym)); 
						break;
					}
					case xlet::tp_sym: 
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, &s_symbol, &s_symbol);  
						break;
					case xlet::tp_list:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, &s_list, &s_list);  
						break;
					case xlet::tp_any:
					    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
						inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, 0, 0);  
						break;
					case xlet::tp_sig:
						if(compatibility && list[ix-1] != xlet::tp_sig) {
							post("%s: All signal inlets must be left-aligned in compatibility mode",thisName());
							ok = false;
						}
						else {
							// pd doesn't seem to be able to handle signals and messages into the same inlet...
							
							inlet_new(&x_obj->obj, &x_obj->obj.ob_pd, &s_signal, &s_signal);  
							++insigs;
						}
						break;
					default:
						error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
						ok = false;
				} 
			}

			incnt = cnt;
		}
#elif defined(MAXMSP)
		{
			int ix,cnt;
			// count leftmost signal inlets
			while(insigs < incnt && list[insigs] == xlet::tp_sig) ++insigs;
			
			for(cnt = 0,ix = incnt-1; ix >= insigs; --ix,++cnt) {
				if(ix == 0) {
					if(list[ix] != xlet::tp_any) {
						error("%s: Leftmost inlet must be of type signal or default",thisName());
						ok = false;
					}
				}
				else {
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
						case xlet::tp_int:
							if(ix >= 10) { 
								post("%s: Only 9 int inlets possible",thisName());
								ok = false;
							}
							else
								intin(x_obj,ix);  
							break;
						case xlet::tp_any: // non-leftmost
						case xlet::tp_sym:
						case xlet::tp_list:
							inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  
							break;
	/*
						case xlet::tp_def:
							if(ix) error("%s: Default inlet type reserved for inlet #0",thisName());
							break;
	*/
						default:
							error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
							ok = false;
					} 
				}
			}

			incnt = cnt;
	
			if(insigs) dsp_setup(thisHdr(),insigs); // signal inlets	
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
		int i;
		for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete outlist; outlist = NULL;
		
		outlets = new outlet *[outcnt];

		// type info is now in list array
#ifdef PD
		for(int ix = 0; ix < outcnt; ++ix) 
#elif defined(MAXMSP)
		for(int ix = outcnt-1; ix >= 0; --ix) 
#endif
		{
			switch(list[ix]) {
				case xlet::tp_float:
					outlets[ix] = (outlet *)newout_float(&x_obj->obj);
					break;
/*
				case xlet::tp_flint: 
					outlets[ix] = (outlet *)newout_flint(&x_obj->obj);
					break;
*/
				case xlet::tp_int: 
					outlets[ix] = (outlet *)newout_flint(&x_obj->obj);
					break;
				case xlet::tp_sig:
					outlets[ix] = (outlet *)newout_signal(&x_obj->obj);
					++outsigs;
					break;
				case xlet::tp_sym:
					outlets[ix] = (outlet *)newout_symbol(&x_obj->obj);
					break;
				case xlet::tp_list:
					outlets[ix] = (outlet *)newout_list(&x_obj->obj);
					break;
				case xlet::tp_any:
					outlets[ix] = (outlet *)newout_anything(&x_obj->obj);
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


void flext_base::Setup(t_class *c)
{
#ifdef PD
	sym_anything = &s_anything;
	sym_pointer = &s_pointer;
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

void flext_base::cb_help(t_class *c) { thisObject(c)->m_help(); }	

void flext_base::cb_loadbang(t_class *c) { thisObject(c)->m_loadbang(); }	
#ifdef MAXMSP
void flext_base::cb_assist(t_class *c,void * /*b*/,long msg,long arg,char *s) { thisObject(c)->m_assist(msg,arg,s); }
#endif

void flext_base::m_help()
{
	// This should better be overloaded
	post("%s (using flext " FLEXT_VERSTR ") - compiled on %s %s",thisName(),__DATE__,__TIME__);
}


union t_any {
	float ft;
	int it;
	t_symbol *st;
#ifdef PD
	t_gpointer *pt;
#endif
};

typedef void (*methfun_G)(flext_base *c,int argc,t_atom *argv);
typedef void (*methfun_XG)(flext_base *c,const t_symbol *s,int argc,t_atom *argv);
typedef void (*methfun_0)(flext_base *c);

#define MAXARGS 5

typedef void (*methfun_1)(flext_base *c,t_any &);
typedef void (*methfun_2)(flext_base *c,t_any &,t_any &);
typedef void (*methfun_3)(flext_base *c,t_any &,t_any &,t_any &);
typedef void (*methfun_4)(flext_base *c,t_any &,t_any &,t_any &,t_any &);
typedef void (*methfun_5)(flext_base *c,t_any &,t_any &,t_any &,t_any &,t_any &);

bool flext_base::m_methodmain(int inlet,const t_symbol *s,int argc,t_atom *argv)
{
	bool ret = false;
	
	LOG3("methodmain inlet:%i args:%i symbol:%s",inlet,argc,s?s->s_name:"");
	
	for(const methitem *m = mlst; m && !ret; m = m->nxt) {
		if(m->tag == s && (inlet == m->inlet || m->inlet < 0 )) {
			// tag fits
			LOG4("found method tag %s: inlet=%i, symbol=%s, argc=%i",m->tag->s_name,inlet,s->s_name,argc);

			if(m->argc == 1 && m->args[0] == a_gimme) {
				((methfun_G)m->fun)(this,argc,argv);
				ret = true;
			}
			else if(m->argc == 1 && m->args[0] == a_xgimme) {
				((methfun_XG)m->fun)(this,s,argc,argv);
				ret = true;
			}
			else if(argc == m->argc) {
				int ix;
				t_any aargs[MAXARGS];
				bool ok = true;
				for(ix = 0; ix < argc && ok; ++ix) {
					switch(m->args[ix]) {
					case a_float: {
						if(IsFloat(argv[ix])) aargs[ix].ft = GetFloat(argv[ix]);
						else if(IsInt(argv[ix])) aargs[ix].ft = (float)GetInt(argv[ix]);
						else ok = false;
						
//						if(ok) LOG2("int arg %i = %f",ix,aargs[ix].ft);
						break;
					}
					case a_int: {
						if(IsFloat(argv[ix])) aargs[ix].it = (int)GetFloat(argv[ix]);
						else if(IsInt(argv[ix])) aargs[ix].it = GetInt(argv[ix]);
						else ok = false;
						
//						if(ok) LOG2("float arg %i = %i",ix,aargs[ix].it);
						break;
					}
					case a_symbol: {
						if(IsSymbol(argv[ix])) aargs[ix].st = GetSymbol(argv[ix]);
						else ok = false;
						
//						if(ok) LOG2("symbol arg %i = %s",ix,get_string(aargs[ix].st));
						break;
					}
#ifdef PD					
					case a_pointer: {
						if(IsPointer(argv[ix])) aargs[ix].pt = GetPointer(argv[ix]);
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
					case 1:	((methfun_1)m->fun)(this,aargs[0]); break;
					case 2:	((methfun_2)m->fun)(this,aargs[0],aargs[1]); break;
					case 3:	((methfun_3)m->fun)(this,aargs[0],aargs[1],aargs[2]); break;
					case 4:	((methfun_4)m->fun)(this,aargs[0],aargs[1],aargs[2],aargs[3]); break;
					case 5:	((methfun_5)m->fun)(this,aargs[0],aargs[1],aargs[2],aargs[3],aargs[4]); break;
					}
					ret = true;	
				}
			}
		} 
		else if(m->tag == sym_anything && (inlet == m->inlet || m->inlet < 0) && m->argc == 1 && m->args[0] == a_xgimme) {
			// any
			LOG4("found any method for %s: inlet=%i, symbol=%s, argc=%i",m->tag->s_name,inlet,s->s_name,argc);

			((methfun_XG)m->fun)(this,s,argc,argv);
			ret = true;
		}
	}
	
	// If float or int message is not explicitly handled: try list handler instead
	if(!ret && argc == 1 && (s == sym_float 
#ifdef MAXMSP
		|| s == sym_int
#endif
	)) {
		t_atom list;
		if(s == sym_float) 
			SetFloat(list,GetFloat(argv[0]));
#ifdef MAXMSP
		else 
			SetInt(list,GetInt(argv[0]));
#endif
		ret = m_methodmain(inlet,sym_list,1,&list);
	}


	// if distmsgs is switched on then distribute list elements over inlets (Max/MSP behavior)
	if(!ret && distmsgs && inlet == 0 && s == sym_list && insigs <= 1) {
		int i = incnt;
		if(i > argc) i = argc;
		for(--i; i >= 0; --i) { // right to left distribution
			const t_symbol *s = NULL;
			if(IsFloat(argv[i])) s = sym_float;
			else if(IsInt(argv[i])) s = sym_int;
			else if(IsSymbol(argv[i])) s = sym_symbol;
#ifdef PD
			else if(IsPointer(argv[i])) s = sym_pointer;  // can pointer atoms occur here?
#endif
			if(s) m_methodmain(i,s,1,argv+i);			
		}
		
		ret = true;
	}
	
	if(!ret) ret = m_method_(inlet,s,argc,argv);
	
	return ret; // true if appropriate handler was found and called
}

bool flext_base::m_method_(int inlet,const t_symbol *s,int argc,t_atom *argv) 
{
//#ifdef _DEBUG
	post("%s: message unhandled - inlet:%i args:%i symbol:%s",thisName(),inlet,argc,s?s->s_name:"");
//#endif
	return false;
}


flext_base::methitem::methitem(int in,t_symbol *t): 
	inlet(in),tag(t),
	fun(NULL), 
	argc(0),args(NULL),
	nxt(NULL)
{}

flext_base::methitem::~methitem() 
{ 
	if(nxt) delete nxt;
	if(args) delete[] args; 
}

void flext_base::methitem::SetArgs(methfun _fun,int _argc,metharg *_args)
{
	fun = _fun;
	if(args) delete[] args;
	argc = _argc,args = _args;
}



void flext_base::AddMethItem(methitem *m)
{
	if(mlst) {
		methitem *mi;
		for(mi = mlst; mi->nxt; mi = mi->nxt) (void)0;
		mi->nxt = m;
	}
	else 
		mlst = m;
}

void flext_base::AddMethodDef(int inlet)
{
	AddMethItem(new methitem(inlet,NULL));
}

void flext_base::AddMethodDef(int inlet,const char *tag)
{
	AddMethItem(new methitem(inlet,gensym(const_cast<char *>(tag))));
}

void flext_base::AddMethod(int inlet,const char *tag,methfun fun,metharg tp,...)
{
	methitem *mi = new methitem(inlet,gensym(const_cast<char *>(tag)));

	va_list marker;
	va_start(marker,tp);
	int argc = 0;
	metharg *args = NULL,arg = tp;
	for(; arg != a_null; ++argc) arg = (metharg)va_arg(marker,int); //metharg);
	va_end(marker);
	
	if(argc > 0) {
		if(argc > MAXARGS) {
			error("Method %s: only %i arguments are type-checkable: use GIMME for more",tag?tag:"?",MAXARGS);
			argc = MAXARGS;
		}

		args = new metharg[argc];

		va_start(marker,tp);
		metharg a = tp;
		for(int ix = 0; ix < argc; ++ix) {
			if(a == a_gimme && ix > 0) {
				error("GIMME argument must be the first and only one");
			}
#ifdef PD
			if(a == a_pointer && flext_base::compatibility) {
				post("Pointer arguments are not allowed in compatibility mode"); 
			}
#endif
			args[ix] = a;
			a = (metharg)va_arg(marker,int); //metharg);
		}
		va_end(marker);
	}
	
	mi->SetArgs(fun,argc,args);

	AddMethItem(mi);
}



void flext_base::GetAString(const t_atom &a,char *buf,int szbuf)
{ 
#ifdef PD
	atom_string(const_cast<t_atom *>(&a),buf,szbuf);
#else
	if(IsSymbol(a)) sprintf(buf,GetString(a));
	else if(IsFloat(a)) sprintf(buf,"%f",GetFloat(a));
	else if(IsInt(a)) sprintf(buf,"%i",GetInt(a));
#endif
}  


