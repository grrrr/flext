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
#include <string.h> // for memcpy

#if FLEXT_SYS != FLEXT_SYS_JMAX
int flext::CmpAtom(const t_atom &a,const t_atom &b)
{
	if(GetType(a) == GetType(b)) {
		switch(GetType(a)) {
			case A_FLOAT: return GetFloat(a) == GetFloat(b)?0:(GetFloat(a) < GetFloat(b)?-1:1);
#if FLEXT_SYS == FLEXT_SYS_MAX
			case A_INT: return GetInt(a) == GetInt(b)?0:(GetInt(a) < GetInt(b)?-1:1);
#endif
			case A_SYMBOL: return GetSymbol(a) == GetSymbol(b)?0:(GetSymbol(a) < GetSymbol(b)?-1:1);
#if FLEXT_SYS == FLEXT_SYS_PD
			case A_POINTER: return GetPointer(a) == GetPointer(b)?0:(GetPointer(a) < GetPointer(b)?-1:1);
#endif
			default:
				// can't be compared.....
				FLEXT_ASSERT(false);
				return 0;
		}
	}
	else
		return GetType(a) < GetType(b)?-1:1;
}
#else
#error Not implemented
#endif

t_atom *flext::CopyList(int argc,const t_atom *argv)
{
	t_atom *dst = new t_atom[argc];
    memcpy(dst,argv,argc*sizeof(t_atom));
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

int flext::AtomList::Compare(const AtomList &a) const
{
	if(Count() == a.Count()) {
		for(int i = 0; i < Count(); ++i) {
			int cmp = CmpAtom((*this)[i],a[i]);
			if(cmp) return cmp;
		}
		return 0;
	}
	else 
		return Count() < a.Count()?-1:1;
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


