/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flatom_app.cpp
    \brief Definitions for handling the t_atom type and lists thereof.
*/
 
#include "flext.h"


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

