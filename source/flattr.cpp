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
#include <ctype.h>

#include <set>


#ifdef __MWERKS__
#define STD std
#else
#define STD
#endif

flext_base::AttrItem::AttrItem(const t_symbol *t,metharg tp,methfun f,int fl):
	Item(NULL),index(0),
	flags(fl|afl_shown),
	argtp(tp),fun(f),tag(t),
	counter(NULL)
{}


flext_base::AttrDataCont::AttrDataCont() {}

flext_base::AttrDataCont::~AttrDataCont()
{
	for(iterator it = begin(); it != end(); ++it)
		if(it.data()) delete it.data();
}


//! Add get and set attributes
void flext_base::AddAttrib(ItemCont *aa,ItemCont *ma,const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	const t_symbol *asym = MakeSymbol(attr);
	AttrItem *a,*b;

	if(sfun) // if commented out, there will be a warning at run-time (more user-friendly)
	{
		a = new AttrItem(asym,tp,sfun,AttrItem::afl_set);
        a->index = aa->Members();
		aa->Add(a,asym); 

		// bind attribute to a method
		MethItem *mi = new MethItem(a);
		mi->SetArgs(sfun,1,new metharg(tp));
		ma->Add(mi,asym);
	}
	else
		a = NULL;

	if(gfun) // if commented out, there will be a warning at run-time (more user-friendly)
	{
		b = new AttrItem(asym,tp,gfun,AttrItem::afl_get);
        b->index = aa->Members();
		aa->Add(b,asym); 

		static char tmp[256] = "get";
		strcpy(tmp+3,attr);

		// bind attribute to a method
		MethItem *mi = new MethItem(b);
		mi->SetArgs(gfun,0,NULL);
		ma->Add(mi,MakeSymbol(tmp));
	}
	else
		b = NULL;

	if(a && b) {
		a->counter = b;
		b->counter = a;
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

void flext_base::ListAttrib(AtomList &la) const
{
	typedef DataMap<int,const t_symbol *> AttrList;
	AttrList list[2];

	int i;
	for(i = 0; i <= 1; ++i) {
        ItemCont *a = i?attrhead:clattrhead;
		if(a && a->Contained(0)) {
            ItemSet &ai = a->GetInlet();
            for(ItemSet::iterator as = ai.begin(); as != ai.end(); ++as) {
                for(Item *al = as.data(); al; al = al->nxt) {
					AttrItem *aa = (AttrItem *)al;
					list[i][aa->index] = as.key();
                    break;
                }
			}
		}
	}

	la(list[0].size()+list[1].size());
	int ix = 0;
	AttrList::iterator it;
	for(i = 0; i <= 1; ++i)
		for(it = list[i].begin(); it != list[i].end(); ++it) 
			SetSymbol(la[ix++],it.data());
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
			AttrDataCont::iterator it = attrdata->find(tag);
			if(it == attrdata->end()) {
				AttrDataCont::pair pair; 
				pair.key() = tag;
				pair.data() = new AttrData;
				it = attrdata->insert(attrdata->begin(),pair);
			}

			AttrData &a = *it.data();
			a.SetInit(true);
			a.SetInitValue(nxt-cur-1,argv+cur+1);

			// pass value to object
			SetAttrib(tag,attr,a.GetInitValue());
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
	AttrItem *a = NULL;
    {
        for(Item *lst = attrhead->FindList(tag); lst; lst = lst->nxt) {
            AttrItem *b = (AttrItem *)lst;
            if(get?b->IsGet():b->IsSet()) { a = b; break; }
        }
    }

    // then (if nothing found) search within class scope
	if(!a) {
        for(Item *lst = clattrhead->FindList(tag); lst; lst = lst->nxt) {
            AttrItem *b = (AttrItem *)lst;
            if(get?b->IsGet():b->IsSet()) { a = b; break; }
        }
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
		return SetAttrib(tag,a,argc,argv);
	else
		return true;
}

bool flext_base::SetAttrib(const t_symbol *tag,AttrItem *a,int argc,const t_atom *argv)
{
	if(a->fun) {
		bool ok = true;

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
				// \todo shall we analyze the patcher args????
//				any.st = const_cast<t_symbol *>(GetParamSym(GetSymbol(argv[0]),thisCanvas()));
				any.st = GetSymbol(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_bool:
			if(argc == 1 && CanbeBool(argv[0])) {
				any.bt = GetABool(argv[0]);
				((methfun_1)a->fun)(this,any);				
			}
			else ok = false;
			break;
		case a_LIST: {
			AtomList la(argc);
			for(int i = 0; i < argc; ++i)
				if(IsSymbol(argv[i])) 
					GetParamSym(la[i],GetSymbol(argv[i]),thisCanvas());
				else
					la[i] = argv[i];

			any.vt = &la;
			((methfun_1)a->fun)(this,any);				
			break;
		}
		default:
			ERRINTERNAL();
		}

		if(!ok)
			post("%s - wrong arguments for attribute %s",thisName(),GetString(tag));
	}
	else
		post("%s - attribute %s has no get method",thisName(),GetString(tag));
	return true;
}


bool flext_base::GetAttrib(const t_symbol *tag,AttrItem *a,AtomList &la) const
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
			case a_bool: {
				((methfun_1)a->fun)(const_cast<flext_base *>(this),any);				
				la(1);
				SetBool(la[0],any.bt);
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
			post("%s - attribute %s has no get method",thisName(),GetString(tag));
			ok = false;
		}
	}
	else {
		error("%s - %s: attribute not found",thisName(),GetString(tag));
		ok = false;
	}
	return ok;
}

bool flext_base::GetAttrib(const t_symbol *s,AtomList &a) const
{
	AttrItem *attr = FindAttrib(s,true);
	return attr && GetAttrib(s,attr,a);
}

//! \param tag symbol "get[attribute]"
bool flext_base::DumpAttrib(const t_symbol *tag,AttrItem *a) const
{
	AtomList la;
	bool ret = GetAttrib(tag,a,la);
	if(ret) {
		ToOutAnything(GetOutAttr(),a->tag,la.Count(),la.Atoms());
	}
	return ret;
}

bool flext_base::DumpAttrib(const t_symbol *attr) const
{
	AttrItem *item = FindAttrib(attr,true);
	return item && DumpAttrib(attr,item);
}

bool flext_base::BangAttrib(const t_symbol *attr,AttrItem *item)
{
	AtomList val;
	AttrItem *item2;
	if(!item->IsGet()) 
		item = item->Counterpart();
	if(item) {
		item2 = item->Counterpart();
		return item2 && GetAttrib(attr,item,val) && SetAttrib(attr,item2,val);
	}
	else
		return false;
}

bool flext_base::BangAttrib(const t_symbol *attr)
{
	AttrItem *item = FindAttrib(attr,true);
	return item && BangAttrib(attr,item);
}

bool flext_base::BangAttribAll()
{
	for(int i = 0; i <= 1; ++i) {
        ItemCont *a = i?attrhead:clattrhead;
		if(a) {
            ItemSet &ai = a->GetInlet(); // \todo need to check for presence of inlet 0?
            for(ItemSet::iterator as = ai.begin(); as != ai.end(); ++as) {
                for(Item *al = as.data(); al; al = al->nxt) {
					AttrItem *a = (AttrItem *)al;
	        		if(a->IsGet() && a->BothExist()) BangAttrib(as.key(),a);
                }
			}
		}
	}
	return true;
}

bool flext_base::ShowAttrib(AttrItem *a,bool show) const
{
	if(show) a->flags |= AttrItem::afl_shown;
	else a->flags &= ~AttrItem::afl_shown;

	// also change counterpart, if present
	AttrItem *ca = a->Counterpart();
	if(ca) {
		if(show) ca->flags |= AttrItem::afl_shown;
		else ca->flags &= ~AttrItem::afl_shown;
	}
	return true;
}

bool flext_base::ShowAttrib(const t_symbol *attr,bool show) const
{
	AttrItem *item = FindAttrib(attr,true);
	return item && ShowAttrib(item,show);
}
