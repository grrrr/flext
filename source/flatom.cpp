/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flatom.cpp
    \brief Definitions for handling the t_atom type and lists thereof.
*/
 
#include "flext.h"

t_atom *flext::CopyList(int argc,const t_atom *argv)
{
	int i;
	t_atom *dst = new t_atom[argc];
	for(i = 0; i < argc; ++i) CopyAtom(dst+i,argv+i);
	return dst;
}

flext::AtomList::AtomList(int argc,const t_atom *argv):
	cnt(0),lst(NULL)
{
	operator()(argc,argv);
}

flext::AtomList::AtomList(const AtomList &a):
	cnt(0),lst(NULL)
{
	operator =(a);
}

flext::AtomList::~AtomList() {	Clear(); }

flext::AtomList &flext::AtomList::Set(int argc,const t_atom *argv,int offs,bool resize)
{
	int ncnt = argc+offs;
	if(resize && lst && cnt != ncnt) { delete[] lst; lst = NULL; cnt = 0; }

	if(ncnt) {
		if(!lst) lst = new t_atom[cnt = ncnt];

		if(argv) {
			for(int i = 0; i < argc; ++i) SetAtom(lst[offs+i],argv[i]);
		}
	}
	return *this;
}

int flext::AtomList::Get(t_atom *argv,int mxsz) const
{
	int argc = Count();
	if(mxsz >= 0 && argc > mxsz) argc = mxsz;

	for(int i = 0; i < argc; ++i) SetAtom(argv[i],lst[i]);

	return argc;
}


flext::AtomList &flext::AtomList::Append(const t_atom &a)
{
	t_atom *nlst = new t_atom[cnt+1];
	for(int i = 0; i < cnt; ++i) SetAtom(nlst[i],lst[i]);
	SetAtom(nlst[cnt],a);
	
	if(lst) delete[] lst;
 	lst = nlst;
	++cnt;

	return *this;
}

flext::AtomList &flext::AtomList::Append(int argc,const t_atom *argv)
{
	if(argc) {
		t_atom *nlst = new t_atom[cnt+argc];
		int i;
		for(i = 0; i < cnt; ++i) SetAtom(nlst[i],lst[i]);
		if(argv) 
			for(i = 0; i < argc; ++i) SetAtom(nlst[cnt+i],argv[i]);
		
		if(lst) delete[] lst;
		lst = nlst;
		cnt += argc;
	}
	return *this;
}

flext::AtomList &flext::AtomList::Prepend(const t_atom &a)
{
	t_atom *nlst = new t_atom[cnt+1];
	for(int i = 0; i < cnt; ++i) SetAtom(nlst[i+1],lst[i]);
	SetAtom(nlst[0],a);
	
	if(lst) delete[] lst;
	lst = nlst;
	++cnt;

	return *this;
}

flext::AtomList &flext::AtomList::Prepend(int argc,const t_atom *argv)
{
	if(argc) {
		t_atom *nlst = new t_atom[cnt+argc];
		int i;

		if(argv) 
			for(i = 0; i < argc; ++i) SetAtom(nlst[i],argv[i]);
		for(i = 0; i < cnt; ++i) SetAtom(nlst[argc+i],lst[i]);
		
		if(lst) delete[] lst;
		lst = nlst;
		cnt += argc;
	}
	return *this;
}

flext::AtomList flext::AtomList::GetPart(int offs,int len) const
{
	if(offs+len > Count()) {
		len = Count()-offs;
		if(len < 0) len = 0;
	}

	return AtomList(len,Atoms()+offs);
}


flext::AtomAnything::AtomAnything(const t_symbol *h,int argc,const t_atom *argv): 
	AtomList(argc,argv),hdr(h?h:MakeSymbol("")) 
{}

flext::AtomAnything::AtomAnything(const char *h,int argc,const t_atom *argv): 
	AtomList(argc,argv),hdr(MakeSymbol(h)) 
{}

flext::AtomAnything::AtomAnything(const AtomAnything &a): 
	AtomList(a),hdr(a.hdr) 
{}


