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
/*
	if(incnt) {
		post("%s - Attributes must be defined prior to SetupInOut()",thisName());
		return;
	}
*/
	char tmp[1024];
	sprintf(tmp,"get%s",attr);
	AddAttrItem(new attritem(MakeSymbol(attr),MakeSymbol(tmp),tp,gfun,sfun));

	if(sfun)
		AddMethod(0,attr,(methfun)cb_SetAttrib,a_any,a_null);
	else
		AddMethod(0,attr,(methfun)cb_NoSetAttrib,a_any,a_null);

	if(gfun)
		AddMethod(0,tmp,(methfun)cb_GetAttrib,a_any,a_null);
	else
		AddMethod(0,tmp,(methfun)cb_NoGetAttrib,a_any,a_null);
}

bool flext_base::ListAttrib()
{
	if(outattr) {
		AtomList la(attrcnt);
		attritem *a = attrhead;
		for(int i = 0; i < attrcnt; ++i,a = a->nxt) SetSymbol(la[i],a->tag);

		ToOutAnything(outattr,thisTag(),la.Count(),la.Atoms());
		return true;
	}
	else return false;
}

bool flext_base::cb_NoGetAttrib(flext_base *c,const t_symbol *tag,int argc,const t_atom *argv)
{
	if(c->outattr) {
		post("%s - attribute %s has no get method",c->thisName(),GetString(tag));
		return true;
	}
	else
		return false;
}

bool flext_base::cb_NoSetAttrib(flext_base *c,const t_symbol *tag,int argc,const t_atom *argv)
{
	if(c->outattr) {
		post("%s - attribute %s has no set method",c->thisName(),GetString(tag));
		return true;
	}
	else
		return false;
}

bool flext_base::SetAttrib(const t_symbol *tag,int argc,const t_atom *argv)
{
	if(outattr) {
		attritem *a = attrhead;
		for(; a && a->tag != tag; a = a->nxt) {}

		if(a) {
			bool ok = true;

			AtomList la;
			t_any any;
			switch(a->argtp) {
			case a_float:
				if(argc == 1 && CanbeFloat(argv[0])) {
					any.ft = GetAFloat(argv[0]);
					((methfun_1)a->gfun)(this,any);				
				}
				else ok = false;
				break;
			case a_int:
				if(argc == 1 && CanbeInt(argv[0])) {
					any.it = GetAInt(argv[0]);
					((methfun_1)a->gfun)(this,any);				
				}
				else ok = false;
				break;
			case a_symbol:
				if(argc == 1 && IsSymbol(argv[0])) {
					any.st = GetSymbol(argv[0]);
					((methfun_1)a->gfun)(this,any);				
				}
				else ok = false;
				break;
			case a_LIST:
				any.vt = &(la(argc,argv));
				((methfun_1)a->gfun)(this,any);				
				break;
			default:
				ERRINTERNAL();
			}

			if(ok)
				ToOutAnything(outattr,tag,la.Count(),la.Atoms());
			else
				post("%s - wrong arguments for attribute %s",thisName(),GetString(tag));
		}
		else
			error("%s - %s: attribute not found",thisName(),tag);
		return true;
	}
	else return false;
}

bool flext_base::GetAttrib(const t_symbol *tag,int argc,const t_atom *argv)
{
	if(outattr) {
		if(argc)
			post("%s - %s: arguments ignored",thisName(),GetString(tag));

		attritem *a = attrhead;
		for(; a && a->tag != tag; a = a->nxt) {}

		if(a) {
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
			ToOutAnything(outattr,tag,la.Count(),la.Atoms());
		}
		else
			error("%s - %s: attribute not found",thisName(),tag);
		return true;
	}
	else return false;
}

