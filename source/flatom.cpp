/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
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


#if FLEXT_SYS != FLEXT_SYS_JMAX 
// not for jmax as long as t_symbol * == char *
flext::AtomAnything::AtomAnything(const t_symbol *h,int argc,const t_atom *argv): 
	AtomList(argc,argv),hdr(h?h:MakeSymbol("")) 
{}
#endif

flext::AtomAnything::AtomAnything(const char *h,int argc,const t_atom *argv): 
	AtomList(argc,argv),hdr(MakeSymbol(h)) 
{}

flext::AtomAnything::AtomAnything(const AtomAnything &a): 
	AtomList(a),hdr(a.hdr) 
{}


