/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flitem.cpp
    \brief Processing of method and attribute lists.
*/
 
#include "flext.h"
#include <string.h>


flext_base::item::item(const t_symbol *t,int inl,attritem *a): 
 tag(t),inlet(inl),attr(a),nxt(NULL) 
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
    if(Ready()) {
        // if finalized, the array has several slots
        int c = Size();
	    for(int i = 0; i < c; ++i)
		    if(arr[i]) delete arr[i];
    }
    else
        // the array only has a head (arr[0]) and a tail (arr[1]) pointer
        delete arr[0];

	delete[] arr;
}

void flext_base::itemarr::Add(item *it)
{
	if(Ready()) {
		// retrieve array index
		int ix = Hash(it->tag,it->inlet,bits);

		// add to array slot
		if(arr[ix]) {
			item *a = arr[ix];
			while(a->nxt) a = a->nxt;
			a->nxt = it;
		}
		else arr[ix] = it;

//		post("RDY inlet=%i,tag=%s,hash=%i",it->inlet,GetString(it->tag),ix);			
	}
	else {
//		post("ADD index=%i,inlet=%i,tag=%s",cnt,it->inlet,GetString(it->tag));			

		if(arr[0]) arr[1] = arr[1]->nxt = it;
		else arr[0] = arr[1] = it;
		++cnt;
	}
}

bool flext_base::itemarr::Remove(item *it)
{
	if(Ready()) {
		// retrieve array index
		int ix = Hash(it->tag,it->inlet,bits);

		// remove from array slot
		if(arr[ix]) {
			item *a1 = NULL,*a = arr[ix];
			while(a && a != it) a1 = a,a = a->nxt;
            if(a) { // found (a == it)
                if(a1) a1->nxt = it->nxt;
                else arr[ix] = it->nxt;
                it->nxt = NULL;
                return true;
            }
            else
                return false;
		}
		else 
            return false;
	}
	else {
        // remove from list
		if(!arr[0]) 
            return false;
        else {
			item *a1 = NULL,*a = arr[0];
			while(a && a != it) a1 = a,a = a->nxt;
            if(a) { // found (a == it)
                if(a1) a1->nxt = it->nxt;
                else arr[0] = it->nxt;
                if(!it->nxt) arr[1] = a1;
                it->nxt = NULL;
        		--cnt;
                return true;
            }
            else
                return false;
        }
	}
}

void flext_base::itemarr::Finalize()
{
	if(!Ready()) 
	{
		bits = Int2Bits(cnt); // at least enough bits to hold all items
		
//		post("This=%p, Count %i, Bits %i",this,cnt,bits);
		
		int sz = Size();

		// save stored item list
		item *lst = arr[0];
		
		delete[] arr;
		arr = new item *[sz];
		memset(arr,0,sz*sizeof(*arr));
		
		while(lst) {
			item *l = lst;
			lst = lst->nxt;
			l->nxt = NULL;

			Add(l);
		}

#if 0
		post("count=%i, bit=%i size=%i",Count(),bits,sz);

		if(Count()) {
			static char usage[1024];
			int c = 0,i;
			for(i = 0; i < sz; ++i) {
				usage[i] = arr[i]?'x':'.'; 
				if(arr[i]) ++c;
			}
			usage[i] = 0;
			post("USAGE %i/%i - sparse=%i%% %s",c,Count(),(int)((float)c/Count()*100.),usage);
		}
#endif
	}
}

flext_base::item *flext_base::itemarr::Find(const t_symbol *tag,int inlet) const
{
	item *a;
	if(!Ready())
		a = arr[0];
	else if(Count()) {
		int ix = Hash(tag,inlet,bits);
		a = arr[ix];
//		post("FIND tag=%s inlet=%i hash=%i p=%p",GetString(tag),inlet,ix,a);
	}
	else
		a = NULL;
		

	// Search first matching entry
	while(a && (a->tag != tag || a->inlet != inlet)) a = a->nxt;
	return a;
}

int flext_base::itemarr::Hash(const t_symbol *tag,int inlet,int bits)
{
	unsigned long h = ((reinterpret_cast<unsigned long>(tag)&~7L)<<1)+inlet;
	return FoldBits(h,bits);
}

// --- class item lists (methods and attributes) ----------------

class _itemarr 
{
public:
	enum { HASHBITS=7, HASHSIZE=1<<HASHBITS };

	_itemarr(flext_obj::t_classid c,int i);
	~_itemarr();

	static int Hash(flext_obj::t_classid c,int ix);

	int Hash() const { return Hash(clid,ix); }
	void Add(_itemarr *a);

	flext_obj::t_classid clid;
	int ix;
	flext_base::itemarr *arr;

	_itemarr *nxt;
};

_itemarr::_itemarr(flext_obj::t_classid c,int i):
	clid(c),ix(i),
	arr(new flext_base::itemarr),
	nxt(NULL)
{}

_itemarr::~_itemarr()
{
	delete arr;
	if(nxt) delete nxt;
}

void _itemarr::Add(_itemarr *a)
{
	if(nxt) nxt->Add(a);
	else nxt = a;
}

int _itemarr::Hash(flext_obj::t_classid c,int ix) 
{
	unsigned long h = (reinterpret_cast<unsigned long>(c)&~3L)+ix;
	return flext::FoldBits(h,HASHBITS);
}

static _itemarr **_arrs = NULL;

flext_base::itemarr *flext_base::GetClassArr(t_classid c,int ix) 
{
	if(!_arrs) {
		_arrs = new _itemarr *[_itemarr::HASHSIZE];
		memset(_arrs,0,_itemarr::HASHSIZE*sizeof(*_arrs));
	}

	int hash = _itemarr::Hash(c,ix);
	_itemarr *a = _arrs[hash];
	_itemarr *pa = NULL;
	while(a && (a->clid != c || a->ix != ix)) pa = a,a = a->nxt;

//	post("GETARR classid=%p ix=%i -> hash=%i,arr=%p",c,ix,hash,a?a->arr:NULL);

	if(!a) {
		a = new _itemarr(c,ix);
		if(pa) 
			// previous entry... extend
			a->nxt = pa->nxt,pa->nxt = a;
		else 
			// new singular entry
			_arrs[hash] = a;
	}

	return a->arr;
}
