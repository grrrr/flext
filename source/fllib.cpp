/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// Code for handling of MaxMSP external libraries

#ifdef MAXMSP

#include "flbase.h"
//#include <string.h>
#include <stdarg.h>

class libfunction {
public:
	libfunction(t_symbol *n,t_method newf,void (*freef)(flext_hdr *));
	~libfunction();
	void add(libfunction *n);
	
	t_symbol *name;
	t_newmethod newfun;
	void (*freefun)(flext_hdr *c);
	int argc;
	int *argv;
	
	libfunction *nxt;
};

static libfunction *libfuncs = NULL;
t_class *flext_obj::lib_class = NULL;

libfunction::libfunction(t_symbol *n,t_newmethod newf,void (*freef)(flext_hdr *)): name(n),newfun(newf),freefun(freef),nxt(NULL),argc(0),argv(NULL) {}
libfunction::~libfunction() { if(argv) delete[] argv; if(nxt) delete nxt; }
void libfunction::add(libfunction *n) { if(nxt) nxt->add(n); else nxt = n; }
	
void flext_obj::libfun_add(const char *name,t_newmethod newfun,void (*freefun)(flext_hdr *),int argtp1,...)
{
	for(int ix = 0; ; ++ix) {
		const char *c = fl_extract(name,ix);
		if(!c || !*c) break;

	 	alias(const_cast<char *>(c));  // make object name available to Max
	
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

typedef flext_hdr *(*libfunG)(t_symbol *,int,t_atom *);
typedef flext_hdr *(*libfun0)();
typedef flext_hdr *(*libfun1)(flext_obj::lib_arg &a1);
typedef flext_hdr *(*libfun2)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2);
typedef flext_hdr *(*libfun3)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2,flext_obj::lib_arg &a3);
typedef flext_hdr *(*libfun4)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2,flext_obj::lib_arg &a3,flext_obj::lib_arg &a4);
typedef flext_hdr *(*libfun5)(flext_obj::lib_arg &a1,flext_obj::lib_arg &a2,flext_obj::lib_arg &a3,flext_obj::lib_arg &a4,flext_obj::lib_arg &a5);

flext_hdr *flext_obj::libfun_new(t_symbol *s,int argc,t_atom *argv)
{
	libfunction *l;
	for(l = libfuncs; l; l = l->nxt) 
		if(s == l->name) break;
	
	if(l) {
		if(l->argc < 0) 
				return ((libfunG)l->newfun)(s,argc,argv);  // A_GIMME
		else {	
			if(argc < l->argc) 
				error("%s: Not enough creation arguments",s->s_name);
			else {
				lib_arg args[5];
				bool ok = true;
				for(int i = 0; /*ok &&*/ i < l->argc; ++i) {
					switch(l->argv[i]) {
					case A_LONG:
						if(argv[i].a_type == A_LONG) args[i].i = argv[i].a_w.w_long;
						else if(argv[i].a_type == A_FLOAT) args[i].i = argv[i].a_w.w_float;
						else ok = false;
						break;
					case A_FLOAT:
						if(argv[i].a_type == A_LONG) args[i].f = argv[i].a_w.w_long;
						else if(argv[i].a_type == A_FLOAT) args[i].f = argv[i].a_w.w_float;
						else ok = false;
						break;
					case A_SYM:
						if(argv[i].a_type == A_SYM) args[i].s = argv[i].a_w.w_sym;
						else ok = false;
						break;
					}
				}
			
				if(ok) {
					switch(l->argc) {
						case 0: return ((libfun0)l->newfun)();
						case 1: return ((libfun1)l->newfun)(args[0]);
						case 2: return ((libfun2)l->newfun)(args[0],args[1]);
						case 3: return ((libfun3)l->newfun)(args[0],args[1],args[2]);
						case 4: return ((libfun4)l->newfun)(args[0],args[1],args[2],args[3]);
						case 5: return ((libfun5)l->newfun)(args[0],args[1],args[2],args[3],args[4]);
						default:
							LOG("More than 5 arguments not expected");
					}
				}
				else
					post("%s: Creation arguments do not match",s->s_name);
			}
			return NULL;
		}
	}
	else {
		error("Class %s not found in library!",s->s_name);
		return NULL;
	}
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
		error("Class %s not found in library!",name);
}

#endif

