/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flattr.cpp
    \brief Attribute handling for the flext base class
*/
 
#include "flext.h"
#include <string.h>

#include <set>


#ifdef __MWERKS__
#define STD std
#else
#define STD
#endif

flext_base::AttrItem::AttrItem(const t_symbol *t,metharg tp,methfun f,int fl):
	Item(t,0,NULL),index(0),
	flags(fl),argtp(tp),fun(f)
{}

flext_base::AttrItem::~AttrItem()
{
//	if(nxt) delete nxt;
}


//! Add get and set attributes
void flext_base::AddAttrib(ItemCont *aa,ItemCont *ma,const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	const t_symbol *asym = MakeSymbol(attr);

	if(sfun) // if commented out, there will be a warning at run-time (more user-friendly)
	{
		AttrItem *a = new AttrItem(asym,tp,sfun,AttrItem::afl_bothexist|AttrItem::afl_set);

		// set index of item to the next higher value
		AttrItem *last = (AttrItem *)aa->Last();
		if(last) a->index = last->index+1;

		aa->Add(a); 

		// bind attribute to a method
		MethItem *mi = new MethItem(0,asym,a);
		mi->SetArgs(sfun,1,new metharg(tp));
		ma->Add(mi);
	}

	if(gfun) // if commented out, there will be a warning at run-time (more user-friendly)
	{
		AttrItem *a = new AttrItem(asym,tp,gfun,AttrItem::afl_bothexist|AttrItem::afl_get);

		// set index of item to the next higher value
		AttrItem *last = (AttrItem *)aa->Last();
		if(last) a->index = last->index+1;

		aa->Add(a); 

		static char tmp[256] = "get";
		strcpy(tmp+3,attr);

		// bind attribute to a method
		MethItem *mi = new MethItem(0,MakeSymbol(tmp),a);
		mi->SetArgs(gfun,0,NULL);
		ma->Add(mi);
	}
}

void flext_base::AddAttrib(const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	if(procattr)
		AddAttrib(ThAttrs(),ThMeths(),attr,tp,gfun,sfun);
	else
		error("%s - attribute procession is not enabled!",thisName());
}

void flext_base::AddAttrib(t_classid c,const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	AddAttrib(ClAttrs(c),ClMeths(c),attr,tp,gfun,sfun);
}

/*
//! Sorting function for pure symbol atom lists (used with qsort below)
static int sortcmp(const void *a, const void *b) 
{ 
	return strcmp(flext::GetString(*(t_atom *)a),flext::GetString(*(t_atom *)b)); 
}
*/

struct attrless : public std::binary_function<flext_base::AttrItem *,flext_base::AttrItem *, bool> 
{
	bool operator()(const flext_base::AttrItem *l,const flext_base::AttrItem *r) const { 
		return l->index != r->index?l->index < r->index:strcmp(flext::GetString(l->tag),flext::GetString(r->tag)) < 0; 
	}
};

void flext_base::ListAttrib(AtomList &la) const
{
	typedef std::set<AttrItem *,attrless> AttrList;
	AttrList list[2];

	for(int i = 0; i <= 1; ++i) {
		ItemCont *a = i?attrhead:clattrhead;
		if(a) {
			for(int ai = 0; ai < a->Size(); ++ai) {
				for(Item *l = a->GetItem(ai); l; l = l->nxt) {
					AttrItem *aa = (AttrItem *)l;

					// only list once!
					if(!aa->BothExist() || aa->IsGet()) 
						list[i].insert(aa);
				}
			}
		}
	}

	la(list[0].size()+list[1].size());
	int ix = 0;
	AttrList::iterator it;
	for(i = 0; i <= 1; ++i)
		for(it = list[i].begin(); it != list[i].end(); ++it) 
			SetSymbol(la[ix++],(*it)->tag);
}

int flext_base::CheckAttrib(int argc,const t_atom *argv)
{
	int offs = 0;
	for(; offs < argc; ++offs)
		if(IsString(argv[offs]) && *GetString(argv[offs]) == '@') break;
	return offs;
}

bool flext_base::InitAttrib(int argc,const t_atom *argv)
{
	int cur,nxt;
	for(cur = 0; cur < argc; cur = nxt) {
		// find next @symbol
		for(nxt = cur+1; nxt < argc; ++nxt)
			if(IsString(argv[nxt]) && *GetString(argv[nxt]) == '@') break;

		const t_symbol *tag = MakeSymbol(GetString(argv[cur])+1);

		// find puttable attribute
		AttrItem *attr = FindAttrib(tag,false,true);
		if(attr) {
			// make an entry (there are none beforehand...)
			AttrData &a = (*attrdata)[tag];
			a.SetInit(true);
			a.SetInitValue(nxt-cur-1,argv+cur+1);

			// pass value to object
			SetAttrib(attr,a.GetInitValue());
		}
	}
	return true;
}

bool flext_base::ListAttrib() const
{
	if(procattr) {
		AtomList la;
		ListAttrib(la);
		ToOutAnything(GetOutAttr(),MakeSymbol("attributes"),la.Count(),la.Atoms());
		return true;
	}
	else
		return false;
}

flext_base::AttrItem *flext_base::FindAttrib(const t_symbol *tag,bool get,bool msg) const
{
    // first search within object scope
	AttrItem *a = (AttrItem *)attrhead->Find(tag);
	while(a && (a->tag != tag || a->inlet != 0 || (get?a->IsSet():a->IsGet()))) a = (AttrItem *)a->nxt;

    // then (if nothing found) search within class scope
	if(!a) {
		a = (AttrItem *)clattrhead->Find(tag);	
		while(a && (a->tag != tag || a->inlet != 0 || (get?a->IsSet():a->IsGet()))) a = (AttrItem *)a->nxt;
	}

	if(!a && msg) {
		// print a message
		error("%s - %s: attribute not found",thisName(),GetString(tag));
	}
	return a;
}

bool flext_base::SetAttrib(const t_symbol *tag,int argc,const t_atom *argv)
{
	// search for matching attribute
	AttrItem *a = FindAttrib(tag,false,true);
	if(a) 
		return SetAttrib(a,argc,argv);
	else
		return true;
}

bool flext_base::SetAttrib(AttrItem *a,int argc,const t_atom *argv)
{
	if(a->fun) {
		bool ok = true;

		AtomList la;
		t_any any;
		switch(a->argtp) {
		case a_float:
			if(argc == 1 && CanbeFloat(argv[0])) {
				any.ft = GetAFloat(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_int:
			if(argc == 1 && CanbeInt(argv[0])) {
				any.it = GetAInt(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_symbol:
			if(argc == 1 && IsSymbol(argv[0])) {
				any.st = GetSymbol(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_LIST:
			any.vt = &(la(argc,argv));
			((methfun_1)a->fun)(this,any);				
			break;
		default:
			ERRINTERNAL();
		}

		if(!ok)
			post("%s - wrong arguments for attribute %s",thisName(),GetString(a->tag));
	}
	else
		post("%s - attribute %s has no get method",thisName(),GetString(a->tag));
	return true;
}


bool flext_base::GetAttrib(AttrItem *a,AtomList &la) const
{
	bool ok = true;
	// main attribute tag
	if(a) {
		if(a->fun) {
			t_any any;
			switch(a->argtp) {
			case a_float: {
				((methfun_1)a->fun)(const_cast<flext_base *>(this),any);				
				la(1);
				SetFloat(la[0],any.ft);
				break;
			}
			case a_int: {
				((methfun_1)a->fun)(const_cast<flext_base *>(this),any);				
				la(1);
				SetInt(la[0],any.it);
				break;
			}
			case a_symbol: {
				((methfun_1)a->fun)(const_cast<flext_base *>(this),any);				
				la(1);
				SetSymbol(la[0],any.st);
				break;
			}
			case a_LIST: {
				any.vt = &la;
				((methfun_1)a->fun)(const_cast<flext_base *>(this),any);				
				break;
			}
			default:
				ERRINTERNAL();
				ok = false;
			}
		}
		else {
			post("%s - attribute %s has no get method",thisName(),GetString(a->tag));
			ok = false;
		}
	}
	else {
		error("%s - %s: attribute not found",thisName(),GetString(a->tag));
		ok = false;
	}
	return ok;
}

bool flext_base::GetAttrib(AttrItem *a)
{
	AtomList la;
	bool ret = GetAttrib(a,la);
	if(ret) ToOutAnything(GetOutAttr(),a->tag,la.Count(),la.Atoms());
	return ret;
}

bool flext_base::GetAttrib(const t_symbol *s,AtomList &a) const
{
	AttrItem *attr = FindAttrib(s,true);
	return attr && GetAttrib(attr,a);
}


bool flext_base::DumpAttrib(const t_symbol *attr) const
{
	AttrItem *item = FindAttrib(attr,true);
	return item && const_cast<flext_base *>(this)->GetAttrib(item);
}
