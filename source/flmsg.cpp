/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flmsg.cpp
    \brief Message processing of flext base class.
*/
 
#include "flext.h"

bool flext_base::CallMeth(const methitem &m,int argc,const t_atom *argv)
{
	bool ret = false;
	int ix;
	t_any aargs[FLEXT_MAXMETHARGS];
	bool ok = true;
	for(ix = 0; ix < argc && ok; ++ix) {
		switch(m.args[ix]) {
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
		case 0:	ret = ((methfun_0)m.fun)(this); break;
		case 1:	ret = ((methfun_1)m.fun)(this,aargs[0]); break;
		case 2:	ret = ((methfun_2)m.fun)(this,aargs[0],aargs[1]); break;
		case 3:	ret = ((methfun_3)m.fun)(this,aargs[0],aargs[1],aargs[2]); break;
		case 4:	ret = ((methfun_4)m.fun)(this,aargs[0],aargs[1],aargs[2],aargs[3]); break;
		case 5:	ret = ((methfun_5)m.fun)(this,aargs[0],aargs[1],aargs[2],aargs[3],aargs[4]); break;
		}
	}
	
	return ret;
}

bool flext_base::TryMethTag(const methitem *m,int inlet,const t_symbol *t,int argc,const t_atom *argv)
{
	do {
		if(m->inlet == inlet && m->tag == t) {
			LOG3("found method tag %s: inlet=%i, argc=%i",GetString(m->tag),m->inlet,argc);
		
			if(m->attr) {
				// attributes are treated differently

				if(m->attr->IsGet())
					return GetAttrib(m->attr);
				else
					return SetAttrib(m->attr,argc,argv);
			}
			else {
				if(m->argc == 1) {
					// try list
					if(m->args[0] == a_list && ((methfun_V)m->fun)(this,argc,const_cast<t_atom *>(argv))) return true;

					// try anything
					if(m->args[0] == a_any && ((methfun_A)m->fun)(this,m->tag,argc,const_cast<t_atom *>(argv))) return true;
				}

				// try matching number of args
				if(argc == m->argc && CallMeth(*m,argc,argv)) return true;
			}
		}
	} while((m = (const methitem *)m->nxt) != NULL);
	return false;
}

bool flext_base::TryMethSym(const methitem *m,int inlet,const t_symbol *t,const t_symbol *s)
{
	do {
		if(!m->IsAttr() && m->inlet == inlet && m->tag == t) {
			LOG3("found symbol method for %s: inlet=%i, symbol=%s",GetString(m->tag),m->inlet,GetString(s));

			t_any sym; sym.st = const_cast<t_symbol *>(s);
			if(((methfun_1)m->fun)(this,sym)) return true;
		}
	} while((m = (const methitem *)m->nxt) != NULL);
	return false;
}

bool flext_base::TryMethAny(const methitem *m,int inlet,const t_symbol *t,const t_symbol *s,int argc,const t_atom *argv)
{
	do {
		if(!m->IsAttr() && m->inlet == inlet && m->tag == t) {
			LOG4("found any method for %s: inlet=%i, symbol=%s, argc=%i",GetString(m->tag),m->inlet,GetString(s),argc);

			if(((methfun_A)m->fun)(this,s,argc,const_cast<t_atom *>(argv))) return true;
		}
	} while((m = (const methitem *)m->nxt) != NULL);
	return false;
}

bool flext_base::FindMeth(int inlet,const t_symbol *s,int argc,const t_atom *argv)
{
	methitem *m;
	
	// search for exactly matching tag
	if((m = (methitem *)methhead->Find(s,inlet)) != NULL && TryMethTag(m,inlet,s,argc,argv)) return true;
	if((m = (methitem *)clmethhead->Find(s,inlet)) != NULL && TryMethTag(m,inlet,s,argc,argv)) return true;
	
	// if no list args, then search for pure symbol 
	if(!argc) {
		if((m = (methitem *)methhead->Find(sym_symbol,inlet)) != NULL && TryMethSym(m,inlet,sym_symbol,s)) return true;
		if((m = (methitem *)clmethhead->Find(sym_symbol,inlet)) != NULL && TryMethSym(m,inlet,sym_symbol,s)) return true;
	}
	
	// otherwise search for anything
	if((m = (methitem *)methhead->Find(sym_anything,inlet)) != NULL && m->argc == 1 && m->args[0] == a_any && TryMethAny(m,inlet,sym_anything,s,argc,argv)) return true;
	if((m = (methitem *)clmethhead->Find(sym_anything,inlet)) != NULL && m->argc == 1 && m->args[0] == a_any && TryMethAny(m,inlet,sym_anything,s,argc,argv)) return true;

	// if nothing found try any inlet
	return inlet >= 0 && FindMeth(-1,s,argc,argv);
}

/*! \brief All the message processing
	The messages of all the inlets go here and are promoted to the registered callback functions
*/
bool flext_base::m_methodmain(int inlet,const t_symbol *s,int argc,const t_atom *argv)
{
	static bool trap = false;

	curtag = s;

//	post("methodmain inlet:%i args:%i symbol:%s",inlet,argc,s?GetString(s):"");
	
	bool ret = FindMeth(inlet,s,argc,argv);
	if(ret) goto end;

#if FLEXT_SYS == FLEXT_SYS_MAX
	// If float message is not explicitly handled: try int handler instead
	if(argc == 1 && s == sym_float && !trap) {
		t_atom fl;
		SetInt(fl,GetAInt(argv[0]));
		trap = true;
		ret = m_methodmain(inlet,sym_int,1,&fl);
		trap = false;
	}
	if(ret) goto end;
	
	// If int message is not explicitly handled: try float handler instead
	if(argc == 1 && s == sym_int && !trap) {
		t_atom fl;
		SetFloat(fl,GetAFloat(argv[0]));
		trap = true;
		ret = m_methodmain(inlet,sym_float,1,&fl);
		trap = false;
	}
	if(ret) goto end;
#endif
	
	// If float or int message is not explicitly handled: try list handler instead
	if(!trap && argc == 1 && (s == sym_float
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
	if(ret) goto end;

	// If symbol message (pure anything without args) is not explicitly handled: try list handler instead
	if(!trap && argc == 0) {
		t_atom list;
		SetSymbol(list,s);
		trap = true;
		ret = m_methodmain(inlet,sym_list,1,&list);
		trap = false;
	}
	if(ret) goto end;

	// if distmsgs is switched on then distribute list elements over inlets (Max/MSP behavior)
	if(distmsgs && !trap && inlet == 0 && s == sym_list && insigs <= 1) {
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
	
end:
	curtag = NULL;

	return ret; // true if appropriate handler was found and called
}

bool flext_base::m_method_(int inlet,const t_symbol *s,int argc,const t_atom *argv) 
{
//#ifdef FLEXT_DEBUG
	post("%s: message unhandled - inlet:%i args:%i symbol:%s",thisName(),inlet,argc,s?GetString(s):"");
//#endif
	return false;
}

