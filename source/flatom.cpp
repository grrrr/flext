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

//namespace flext {

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

flext::AtomList &flext::AtomList::operator()(int argc,const t_atom *argv)
{
	if(lst && cnt != argc) { delete[] lst; lst = NULL; cnt = 0; }

	if(argc) {
		cnt = argc;
		lst = new t_atom[cnt];

		if(argv) {
			for(int i = 0; i < argc; ++i) SetAtom(lst[i],argv[i]);
/*			
			{
				switch(lst[i].a_type = argv[i].a_type) {
				case A_FLOAT:
					lst[i].a_w.w_float = argv[i].a_w.w_float;
					break;
#ifdef MAXMSP
				case A_LONG:
					lst[i].a_w.w_int = argv[i].a_w.w_int;
					break;
#endif
				case A_SYMBOL:
					lst[i].a_w.w_symbol = argv[i].a_w.w_symbol;
					break;
#ifdef PD
				case A_POINTER:
					lst[i].a_w.w_gpointer = argv[i].a_w.w_gpointer;
					break;
#endif
				default:
					post("AtomList - atom type (%i) not supported",lst[i].a_type);
					lst[i].a_type = A_NULL;
					break;
				}
			}
*/
		}
	}
	return *this;
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

//} // namespace flext
