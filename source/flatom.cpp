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


flext::AtomList::AtomList(int argc,const t_atom *argv):
	lst(NULL),cnt(0)
{
	operator()(argc,argv);
}

flext::AtomList::AtomList(const AtomList &a):
	lst(NULL),cnt(0)
{
	operator =(a);
}

flext::AtomList::~AtomList() {	Clear(); }

flext::AtomList &flext::AtomList::Set(int argc,const t_atom *argv)
{
	if(lst && cnt != argc) { delete[] lst; lst = NULL; cnt = 0; }

	if(argc) {
		cnt = argc;
		lst = new t_atom[cnt];

		if(argv) {
			for(int i = 0; i < argc; ++i) SetAtom(lst[i],argv[i]);
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
	t_atom *nlst = new t_atom[cnt+argc];
	int i;
	for(i = 0; i < cnt; ++i) SetAtom(nlst[i],lst[i]);
	for(i = 0; i < argc; ++i) SetAtom(nlst[cnt+i],argv[i]);
	
	if(lst) delete[] lst;
	lst = nlst;
	cnt += argc;

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
	t_atom *nlst = new t_atom[cnt+argc];
	int i;
	for(i = 0; i < argc; ++i) SetAtom(nlst[i],argv[i]);
	for(i = 0; i < cnt; ++i) SetAtom(nlst[argc+i],lst[i]);
	
	if(lst) delete[] lst;
	lst = nlst;
	cnt += argc;

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

