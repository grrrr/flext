/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flmeth.cpp
    \brief Method processing of flext base class.
*/
 
#include "flext.h"
#include <string.h>
#include <stdarg.h>
#include "flinternal.h"

#include <set>


flext_base::MethItem::MethItem(int in,const t_symbol *tg,AttrItem *conn): 
	Item(tg,in,conn),index(0),
	argc(0),args(NULL)
	,fun(NULL)
{}

flext_base::MethItem::~MethItem() 
{ 
	if(args) delete[] args; 
}

void flext_base::MethItem::SetArgs(methfun _fun,int _argc,metharg *_args)
{
	fun = _fun;
	if(args) delete[] args;
	argc = _argc,args = _args;
}


void flext_base::AddMethodDef(int inlet,const char *tag)
{
	methhead->Add(new MethItem(inlet,tag?MakeSymbol(tag):NULL));
}

/*! \brief Add a method to the queue
*/
void flext_base::AddMethod(ItemCont *ma,int inlet,const char *tag,methfun fun,metharg tp,...)
{
	va_list marker; 

	// at first just count the arg type list (in argc)
	int argc = 0;
	va_start(marker,tp);
	metharg *args = NULL,arg = tp;
	for(; arg != a_null; ++argc) arg = (metharg)va_arg(marker,int); //metharg);
	va_end(marker);
	
	if(argc > 0) {
		if(argc > FLEXT_MAXMETHARGS) {
			error("flext - method %s: only %i arguments are type-checkable: use variable argument list for more",tag?tag:"?",FLEXT_MAXMETHARGS);
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
	
	MethItem *mi = new MethItem(inlet,MakeSymbol(tag));

	mi->SetArgs(fun,argc,args);

	ma->Add(mi);

	// set index
	MethItem *last = (MethItem *)ma->Last();
	if(last) mi->index = last->index+1;
}


struct methless : public std::binary_function <flext_base::MethItem *,flext_base::MethItem *, bool> 
{
	bool operator()(const flext_base::MethItem *l,const flext_base::MethItem *r) const { 
		return l->index != r->index?l->index < r->index:strcmp(flext::GetString(l->tag),flext::GetString(r->tag)) < 0; 
	}
};

void flext_base::ListMethods(AtomList &la,int inlet) const
{
	typedef std::set<MethItem *,methless> MethList;
	MethList list[2];

	for(int i = 0; i <= 1; ++i) {
		ItemCont *a = i?methhead:clmethhead;
		if(a) {
			for(int ai = 0; ai < a->Size(); ++ai) {
				for(Item *l = a->GetItem(ai); l; l = l->nxt) {
					MethItem *aa = (MethItem *)l;

					// match inlet and see check it's not related to an attribute
					if(aa->inlet == inlet && !aa->IsAttr())
						list[i].insert(aa);
				}
			}
		}
	}

	la(list[0].size()+list[1].size());
	int ix = 0;
	MethList::iterator it;
	for(i = 0; i <= 1; ++i)
		for(it = list[i].begin(); it != list[i].end(); ++it) 
			SetSymbol(la[ix++],(*it)->tag);
}

bool flext_base::ListMethods(int inlet) const
{
	if(procattr) {
		AtomList la;
		ListMethods(la,inlet);
		ToOutAnything(GetOutAttr(),MakeSymbol("methods"),la.Count(),la.Atoms());
		return true;
	}
	else
		return false;
}

bool flext_base::cb_ListMethods(flext_base *c,int argc,const t_atom *argv) 
{ 
	if(argc == 0 || (argc == 1 && CanbeInt(argv[0])))
		return c->ListMethods(argc?GetAInt(argv[0]):0); 
	else
		return false;
}

