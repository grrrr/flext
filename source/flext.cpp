/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flext.cpp
    \brief Implementation of the flext base class.
*/
 
#include "flext.h"
#include "flinternal.h"
#include <string.h>
#include <stdarg.h>


// === proxy class for flext_base ============================

#if FLEXT_SYS == FLEXT_SYS_PD

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

#elif FLEXT_SYS == FLEXT_SYS_MAX

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
	SetInt(atom,v);  
	cb_px_anything(c,sym_int,1,&atom);
}

void flext_base::cb_px_float(t_class *c,float v)
{
	// check if inlet allows float type
	t_atom atom;
	SetFloat(atom,v);  
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

#else
#error // Other system
#endif 


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

flext_base::flext_base():
	inlist(NULL),outlist(NULL),
	curtag(NULL),
	incnt(0),outcnt(0),
	insigs(0),outsigs(0),
	outlets(NULL),outattr(NULL),
	distmsgs(false),
	methhead(NULL),attrhead(NULL),attrcnt(0),
	inlets(NULL)
{
	LOG1("%s - flext logging is on",thisName());

#ifdef FLEXT_THREADS
//	shouldexit = false;
//	thrhead = thrtail = NULL;
#endif
	qhead = qtail = NULL;
	qclk = (t_qelem *)(qelem_new(this,(t_method)QTick));
#if FLEXT_SYS == FLEXT_SYS_MAX
	yclk = (t_clock *)(clock_new(this,(t_method)YTick));
#endif

	AddMethod(0,"getattributes",(methfun)cb_ListAttrib);
}

flext_base::~flext_base()
{
#ifdef FLEXT_THREADS
	TermThreads();
#endif

	// send remaining pending messages
	while(qhead) QTick(this);
	qelem_free((t_qelem *)qclk);
#if FLEXT_SYS == FLEXT_SYS_MAX
	clock_free((object *)yclk);
#endif

	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(outlets) delete[] outlets;

	if(inlets) {
		for(int ix = 0; ix < incnt; ++ix)
			if(inlets[ix]) {
#if FLEXT_SYS == FLEXT_SYS_PD
				pd_free(&inlets[ix]->obj.ob_pd);
#elif FLEXT_SYS == FLEXT_SYS_MAX
				freeobject((object *)inlets[ix]);
#endif
			}
		delete[] inlets;
	}
	
#if FLEXT_SYS == FLEXT_SYS_MAX
//	if(insigs) dsp_free(thisHdr());
	if(insigs) dsp_freebox(thisHdr());
#endif

	if(methhead) delete methhead;
	if(attrhead) delete attrhead;
}

/*! This virtual function is created after the object has been created, that is, 
	after the constructor has been processed. It creates the inlets and outlets.
	\note You can override it in your own class, but be sure to call it, 
	\note otherwise no inlets/outlets will be created
	\remark Creation of inlets/outlets can't be done upon declaration, as Max/MSP needs creation
	\remark in reverse.
*/
bool flext_base::Init()
{
	bool ok = true;

	// ----------------------------------
	// create inlets
	// ----------------------------------

	incnt = insigs = 0; 

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
#if FLEXT_SYS == FLEXT_SYS_PD
		{
			int cnt = 0;

			if(incnt >= 1) {
				switch(list[0]) {
					case xlet::tp_sig:
						CLASS_MAINSIGNALIN(thisClass(),flext_hdr,defsig);
						++insigs;
						break;
					default:
						// leftmost inlet is already there...
						break;
				} 
				++cnt;
			}		

			for(int ix = 1; ix < incnt; ++ix,++cnt) {
				switch(list[ix]) {
					case xlet::tp_float:
					case xlet::tp_int: {
						char sym[] = "ft??";
						if(ix >= 10) { 
							if(compatibility) {
								// Max allows max. 9 inlets
								post("%s: Only 9 float/int inlets allowed in compatibility mode",thisName());
								ok = false;
							}
							else {
								if(ix > 99)
									post("%s: Inlet index > 99 not allowed for float/int inlets",thisName());
								sym[2] = '0'+ix/10,sym[3] = '0'+ix%10;
							}
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
#elif FLEXT_SYS == FLEXT_SYS_MAX
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
						default:
							error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
							ok = false;
					} 
				}
			}

			incnt = cnt;
	
			if(insigs) 
//				dsp_setup(thisHdr(),insigs); // signal inlets	
				dsp_setupbox(thisHdr(),insigs); // signal inlets	
		}
#else
#error
#endif

		delete[] list;
	}
	
/*
	if(outlets) { delete[] outlets; outlets = NULL; }
*/
	// ----------------------------------
	// create outlets
	// ----------------------------------

	outcnt = outsigs = 0; 
	
#if FLEXT_SYS == FLEXT_SYS_MAX
	// for Max/MSP the rightmost outlet has to be created first
	if(procattr) 
		outattr = (outlet *)newout_anything(&x_obj->obj);
#endif

	if(outlist) {
		xlet *xi;

		// count outlets
		outcnt = 0;
		for(xi = outlist; xi; xi = xi->nxt) ++outcnt;

		xlet::type *list = new xlet::type[outcnt];
		int i;
		for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
		delete outlist; outlist = NULL;
		
		outlets = new outlet *[outcnt];

		// type info is now in list array
#if FLEXT_SYS == FLEXT_SYS_PD
		for(int ix = 0; ix < outcnt; ++ix) 
#elif FLEXT_SYS == FLEXT_SYS_MAX
		for(int ix = outcnt-1; ix >= 0; --ix) 
#else
#error
#endif
		{
			switch(list[ix]) {
				case xlet::tp_float:
					outlets[ix] = (outlet *)newout_float(&x_obj->obj);
					break;
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
#ifdef FLEXT_DEBUG
				default:
					ERRINTERNAL();
					ok = false;
#endif
			} 
		}
		
		delete[] list;
	}

	if(procattr) {
#if FLEXT_SYS == FLEXT_SYS_PD
		// attribute dump outlet is the last one
		outattr = (outlet *)newout_anything(&x_obj->obj);
#endif

		// initialize creation attributes
		if(m_holdaargc && m_holdaargv)
			ok = InitAttrib(m_holdaargc,m_holdaargv);
	}


	return ok;
}

/*! Set up proxy classes and basic methods at class creation time
	This ensures that they are processed before the registered flext messages
*/
void flext_base::Setup(t_class *c)
{
	add_method(c,cb_help,"help");
	add_loadbang(c,cb_loadbang);
#if FLEXT_SYS == FLEXT_SYS_MAX
	add_assist(c,cb_assist);
#endif

	// proxy for extra inlets
#if FLEXT_SYS == FLEXT_SYS_PD
	add_anything(c,cb_px_anything); // for leftmost inlet
    px_class = class_new(gensym("flext_base proxy"),NULL,NULL,sizeof(px_object),CLASS_PD|CLASS_NOINLET, A_NULL);
	add_anything(px_class,px_object::px_method); // for other inlets
#elif FLEXT_SYS == FLEXT_SYS_MAX
	add_bang(c,cb_px_bang);
	add_method1(c,cb_px_int,"int",A_INT);  
	add_method1(c,cb_px_float,"float",A_FLOAT);  
	add_methodG(c,cb_px_anything,"list");  
	add_anything(c,cb_px_anything);
#else
#error
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

#ifdef FLEXT_THREADS
	thrid = pthread_self();

	StartHelper();
#endif
}

void flext_base::cb_help(t_class *c) { thisObject(c)->m_help(); }	

void flext_base::cb_loadbang(t_class *c) { thisObject(c)->m_loadbang(); }	
#if FLEXT_SYS == FLEXT_SYS_MAX
void flext_base::cb_assist(t_class *c,void * /*b*/,long msg,long arg,char *s) { thisObject(c)->m_assist(msg,arg,s); }
#endif

void flext_base::m_help()
{
	// This should better be overloaded
	post("%s (using flext " FLEXT_VERSTR ") - compiled on %s %s",thisName(),__DATE__,__TIME__);
}

/*! \brief All the message processing
	The messages of all the inlets go here and are promoted to the registered callback functions
*/
bool flext_base::m_methodmain(int inlet,const t_symbol *s,int argc,const t_atom *argv)
{
	static bool trap = false;
	bool ret = false;

	curtag = s;

	LOG3("methodmain inlet:%i args:%i symbol:%s",inlet,argc,s?s->s_name:"");
	
	for(const methitem *m = methhead; m && !ret; m = m->nxt) {
		if(m->tag == s && (inlet == m->inlet || m->inlet < 0 )) {
			// tag fits
			LOG4("found method tag %s: inlet=%i, symbol=%s, argc=%i",m->tag->s_name,inlet,s->s_name,argc);

			if(m->argc == 1 && m->args[0] == a_list) {
				ret = ((methfun_V)m->fun)(this,argc,const_cast<t_atom *>(argv));
			}
			else if(m->argc == 1 && m->args[0] == a_any) {
				ret = ((methfun_A)m->fun)(this,s,argc,const_cast<t_atom *>(argv));
			}
			else if(argc == m->argc) {
				int ix;
				t_any aargs[FLEXT_MAXMETHARGS];
				bool ok = true;
				for(ix = 0; ix < argc && ok; ++ix) {
					switch(m->args[ix]) {
					case a_float: {
						if(IsFloat(argv[ix])) aargs[ix].ft = GetFloat(argv[ix]);
						else if(IsInt(argv[ix])) aargs[ix].ft = (float)GetInt(argv[ix]);
						else ok = false;
						
						if(ok) LOG2("int arg %i = %f",ix,aargs[ix].ft);
						break;
					}
					case a_int: {
						if(IsFloat(argv[ix])) aargs[ix].it = (int)GetFloat(argv[ix]);
						else if(IsInt(argv[ix])) aargs[ix].it = GetInt(argv[ix]);
						else ok = false;
						
						if(ok) LOG2("float arg %i = %i",ix,aargs[ix].it);
						break;
					}
					case a_symbol: {
						if(IsSymbol(argv[ix])) aargs[ix].st = GetSymbol(argv[ix]);
						else ok = false;
						
						if(ok) LOG2("symbol arg %i = %s",ix,GetString(aargs[ix].st));
						break;
					}
#if FLEXT_SYS == FLEXT_SYS_PD
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

				if(ok && ix == argc) {
					switch(argc) {
					case 0:	ret = ((methfun_0)m->fun)(this); break;
					case 1:	ret = ((methfun_1)m->fun)(this,aargs[0]); break;
					case 2:	ret = ((methfun_2)m->fun)(this,aargs[0],aargs[1]); break;
					case 3:	ret = ((methfun_3)m->fun)(this,aargs[0],aargs[1],aargs[2]); break;
					case 4:	ret = ((methfun_4)m->fun)(this,aargs[0],aargs[1],aargs[2],aargs[3]); break;
					case 5:	ret = ((methfun_5)m->fun)(this,aargs[0],aargs[1],aargs[2],aargs[3],aargs[4]); break;
					}
				}
			}
		} 
		else if(m->tag == sym_symbol && !argc && (inlet == m->inlet || m->inlet < 0 )) {
			// symbol
			LOG3("found symbol method for %s: inlet=%i, symbol=%s",m->tag->s_name,inlet,s->s_name);

			t_any sym; sym.st = const_cast<t_symbol *>(s);
			ret = ((methfun_1)m->fun)(this,sym);
		}
		else if(m->tag == sym_anything && (inlet == m->inlet || m->inlet < 0) && m->argc == 1 && m->args[0] == a_any) {
			// any
			LOG4("found any method for %s: inlet=%i, symbol=%s, argc=%i",m->tag->s_name,inlet,s->s_name,argc);

			ret = ((methfun_A)m->fun)(this,s,argc,const_cast<t_atom *>(argv));
		}
	}
	
#if FLEXT_SYS == FLEXT_SYS_MAX
	// If float message is not explicitly handled: try int handler instead
	if(!ret && argc == 1 && s == sym_float && !trap) {
		t_atom fl;
		SetInt(fl,GetAInt(argv[0]));
		trap = true;
		ret = m_methodmain(inlet,sym_int,1,&fl);
		trap = false;
	}
	
	// If int message is not explicitly handled: try float handler instead
	if(!ret && argc == 1 && s == sym_int && !trap) {
		t_atom fl;
		SetFloat(fl,GetAFloat(argv[0]));
		trap = true;
		ret = m_methodmain(inlet,sym_float,1,&fl);
		trap = false;
	}
#endif
	
	// If float or int message is not explicitly handled: try list handler instead
	if(!ret && !trap && argc == 1 && (s == sym_float
#if FLEXT_SYS == FLEXT_SYS_MAX
		|| s == sym_int
#endif
	)) {
		t_atom list;
		if(s == sym_float) 
			SetFloat(list,GetFloat(argv[0]));
#if FLEXT_SYS == FLEXT_SYS_MAX
		else if(s == sym_int)
			SetInt(list,GetInt(argv[0]));
#endif

		trap = true;
		ret = m_methodmain(inlet,sym_list,1,&list);
		trap = false;
	}

	// If symbol message (pure anything without args) is not explicitly handled: try list handler instead
	if(!ret && !trap && argc == 0) {
		t_atom list;
		SetSymbol(list,s);
		trap = true;
		ret = m_methodmain(inlet,sym_list,1,&list);
		trap = false;
	}

	// if distmsgs is switched on then distribute list elements over inlets (Max/MSP behavior)
	if(!ret && distmsgs && !trap && inlet == 0 && s == sym_list && insigs <= 1) {
		int i = incnt;
		if(i > argc) i = argc;
		for(--i; i >= 0; --i) { // right to left distribution
			const t_symbol *sym = NULL;
			if(IsFloat(argv[i])) sym = sym_float;
			else if(IsInt(argv[i])) sym = sym_int;
			else if(IsSymbol(argv[i])) sym = sym_symbol;
#if FLEXT_SYS == FLEXT_SYS_PD
			else if(IsPointer(argv[i])) sym = sym_pointer;  // can pointer atoms occur here?
#endif
			if(sym) {
				trap = true;
				m_methodmain(i,sym,1,argv+i);			
				trap = false;
			}
		}
		
		ret = true;
	}
	
	if(!ret && !trap) ret = m_method_(inlet,s,argc,argv);
	
	curtag = NULL;

	return ret; // true if appropriate handler was found and called
}

bool flext_base::m_method_(int inlet,const t_symbol *s,int argc,const t_atom *argv) 
{
//#ifdef FLEXT_DEBUG
	post("%s: message unhandled - inlet:%i args:%i symbol:%s",thisName(),inlet,argc,s?s->s_name:"");
//#endif
	return false;
}


flext_base::methitem::methitem(int in,const t_symbol *t): 
	tag(t),inlet(in),
	argc(0),args(NULL),
	fun(NULL), 
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
	if(methhead) {
		methitem *mi;
		for(mi = methhead; mi->nxt; mi = mi->nxt) {}
		mi->nxt = m;
	}
	else 
		methhead = m;
}
/*
const flext_base::methitem *flext_base::FindMethItem(int inlet,const t_symbol *tag,const methitem *st)
{
	const methitem *mi = st?st:mlst;
	if(inlet < 0) {
		for(; mi; mi = mi->nxt) 
			if(mi->tag == tag) break;
	}
	else {
		for(; mi; mi = mi->nxt) 
			if(mi->inlet == inlet && mi->tag == tag) break;
	}
	return mi;
}
*/

void flext_base::AddMethodDef(int inlet,const char *tag)
{
	AddMethItem(new methitem(inlet,tag?MakeSymbol(tag):NULL));
}

/*! \brief Add a method to the queue
*/
void flext_base::AddMethod(int inlet,const char *tag,methfun fun,metharg tp,...)
{
	methitem *mi = new methitem(inlet,MakeSymbol(tag));

	va_list marker;

	// at first just count the arg type list (in argc)
	int argc = 0;
	va_start(marker,tp);
	metharg *args = NULL,arg = tp;
	for(; arg != a_null; ++argc) arg = (metharg)va_arg(marker,int); //metharg);
	va_end(marker);
	
	if(argc > 0) {
		if(argc > FLEXT_MAXMETHARGS) {
			error("%s - method %s: only %i arguments are type-checkable: use variable argument list for more",thisName(),tag?tag:"?",FLEXT_MAXMETHARGS);
			argc = FLEXT_MAXMETHARGS;
		}

		args = new metharg[argc];

		va_start(marker,tp);
		metharg a = tp;
		for(int ix = 0; ix < argc; ++ix) {
#ifdef FLEXT_DEBUG
			if(a == a_list && ix > 0) {
				ERRINTERNAL();
			}
#endif
#if FLEXT_SYS == FLEXT_SYS_PD
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


