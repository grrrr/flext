/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flattr.cpp
    \brief Attribute handling for the flext base class
*/
 
#include "flext.h"
#include <string.h>
#include <stdio.h>

flext_base::attritem::attritem(const t_symbol *t,const t_symbol *gt,metharg tp,methfun gf,methfun sf):
	tag(t),gtag(gt),argtp(tp),gfun(gf),sfun(sf),nxt(NULL)
{}

flext_base::attritem::~attritem()
{
	if(nxt) delete nxt;
}

void flext_base::AddAttrItem(attritem *m)
{
	if(attrhead) {
		attritem *mi;
		for(mi = attrhead; mi->nxt; mi = mi->nxt) {}
		mi->nxt = m;
	}
	else 
		attrhead = m;
	attrcnt++;
}

void flext_base::AddAttrib(const char *attr,metharg tp,methfun gfun,methfun sfun)
{
	if(!procattr) {
		error("%s - Attribute processing is not enabled",thisName());
		return;
	}

	char tmp[1024];
	sprintf(tmp,"get%s",attr);
	AddAttrItem(new attritem(MakeSymbol(attr),MakeSymbol(tmp),tp,gfun,sfun));

	AddMethod(0,attr,(methfun)cb_SetAttrib,a_any,a_null);
	AddMethod(0,tmp,(methfun)cb_GetAttrib,a_any,a_null);
}

bool flext_base::InitAttrib(int argc,const t_atom *argv)
{
	if(procattr) {
		int nxt = 0,cur = 0;
		while(nxt < argc) {
			// find next @symbol
			cur = nxt;
			if(nxt) ++nxt;
			for(; nxt < argc; ++nxt)
				if(IsString(argv[nxt]) && *GetString(argv[nxt]) == '@') break;
			if(!cur) continue;

			const t_symbol *tag = MakeSymbol(GetString(argv[cur])+1);
			SetAttrib(tag,nxt-cur-1,argv+cur+1);
		}
	}
	return true;
}

bool flext_base::ListAttrib()
{
	AtomList la(attrcnt);
	attritem *a = attrhead;
	for(int i = 0; i < attrcnt; ++i,a = a->nxt) SetSymbol(la[i],a->tag);

	ToOutAnything(outattr,thisTag(),la.Count(),la.Atoms());
	return true;
}

bool flext_base::SetAttrib(const t_symbol *tag,int argc,const t_atom *argv)
{
	attritem *a = attrhead;
	for(; a && a->tag != tag; a = a->nxt) {}

	if(a) {
		if(a->sfun) {
			bool ok = true;

			AtomList la;
			t_any any;
			switch(a->argtp) {
			case a_float:
				if(argc == 1 && CanbeFloat(argv[0])) {
					any.ft = GetAFloat(argv[0]);
					((methfun_1)a->sfun)(this,any);				
				}
				else ok = false;
				break;
			case a_int:
				if(argc == 1 && CanbeInt(argv[0])) {
					any.it = GetAInt(argv[0]);
					((methfun_1)a->sfun)(this,any);				
				}
				else ok = false;
				break;
			case a_symbol:
				if(argc == 1 && IsSymbol(argv[0])) {
					any.st = GetSymbol(argv[0]);
					((methfun_1)a->sfun)(this,any);				
				}
				else ok = false;
				break;
			case a_LIST:
				any.vt = &(la(argc,argv));
				((methfun_1)a->sfun)(this,any);				
				break;
			default:
				ERRINTERNAL();
			}

			if(!ok)
				post("%s - wrong arguments for attribute %s",thisName(),GetString(tag));
		}
		else
			post("%s - attribute %s has no get method",thisName(),GetString(tag));
	}
	else
		error("%s - %s: attribute not found",thisName(),tag);
	return true;
}

bool flext_base::GetAttrib(const t_symbol *tag,int argc,const t_atom *argv)
{
	if(argc)
		post("%s - %s: arguments ignored",thisName(),GetString(tag));

	attritem *a = attrhead;
	for(; a && a->gtag != tag; a = a->nxt) {}

	if(a) {
		if(a->gfun) {
			AtomList la;
			t_any any;
			switch(a->argtp) {
			case a_float: {
				((methfun_1)a->gfun)(this,any);				
				la(1);
				SetFloat(la[0],any.ft);
				break;
			}
			case a_int: {
				((methfun_1)a->gfun)(this,any);				
				la(1);
				SetInt(la[0],any.it);
				break;
			}
			case a_symbol: {
				((methfun_1)a->gfun)(this,any);				
				la(1);
				SetSymbol(la[0],any.st);
				break;
			}
			case a_LIST: {
				any.vt = &la;
				((methfun_1)a->gfun)(this,any);				
				break;
			}
			default:
				ERRINTERNAL();
			}
			ToOutAnything(outattr,a->tag,la.Count(),la.Atoms());
		}
		else 
			post("%s - attribute %s has no set method",thisName(),GetString(tag));
	}
	else
		error("%s - %s: attribute not found",thisName(),tag);
	return true;
}

