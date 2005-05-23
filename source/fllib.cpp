/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fllib.cpp
    \brief Code for handling of object (and library) creation functions.
*/

#include "flext.h"
#include "flinternal.h"

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#define ALIASDEL ','

#define ALIASSLASHES ":/\\"
#if FLEXT_OS == FLEXT_OS_MAC
	#define ALIASSLASH ':'
#elif FLEXT_OS == FLEXT_OS_WIN
	#if FLEXT_SYS == FLEXT_SYS_PD
		#define ALIASSLASH '/'
	#elif FLEXT_SYS == FLEXT_SYS_MAX
		#define ALIASSLASH '/'
	#else
		#error "Undefined"
	#endif
#else
	// default to "/"
	#define ALIASSLASH '/'
#endif

//! Extract space-delimited words from a string
static const char *extract(const char *name,int ix = 0)
{
	static char tmp[1024];
	const char *n = name;
	
	const char *del = strchr(name,ALIASDEL);

	if(del) {
		if(ix < 0) {
			char *t = tmp;
			while(n < del && (isspace(*n) || strchr(ALIASSLASHES,*n))) ++n;
			while(n < del && !isspace(*n)) {
				char c = *(n++);
				*(t++) = strchr(ALIASSLASHES,c)?ALIASSLASH:c;
			}
			while(*t == ALIASSLASH && t > tmp) --t;
			*t = 0;

			return tmp;
		}

		n = del+1;
	}

	while(*n && isspace(*n)) ++n;
	
	for(int i = 0; n && *n; ++i) {
		if(i == ix) {
			char *t = tmp;

			for(; *n && !isspace(*n); ++t,++n) *t = *n;
			*t = 0;
			return *tmp?tmp:NULL;
		}
		else {
			while(*n && !isspace(*n)) ++n;
			while(*n && isspace(*n)) ++n;		
		}
	}

	return NULL;
}


//! Check if object's name ends with a tilde
bool flext::chktilde(const char *objname)
{
//	int stplen = strlen(setupfun);
	bool tilde = true; //!strncmp(setupfun,"_tilde",6);

	if((objname[strlen(objname)-1] == '~'?1:0)^(tilde?1:0)) {
		if(tilde) 
			error("flext: %s (no trailing ~) is defined as a tilde object",objname);
		else
			error("flext::check_tilde: %s is no tilde object",objname);
		return true;
	} 
	else
		return false;
}


// this class stands for one registered object
// it holds the class, type flags, constructor and destructor of the object and the creation arg types
// it will never be destroyed
class flext_class:
    public flext_root
{
public:
	flext_class(t_class *&cl,flext_obj *(*newf)(int,t_atom *),void (*freef)(flext_hdr *)); 
	
	flext_obj *(*newfun)(int,t_atom *);
	void (*freefun)(flext_hdr *c);

	t_class *const &clss;
	bool lib,dsp,attr,dist;
	int argc;
	int *argv;

    flext_base::ItemCont meths,attrs;
};

flext_class::flext_class(t_class *&cl,flext_obj *(*newf)(int,t_atom *),void (*freef)(flext_hdr *)): 
	clss(cl),
	newfun(newf),freefun(freef),
	argc(0),argv(NULL) 
    , dist(false)
{}


typedef TablePtrMap<const t_symbol *,flext_class *,8> LibMap;

static LibMap libnames;

//! Store or retrieve registered classes
static flext_class *FindName(const t_symbol *s,flext_class *o = NULL) 
{
    flext_class *cl = libnames.find(s);
    if(!cl) libnames.insert(s,cl = o);
    return cl;
}


// for Max/MSP, the library is represented by a special object (class) registered at startup
// all objects in the library are clones of that library object - they share the same class
#if FLEXT_SYS == FLEXT_SYS_MAX
static t_class *lib_class = NULL;
static const t_symbol *lib_name = NULL;
#endif

t_class *flext_obj::getClass(t_classid id) { return reinterpret_cast<flext_class *>(id)->clss; }

bool flext_obj::HasAttributes() const { return clss->attr; }


void flext_obj::lib_init(const char *name,void setupfun(),bool attr)
{
    flext::Setup();

#if FLEXT_SYS == FLEXT_SYS_MAX
	lib_name = MakeSymbol(name);
	::setup(
		(t_messlist **)&lib_class,
		(t_newmethod)obj_new,(t_method)obj_free,
		sizeof(flext_hdr),NULL,A_GIMME,A_NULL);
#endif
	process_attributes = attr;

    try {
	    setupfun();
    }
    catch(std::exception &x) {
        error("%s - %s",name,x.what());
    }
    catch(char *txt) {
    	error("%s - %s",name,txt);
    }
    catch(...) {
    	error("%s - Unknown exception at library setup",name);
    }
}

#if FLEXT_SYS == FLEXT_SYS_JMAX
static void jmax_class_inst(t_class *cl) 
{
	fts_class_init(cl, sizeof(flext_hdr),flext_obj::obj_new,flext_obj::obj_free);
}
#endif

void flext_obj::obj_add(bool lib,bool dsp,bool attr,const char *idname,const char *names,void setupfun(t_classid),flext_obj *(*newfun)(int,t_atom *),void (*freefun)(flext_hdr *),int argtp1,...)
{
	// get first possible object name
	const t_symbol *nsym = MakeSymbol(extract(names));
	
#ifdef FLEXT_DEBUG
	if(dsp) chktilde(GetString(nsym));
#endif

	if(!lib) process_attributes = attr;

	// set dynamic class pointer
	t_class **cl = 
#if FLEXT_SYS == FLEXT_SYS_MAX
		lib?&lib_class:
#endif
		new t_class *;

	// register object class
#if FLEXT_SYS == FLEXT_SYS_PD
    *cl = ::class_new(
		(t_symbol *)nsym,
    	(t_newmethod)obj_new,(t_method)obj_free,
     	sizeof(flext_hdr),CLASS_DEFAULT,A_GIMME,A_NULL);
#elif FLEXT_SYS == FLEXT_SYS_MAX
	if(!lib) {
		::setup(
			(t_messlist **)cl,
    		(t_newmethod)obj_new,(t_method)obj_free,
     		sizeof(flext_hdr),NULL,A_GIMME,A_NULL);
     	// attention: in Max/MSP the *cl variable is not initialized after that call.
     	// just the address is stored, the initialization then occurs with the first object instance!
	}
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	*cl = fts_class_install(nsym, jmax_class_inst);
#else
#error
#endif

	// make new dynamic object
	flext_class *lo = new flext_class(*cl,newfun,freefun);
	lo->lib = lib;
	lo->dsp = dsp;
	lo->attr = process_attributes;

//	post("ADDCLASS %p -> LIBOBJ %p -> %p",*cl,lo,lo->clss);

	// parse the argument type list and store it with the object
	if(argtp1 == FLEXTTPN_VAR)
		lo->argc = -1;
	else {
		int argtp,i;
		va_list marker;
		
		// parse a first time and count only
		va_start(marker,argtp1);
		for(argtp = argtp1; argtp != FLEXTTPN_NULL; ++lo->argc) argtp = (int)va_arg(marker,int); 
		va_end(marker);

		lo->argv = new int[lo->argc];
	
		// now parse and store
		va_start(marker,argtp1);
		for(argtp = argtp1,i = 0; i < lo->argc; ++i) {
			lo->argv[i] = argtp;
			argtp = (int)va_arg(marker,int); 
		}
		va_end(marker);
	}

	// get unique class id
	t_classid clid = lo;

	// make help reference
	flext_obj::DefineHelp(clid,idname,extract(names,-1),dsp);

	for(int ix = 0; ; ++ix) {
		// in this loop register all the possible aliases of the object
	
		const char *c = ix?extract(names,ix):GetString(nsym);
		if(!c || !*c) break;

		// add to name list
        const t_symbol *lsym = MakeSymbol(c);
		FindName(lsym,lo);
	
#if FLEXT_SYS == FLEXT_SYS_PD
		if(ix > 0) 
			// in PD the first name is already registered with class creation
			::class_addcreator((t_newmethod)obj_new,(t_symbol *)lsym,A_GIMME,A_NULL);
#elif FLEXT_SYS == FLEXT_SYS_MAX
		if(ix > 0 || lib) 
			// in Max/MSP the first alias gets its name from the name of the object file,
			// unless it is a library (then the name can be different)
			::alias(const_cast<char *>(c));  
#elif FLEXT_SYS == FLEXT_SYS_JMAX
		if(ix > 0)  fts_class_alias(lo->clss,lsym);
#else
#error
#endif	
	}

    try {
	    // call class setup function
        setupfun(clid);
    }
    catch(std::exception &x) {
        error("%s - Exception while initializing class: %s",idname,x.what());
    }
    catch(char *txt) {
    	error("%s - Exception while initializing class: %s",idname,txt);
    }
    catch(...) {
    	error("%s - Unknown exception while initializing class",idname);
    }
}
	

#define NEWARGS 256 // must be larger than FLEXT_NEWARGS = 5

typedef flext_obj *(*libfun)(int,t_atom *);

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_obj::obj_new(fts_object_t *o, int, fts_symbol_t s, int _argc_, const fts_atom_t *argv)
{
	flext_hdr *obj = (flext_hdr *)o;
#else
#if FLEXT_SYS == FLEXT_SYS_MAX
flext_hdr *flext_obj::obj_new(const t_symbol *s,short _argc_,t_atom *argv)
#else
flext_hdr *flext_obj::obj_new(const t_symbol *s,int _argc_,t_atom *argv)
#endif
{
	flext_hdr *obj = NULL;
#endif
	flext_class *lo = FindName(s);
	if(lo) {
		bool ok = true;
		t_atom args[NEWARGS]; 

		int argc = _argc_;
		if(lo->attr) {
			argc = flext_base::CheckAttrib(argc,argv);
		}

		if(lo->argc >= 0) {
#ifdef FLEXT_DEBUG
			if(lo->argc > FLEXT_MAXNEWARGS) { ERRINTERNAL(); ok = false; }
#endif

			int misnum = 0;
			if(argc > lo->argc) { ok = false; misnum = 1; }

			for(int i = 0; ok && i < lo->argc; ++i) {
				switch(lo->argv[i]) {
#if FLEXT_SYS != FLEXT_SYS_PD
				case FLEXTTPN_INT:
				case FLEXTTPN_DEFINT:
					if(i >= argc)
						if(lo->argv[i] == FLEXTTPN_DEFINT) SetInt(args[i],0);
						else { misnum = -1,ok = false; break; }
					else if(IsInt(argv[i])) args[i] = argv[i];
					else if(IsFloat(argv[i])) SetInt(args[i],(int)GetFloat(argv[i]));
					else ok = false;
					break;
#endif
				case FLEXTTPN_FLOAT:
				case FLEXTTPN_DEFFLOAT:
					if(i >= argc)
						if(lo->argv[i] == FLEXTTPN_DEFFLOAT) SetFloat(args[i],0);
						else { misnum = -1,ok = false; break; }
					else if(IsInt(argv[i])) SetFloat(args[i],(float)GetInt(argv[i]));
					else if(IsFloat(argv[i])) args[i] = argv[i];
					else ok = false;
					break;
				case FLEXTTPN_SYM:
				case FLEXTTPN_DEFSYM:
					// \todo shall we analyze the patcher args????... should already be done!
					if(i >= argc)
						if(lo->argv[i] == FLEXTTPN_DEFSYM) SetSymbol(args[i],sym__);
						else { misnum = -1,ok = false; break; }
					else if(IsSymbol(argv[i])) 
//							SetSymbol(args[i],GetParamSym(GetSymbol(argv[i]),NULL));
						args[i] = argv[i];
					else ok = false;
					break;
				}	
			}

			if(!ok)
				if(misnum)
					error("%s: %s creation arguments",GetString(s),misnum < 0?"Not enough":"Too many");
				else
					error("%s: Creation arguments do not match",GetString(s));
		}


		if(ok) {
            try {
#if FLEXT_SYS == FLEXT_SYS_PD
			    obj = (flext_hdr *)::pd_new(lo->clss);
#elif FLEXT_SYS == FLEXT_SYS_MAX
			    obj = (flext_hdr *)::newobject(lo->clss);
#else
#error
#endif

                flext_obj::m_holder = obj;
			    flext_obj::m_holdclass = lo;
			    flext_obj::m_holdname = s;
                flext_obj::initing = true;
                flext_obj::init_ok = true;

			    // get actual flext object (newfun calls "new flext_obj()")
			    if(lo->argc >= 0)
				    obj->data = lo->newfun(lo->argc,args); 
			    else
				    obj->data = lo->newfun(argc,argv); 
    	
			    flext_obj::m_holder = NULL;
			    flext_obj::m_holdclass = NULL;
			    flext_obj::m_holdname = NULL;

			    ok = obj->data &&
				    // check constructor exit flag
				    flext_obj::init_ok;

			    if(ok) {
				    if(lo->attr) {
					    // DON'T convert eventual patcher args here... this is done by the actual attribute stuff
					    // so that the initial $- or #- be preserved!

					    // store creation args for attribute initialization (inside flext_base::Init())
					    flext_obj::m_holdaargc = _argc_-argc;
					    flext_obj::m_holdaargv = argv+argc;
				    }
				    else {
					    flext_obj::m_holdaargc = 0;
					    flext_obj::m_holdaargv = NULL;
				    }

				    // call virtual init function 
				    // here, inlets, outlets, methods and attributes can be set up
				    ok = obj->data->Init();

                    flext_obj::initing = false;

				    // call another virtual init function 
				    if(ok) ok = obj->data->Finalize();

				    flext_obj::m_holdaargc = 0;
				    flext_obj::m_holdaargv = NULL;
			    }

            } //try
            catch(std::exception &x) {
                error("%s - Exception while creating object: %s",GetString(s),x.what());
                ok = false;
            }
            catch(char *txt) {
    		    error("%s - Exception while creating object: %s",GetString(s),txt);
                ok = false;
            }
            catch(...) {
    		    error("%s - Unknown exception while creating object",GetString(s));
                ok = false;
            }

            flext_obj::initing = false;

            if(!ok) { 
				// there was some init error, free object
				lo->freefun(obj); 
				obj = NULL; 
			}
		}
	}
#ifdef FLEXT_DEBUG
	else
#if FLEXT_SYS == FLEXT_SYS_MAX
		// in Max/MSP an object with the name of the library exists, even if not explicitly declared!
		if(s != lib_name) 
#endif
		error("Class %s not found in library!",s->s_name);
#endif

#if FLEXT_SYS != FLEXT_SYS_JMAX
	return obj;
#endif
}

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_obj::obj_free(fts_object_t *h, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
#else
void flext_obj::obj_free(flext_hdr *h)
#endif
{
	flext_hdr *hdr = (flext_hdr *)h;
	const t_symbol *name = hdr->data->thisNameSym();
	flext_class *lcl = FindName(name);

	if(lcl) {
        try {
            flext_obj::exiting = true;

		    // call virtual exit function
		    hdr->data->Exit();

		    // now call object destructor and deallocate
		    lcl->freefun(hdr);
        } //try
        catch(std::exception &x) {
            error("%s - Exception while destroying object: %s",GetString(name),x.what());
        }
        catch(char *txt) {
    		error("%s - Exception while destroying object: %s",GetString(name),txt);
        }
        catch(...) {
    		error("%s - Unknown exception while destroying object",GetString(name));
        }

        flext_obj::exiting = false;
    }
#ifdef FLEXT_DEBUG
	else 
#if FLEXT_SYS == FLEXT_SYS_MAX
		// in Max/MSP an object with the name of the library exists, even if not explicitely declared!
		if(name != lib_name) 
#endif
		error("Class %s not found in library!",name);
#endif
}


t_class *flext_obj::thisClass() const { FLEXT_ASSERT(x_obj); return thisClassId()->clss; }

void flext_base::SetDist(t_classid c,bool d) { c->dist = d; }
bool flext_base::DoDist() const { return thisClassId()->dist; }

flext_base::ItemCont *flext_base::ClMeths(t_classid c) { return &c->meths; }
flext_base::ItemCont *flext_base::ClAttrs(t_classid c) { return &c->attrs; }
