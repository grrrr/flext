/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2004 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flitem.cpp
    \brief Processing of method and attribute lists.
*/
 
#include "flext.h"
#include <string.h>


flext_base::Item::~Item()
{
    if(nxt) delete nxt;
}

flext_base::ItemSet::ItemSet() {}

flext_base::ItemSet::~ItemSet()
{
    for(iterator it = begin(); it != end(); ++it)
        if(it.data()) delete it.data();
}

flext_base::ItemCont::ItemCont(): 
    members(0),memsize(0),size(0),cont(NULL)
{}

flext_base::ItemCont::~ItemCont()
{
    if(cont) {
        for(int i = 0; i < size; ++i) delete cont[i];
        delete[] cont;
    }
}

void flext_base::ItemCont::Resize(int nsz)
{
    if(nsz > memsize) {
        int nmemsz = nsz+10;  // increment maximum allocation size
        ItemSet **ncont = new ItemSet *[nmemsz]; // make new array
        if(cont) {
            memcpy(ncont,cont,size*sizeof(*cont)); // copy existing entries
            delete[] cont; 
        }
        cont = ncont;  // set current array
        memsize = nmemsz;  // set new allocation size
    }

    // make new items
    while(size < nsz) cont[size++] = new ItemSet;
}

void flext_base::ItemCont::Add(Item *item,const t_symbol *tag,int inlet)
{
    if(!Contained(inlet)) Resize(inlet+2);
    ItemSet &set = GetInlet(inlet);
    Item *&lst = set[tag];
    if(!lst) 
        lst = item;
    else
        for(;;)
            if(!lst->nxt) { lst->nxt = item; break; }
            else lst = lst->nxt;
    members++;
}

bool flext_base::ItemCont::Remove(Item *item,const t_symbol *tag,int inlet)
{
    if(Contained(inlet)) {
        ItemSet &set = GetInlet(inlet);
        ItemSet::iterator it = set.find(tag);
        if(it != set.end()) {
            for(Item *lit = it.data(),*prv = NULL; lit; prv = lit,lit = lit->nxt) {
                if(lit == item) {
                    if(prv) prv->nxt = lit->nxt;
                    else it.data() = lit->nxt;
                
                    lit->nxt = NULL; delete lit;
                    return true;
                }
            }
        }
    }
    return false;
}

flext_base::Item *flext_base::ItemCont::FindList(const t_symbol *tag,int inlet)
{
    if(Contained(inlet)) {
        ItemSet &ai = GetInlet(inlet);
        ItemSet::iterator as = ai.find(tag); 
        if(as != ai.end()) return as.data();
    }
    return NULL;
}

// --- class item lists (methods and attributes) ----------------

typedef DataMap<flext_base::t_classid,flext_base::ItemCont *> ClassMap;

static ClassMap classarr[2];

flext_base::ItemCont *flext_base::GetClassArr(t_classid c,int ix) 
{
    ClassMap &map = classarr[ix];
    ClassMap::iterator it = map.find(c);
    if(it == map.end()) {
        ItemCont *cont = new ItemCont;
        map[c] = cont;
        return cont;
    }
    else
        return it.data();
}
