/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flitem.cpp
    \brief Processing of method and attribute lists.
*/
 
#include "flext.h"
#include <string.h>


flext_base::item::item(const t_symbol *t,int inl): 
 inlet(inl),tag(t),nxt(NULL) 
{}

flext_base::item::~item()
{
	if(nxt) delete nxt;
}


flext_base::itemarr::itemarr():
	arr(new item *[2]),cnt(0),bits(-1)
{
	arr[0] = arr[1] = NULL;
}

flext_base::itemarr::~itemarr()
{
	int c = Ready()?Size():2;

	for(int i = 0; i < c; ++i)
		if(arr[i]) delete arr[i];
	delete[] arr;
}

void flext_base::itemarr::Add(item *it)
{
#ifdef FLEXT_DEBUG
	if(Ready()) ERRINTERNAL();
	else
#endif
	{
//		post("ADD %i,%s",it->inlet,GetString(it->tag));			

		if(arr[0]) arr[1] = arr[1]->nxt = it;
		else arr[0] = arr[1] = it;
		++cnt;
	}
}

void flext_base::itemarr::Finalize()
{
#ifdef FLEXT_DEBUG
	if(Ready()) ERRINTERNAL();
	else
#endif
	{
		bits = Int2Bits(cnt); // at least enough bits to hold all items
		
		int sz = Size();

		post("count=%i, bit=%i size=%i",Count(),bits,sz);

		// save stored item list
		item *lst = arr[0];
		
		delete[] arr;
		arr = new item *[sz];
		memset(arr,0,sz*sizeof(*arr));
		
		while(lst) {
			item *l = lst;
			lst = lst->nxt;
			l->nxt = NULL;

			// retrieve array index
			int ix = Hash(l->tag,l->inlet,bits);

//			post("ADD %i,%s -> index %i",l->inlet,GetString(l->tag),ix);			

			// add to array slot
			if(arr[ix]) {
				item *a = arr[ix];
				while(a->nxt) a = a->nxt;
				a->nxt = l;
			}
			else arr[ix] = l;
		}
		
		if(Count()) {
			static char usage[1024];
			int c = 0,i;
			for(i = 0; i < sz; ++i) {
				usage[i] = arr[i]?'x':'.'; 
				if(arr[i]) ++c;
			}
			usage[i] = 0;
			post("USAGE %s",usage);
			post("USAGE %i/%i ... sparseness=%i%%",c,Count(),(int)((float)c/Count()*100.));
		}
	}
}

flext_base::item *flext_base::itemarr::Find(const t_symbol *tag,int inlet) const
{
#ifdef FLEXT_DEBUG
	if(!Ready()) {
		ERRINTERNAL();
		return NULL;
	}
	else
#endif
	if(Count()) {
		int ix = Hash(tag,inlet,bits);
		return arr[ix];
	}
	else
		return NULL;
}

int flext_base::itemarr::Hash(const t_symbol *tag,int inlet,int bits)
{
	unsigned long h = ((reinterpret_cast<unsigned long>(tag)&~7L)<<2)+inlet;
	
	return FoldBits(h,bits);
}


flext_base::itemarr *flext_base::GetClassArr(int ix) const
{
	// class item array should be retrieved here
	return new itemarr;
}
