/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// Code for handling of object creation functions

#include "flext.h"

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#define ALIASDEL ','

#define ALIASSLASHES ":/\\"
#if FLEXT_OS == FLEXT_OS_MACOS
	#define ALIASSLASH ':'
#elif FLEXT_OS == FLEXT_OS_WIN
	#if FLEXT_SYS == FLEXT_SYS_PD
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
static bool chktilde(const char *objname)
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
class libobject {
public:
	libobject(t_class *&cl,flext_obj *(*newf)(int,t_atom *),void (*freef)(flext_hdr *)); 
	
	flext_obj *(*newfun)(int,t_atom *);
	void (*freefun)(flext_hdr *c);

	t_class *const &clss;
	bool lib,dsp,attr;
	int argc;
	int *argv;
};

libobject::libobject(t_class *&cl,flext_obj *(*newf)(int,t_atom *),void (*freef)(flext_hdr *)): 
	newfun(newf),freefun(freef),
	clss(cl),
	argc(0),argv(NULL) 
{}
	
// this class stands for one registered object name
// it holds a pointer to the respective object
// it will never be destroyed
class libname {
public:
	const t_symbol *name;
	libobject *obj;

	static void add(libname *n);
	static libname *Find(const t_symbol *s,libobject *o = NULL);
	
protected:
	libname(const t_symbol *n,libobject *o): name(n),obj(o),nxt(NULL) {}	

	static int Hash(const t_symbol *s);
	int Hash() const { return Hash(name); }

	enum { HASHBITS=7, HASHSIZE=1<<HASHBITS };

	libname *nxt;
	void Add(libname *n);

	static libname **root;
};

libname **libname::root = NULL;


void libname::Add(libname *n) { if(nxt) nxt->Add(n); else nxt = n; }

int libname::Hash(const t_symbol *s) 
{
	return flext::FoldBits(reinterpret_cast<unsigned long>(s),HASHBITS);
}

libname *libname::Find(const t_symbol *s,libobject *o) 
{
	if(!root) {
		root = new libname *[HASHSIZE];
		memset(root,0,HASHSIZE*sizeof(*root));
	}

	int hash = Hash(s);
	libname *a = root[hash];
	libname *pa = NULL;
	while(a && a->name != s) pa = a,a = a->nxt;

	if(!a && o) {
		a = new libname(s,o);
		if(pa) 
			// previous entry... extend
			a->nxt = pa->nxt,pa->nxt = a;
		else 
			// new singular entry
			root[hash] = a;
	}

	return a;
}


// for Max/MSP, the library is represented by a special object (class) registered at startup
// all objects in the library are clones of that library object - they share the same class
#if FLEXT_SYS == FLEXT_SYS_MAX
static t_class *lib_class = NULL;
static const t_symbol *lib_name = NULL;

flext_obj::t_classid flext_obj::thisClassId() const { return libname::Find(thisNameSym())->obj; }
t_class *flext_obj::getClass(t_classid id) { return reinterpret_cast<libobject *>(id)->clss; }
#endif

void flext_obj::lib_init(const char *name,void setupfun(),bool attr)
{
#if FLEXT_SYS == FLEXT_SYS_MAX
	lib_name = MakeSymbol(name);
	::setup(
		(t_messlist **)&lib_class,
		(t_newmethod)obj_new,(t_method)obj_free,
		sizeof(flext_hdr),NULL,A_GIMME,A_NULL);
#endif
	process_attributes = attr;
	setupfun();
}

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
     	sizeof(flext_hdr),0,A_GIMME,A_NULL);
#elif FLEXT_SYS == FLEXT_SYS_MAX
	if(!lib) {
		::setup(
			(t_messlist **)cl,
    		(t_newmethod)obj_new,(t_method)obj_free,
     		sizeof(flext_hdr),NULL,A_GIMME,A_NULL);
     	// attention: in Max/MSP the *cl variable is not initialized after that call.
     	// just the address is stored, the initialization then occurs with the first object instance!
	}
#else
#error
#endif

	// make new dynamic object
	libobject *lo = new libobject(*cl,newfun,freefun);
	lo->lib = lib;
	lo->dsp = dsp;
	lo->attr = process_attributes;

//	post("ADDCLASS %p -> LIBOBJ %p -> %p",*cl,lo,lo->clss);

	// parse the argument type list and store it with the object
	if(argtp1 == A_GIMME)
		lo->argc = -1;
	else {
		int argtp,i;
		va_list marker;
		
		// parse a first time and count only
		va_start(marker,argtp1);
		for(argtp = argtp1; argtp != A_NULL; ++lo->argc) argtp = (int)va_arg(marker,int); 
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

	// make help reference
	flext_obj::DefineHelp(lo->clss,idname,extract(names,-1),dsp);

	for(int ix = 0; ; ++ix) {
		// in this loop register all the possible aliases of the object
	
		const char *c = ix?extract(names,ix):GetString(nsym);
		if(!c || !*c) break;

		// add to name list
		libname *l = libname::Find(MakeSymbol(c),lo);
	
#if FLEXT_SYS == FLEXT_SYS_PD
		if(ix > 0) 
			// in PD the first name is already registered with class creation
			::class_addcreator((t_newmethod)obj_new,(t_symbol *)l->name,A_GIMME,A_NULL);
#elif FLEXT_SYS == FLEXT_SYS_MAX
		if(ix > 0 || lib) 
			// in Max/MSP the first alias gets its name from the name of the object file,
			// unless it is a library (then the name can be different)
			::alias(const_cast<char *>(c));  
#else
#error
#endif	
	}

	// get unique class id
#if FLEXT_SYS == FLEXT_SYS_PD
	t_classid clid = lo->clss;
#else
	// in Max/MSP the t_class *value can't be used because it's possible that's it's not yet set!!
	t_classid clid = lo;
#endif

	// call class setup function
    setupfun(clid);
}
	

typedef flext_obj *(*libfun)(int,t_atom *);

flext_hdr *flext_obj::obj_new(const t_symbol *s,int _argc_,t_atom *argv)
{
	flext_hdr *obj = NULL;
	libname *l = libname::Find(s);
	if(l) {
		bool ok = true;
		t_atom args[FLEXT_MAXNEWARGS]; 
		libobject *lo = l->obj;

		int argc = _argc_;
		if(lo->attr) {
			argc = flext_base::CheckAttrib(argc,argv);
		}

		if(lo->argc >= 0) {
#ifdef FLEXT_DEBUG
			if(lo->argc > FLEXT_MAXNEWARGS) { ERRINTERNAL(); ok = false; }
#endif

			if(argc == lo->argc) {
				for(int i = 0; /*ok &&*/ i < lo->argc; ++i) {
					switch(lo->argv[i]) {
#if FLEXT_SYS == FLEXT_SYS_MAX
					case A_INT:
						if(flext::IsInt(argv[i])) args[i] = argv[i];
						else if(flext::IsFloat(argv[i])) flext::SetInt(args[i],(int)flext::GetFloat(argv[i]));
						else ok = false;
						break;
#endif
					case A_FLOAT:
						if(flext::IsInt(argv[i])) flext::SetFloat(args[i],(float)flext::GetInt(argv[i]));
						else if(flext::IsFloat(argv[i])) args[i] = argv[i];
						else ok = false;
						break;
					case A_SYMBOL:
						if(flext::IsSymbol(argv[i])) args[i] = argv[i];
						else ok = false;
						break;
					}
				}
			
				if(!ok)
					post("%s: Creation arguments do not match",s->s_name);
			}
			else {
				error("%s: %s creation arguments",s->s_name,argc < lo->argc?"Not enough":"Too many");
				ok = false;
			}
		}

		if(ok) {
			t_classid clid;

#if FLEXT_SYS == FLEXT_SYS_PD
			clid = lo->clss;
			obj = (flext_hdr *)::pd_new(lo->clss);
#elif FLEXT_SYS == FLEXT_SYS_MAX
			clid = lo;
			obj = (flext_hdr *)::newobject(lo->clss);
#else
#error
#endif
//			post("NEWINST CLID %p",clid);

		    flext_obj::m_holder = obj;
			flext_obj::m_holdname = l->name;
			flext_obj::m_holdattr = lo->attr;

			// get actual flext object (newfun calls "new flext_obj()")
			if(lo->argc >= 0)
				// for interpreted arguments
				obj->data = lo->newfun(lo->argc,args); 
			else
				obj->data = lo->newfun(argc,argv); 
	
			flext_obj::m_holder = NULL;
			flext_obj::m_holdname = NULL;
			flext_obj::m_holdattr = false;

			ok = obj->data &&
				// check constructor exit flag
				obj->data->InitOk();

			if(ok) {
				// store creation args for attribute initialization (inside flext_base::Init())
				flext_obj::m_holdaargc = _argc_-argc;
				flext_obj::m_holdaargv = argv+argc;

				// call virtual init function 
				// here, inlets, outlets, methods and attributes can be set up
				ok = obj->data->Init();

				// call another virtual init function 
				if(ok) ok = obj->data->Finalize();

				flext_obj::m_holdaargc = 0;
				flext_obj::m_holdaargv = NULL;
			}

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
		// in Max/MSP an object with the name of the library exists, even if not explicitely declared!
		if(s != lib_name) 
#endif
		error("Class %s not found in library!",s->s_name);
#endif

	return obj;
}

void flext_obj::obj_free(flext_hdr *hdr)
{
	const t_symbol *name = hdr->data->thisNameSym();
	libname *l = libname::Find(name);

	if(l) {
		// call virtual exit function
		hdr->data->Exit();

		// now call object destructor and deallocate
		l->obj->freefun(hdr);
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


