/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
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

void flext::AtomList::Alloc(int sz)
{
    if(lst) {
        if(cnt == sz) return; // no change
        delete[] lst;
    }
    else
        FLEXT_ASSERT(cnt == 0);
    lst = new t_atom[cnt = sz];
}

flext::AtomList::~AtomList() { Free(); }

void flext::AtomList::Free()
{
    if(lst) { 
        delete[] lst; lst = NULL; 
        cnt = 0;
    }
    else
        FLEXT_ASSERT(cnt == 0);
}

flext::AtomList &flext::AtomList::Set(int argc,const t_atom *argv,int offs,bool resize)
{
	int ncnt = argc+offs;
	if(resize) Alloc(ncnt);

    // argv can be NULL indepently from argc
    if(argv)
        for(int i = 0; i < argc; ++i) 
            SetAtom(lst[offs+i],argv[i]);

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

flext::AtomListStaticBase::~AtomListStaticBase() { Free(); }

void flext::AtomListStaticBase::Alloc(int sz) 
{ 
    if(sz < precnt) lst = predata,cnt = sz;
    else AtomList::Alloc(sz);
}

void flext::AtomListStaticBase::Free() 
{
    if(lst != predata) AtomList::Free();
    else lst = NULL,cnt = 0;
}
