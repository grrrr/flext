/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// Code for handling of object creation functions

#include "flbase.h"
#include "flsupport.h"

#include <stdarg.h>


enum libargtp { a_null,a_int,a_float,a_sym };

class libfunction {
public:
	libfunction(t_symbol *n,t_newmethod newf,void (*freef)(flext_hdr *));
	~libfunction();
	void add(libfunction *n);
	
	t_symbol *name;
	t_newmethod newfun;
	void (*freefun)(flext_hdr *c);
	int argc;
	int *argv;
	bool lib,dsp;
	t_class *clss;
	
	libfunction *nxt;
};

static libfunction *libfuncs = NULL;
t_class *flext_obj::lib_class = NULL;
t_symbol *flext_obj::lib_name = NULL;

libfunction::libfunction(t_symbol *n,t_newmethod newf,void (*freef)(flext_hdr *)): name(n),newfun(newf),freefun(freef),nxt(NULL),argc(0),argv(NULL),clss(NULL) {}
libfunction::~libfunction() { if(argv) delete[] argv; if(nxt) delete nxt; }
void libfunction::add(libfunction *n) { if(nxt) nxt->add(n); else nxt = n; }

void flext_obj::libfun_add(bool lib,bool dsp,t_class *&clss,const char *name,void setupfun(t_class *),t_newmethod newfun,void (*freefun)(flext_hdr *),int argtp1,...)
{
	if(dsp) flext_util::chktilde(name);

#ifdef PD
    clss = ::class_new(
		::gensym((char *)flext_util::extract(name)),
    	(t_newmethod)&flext_obj::libfun_new,
    	(t_method)&flext_obj::libfun_free,
     	sizeof(flext_hdr),0,A_GIMME,A_NULL);
#elif defined(MAX)
	if(lib) 
		clss = flext_obj::lib_class;
	else {
		::setup(
			(t_messlist **)&clss,
    		(t_newmethod)&flext_obj::libfun_new,
    		(t_method)&flext_obj::libfun_free,
     		sizeof(flext_hdr),0,A_GIMME,A_NULL);
	}
#endif

	for(int ix = 0; ; ++ix) {
		const char *c = flext_util::extract(name,ix);
		if(!c || !*c) break;

		libfunction *l = new libfunction(gensym(const_cast<char *>(c)),newfun,freefun);
	
		l->clss = clss;
		l->lib = lib;
		l->dsp = dsp;

		if(argtp1 == A_GIMME)
			l->argc = -1;
		else {
			int argtp,i;
			va_list marker;
			va_start(marker,argtp1);
			l->argc = 0;
			for(argtp = argtp1; argtp != A_NULL; ++l->argc) argtp = (int)va_arg(marker,int); 
			va_end(marker);
	
			l->argv = new int[l->argc];
		
			va_start(marker,argtp1);
			for(argtp = argtp1,i = 0; i < l->argc; ++i) {
				l->argv[i] = argtp;
				argtp = (int)va_arg(marker,int); 
			}
			va_end(marker);
		}
	
		if(libfuncs) libfuncs->add(l);
		else libfuncs = l;

		if(ix == 0) {
			// make help reference
			flext_obj::DefineHelp(clss,c,flext_util::extract(name,-1),dsp);
		}
#ifdef PD
		if(ix > 0) ::class_addcreator((t_newmethod)libfun_new,l->name,A_GIMME,A_NULL);
#elif defined(MAXMSP)
		if(ix > 0 || lib) ::alias(const_cast<char *>(c));  // make object name available to Max
#endif	
	}

    setupfun(clss);
}
	
/*
void flext_obj::libfun_add(const char *name,t_newmethod newfun,void (*freefun)(flext_hdr *),int argtp1,...)
{
	for(int ix = 0; ; ++ix) {
		const char *c = flext::extract(name,ix);
		if(!c || !*c) break;

#ifdef PD
		if(ix > 0) { FLEXT_ADDALIAS1(c,libfun_new,A_GIMME); }
#elif defined(MAXMSP)
	 	alias(const_cast<char *>(c));  // make object name available to Max
#endif	

		libfunction *l = new libfunction(gensym(const_cast<char *>(c)),newfun,freefun);
	
		if(argtp1 == A_GIMME)
			l->argc = -1;
		else {
			int argtp,i;
			va_list marker;
			va_start(marker,argtp1);
			l->argc = 0;
			for(argtp = argtp1; argtp != A_NULL; ++l->argc) argtp = (int)va_arg(marker,int); 
			va_end(marker);
	
			l->argv = new int[l->argc];
		
			va_start(marker,argtp1);
			for(argtp = argtp1,i = 0; i < l->argc; ++i) {
				l->argv[i] = argtp;
				argtp = (int)va_arg(marker,int); 
			}
			va_end(marker);
		}
	
		if(libfuncs) libfuncs->add(l);
		else libfuncs = l;
	}
}
*/

typedef flext_obj *(*libfunV)(int,t_atom *);
typedef flext_obj *(*libfun0)();
typedef flext_obj *(*libfun1)(flext_obj::lib_arg &a1);
typedef flext_obj *(*libfun2)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2);
typedef flext_obj *(*libfun3)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2,flext_obj::lib_arg &a3);
typedef flext_obj *(*libfun4)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2,flext_obj::lib_arg &a3,flext_obj::lib_arg &a4);
typedef flext_obj *(*libfun5)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2,flext_obj::lib_arg &a3,flext_obj::lib_arg &a4,flext_obj::lib_arg &a5);

flext_hdr *flext_obj::libfun_new(t_symbol *s,int argc,t_atom *argv)
{
	flext_hdr *obj = NULL;

	libfunction *l;
	for(l = libfuncs; l; l = l->nxt) 
		if(s == l->name) break;
	
	if(l) {
		bool ok = true;
		lib_arg args[5];

		if(l->argc >= 0) {
			if(l->argc > 5) { ERRINTERNAL(); ok = false; }

			if(argc == l->argc) {
				for(int i = 0; /*ok &&*/ i < l->argc; ++i) {
					switch(l->argv[i]) {
#ifdef MAXMSP
					case A_INT:
						if(flext::IsInt(argv[i])) args[i].i = flext::GetInt(argv[i]);
						else if(flext::IsFloat(argv[i])) args[i].i = (int)flext::GetFloat(argv[i]);
						else ok = false;
						break;
#endif
					case A_FLOAT:
						if(flext::IsInt(argv[i])) args[i].f = (float)flext::GetInt(argv[i]);
						else if(flext::IsFloat(argv[i])) args[i].f = flext::GetFloat(argv[i]);
						else ok = false;
						break;
					case A_SYMBOL:
						if(flext::IsSymbol(argv[i])) args[i].s = flext::GetSymbol(argv[i]);
						else ok = false;
						break;
					}
				}
			
				if(ok) {
				}
				else
					post("%s: Creation arguments do not match",s->s_name);
			}
			else {
				error("%s: %s creation arguments",s->s_name,argc < l->argc?"Not enough":"Too many");
				ok = false;
			}
		}

		if(ok) {
#ifdef PD
			obj = (flext_hdr *)::pd_new(l->clss);
#elif defined(MAXMSP)
			obj = (flext_hdr *)::newobject(l->clss);
#endif
		    flext_obj::m_holder = obj;
			flext_obj::m_holdname = flext::GetString(l->name);

			switch(l->argc) {
				case -1: obj->data = ((libfunV)l->newfun)(argc,argv); break;
				case 0: obj->data = ((libfun0)l->newfun)(); break;
				case 1: obj->data = ((libfun1)l->newfun)(args[0]); break;
				case 2: obj->data = ((libfun2)l->newfun)(args[0],args[1]); break;
				case 3: obj->data = ((libfun3)l->newfun)(args[0],args[1],args[2]); break;
				case 4: obj->data = ((libfun4)l->newfun)(args[0],args[1],args[2],args[3]); break;
				case 5: obj->data = ((libfun5)l->newfun)(args[0],args[1],args[2],args[3],args[4]); break;
			}
			
			flext_obj::m_holder = NULL;
			if(!obj->data || !obj->data->InitOk()) { 
				l->freefun(obj); 
				obj = NULL; 
			}
		}
	}
	else {
		if(s != lib_name) error("Class %s not found in library!",s->s_name);
	}

	return obj;
}

void flext_obj::libfun_free(flext_hdr *hdr)
{
	t_symbol *name = gensym(const_cast<char *>(hdr->data->m_name));
	libfunction *l;
	for(l = libfuncs; l; l = l->nxt)
		if(name == l->name) break;

	if(l) 
		l->freefun(hdr);
	else 
		if(name != lib_name) error("Class %s not found in library!",name);
}


