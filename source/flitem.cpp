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

void flext_base::ItemCont::Add(Item *item,const t_symbol *tag,int inlet)
{
    FLEXT_ASSERT(inlet >= -1);
    if(!Contained(inlet)) resize(inlet+2);
    ItemSet &set = GetInlet(inlet);
    set[tag].push_back(item);
    members++;
}

bool flext_base::ItemCont::Remove(Item *item,const t_symbol *tag,int inlet)
{
    FLEXT_ASSERT(inlet >= -1);
    if(Contained(inlet)) {
        ItemSet &set = GetInlet(inlet);
        ItemSet::iterator it = set.find(tag);
        if(it != set.end()) {
            ItemList &lst = it->second;
            for(ItemList::iterator lit = lst.begin(); lit != lst.end(); ++lit) {
                if(*lit == item) {
                    delete *lit;
                    lst.erase(lit);
                    return true;
                }
            }
        }
    }
    return false;
}

flext_base::ItemList *flext_base::ItemCont::FindList(const t_symbol *tag,int inlet)
{
    FLEXT_ASSERT(inlet >= -1);
    if(Contained(inlet)) {
        ItemSet &ai = GetInlet(inlet);
        ItemSet::iterator as = ai.find(tag); 
        if(as != ai.end()) return &as->second;
    }
    return NULL;
}

// --- class item lists (methods and attributes) ----------------

typedef std::map<flext_base::t_classid,flext_base::ItemCont *> ClassMap;
typedef std::vector<ClassMap> ClassArr;

static ClassArr classarr;

flext_base::ItemCont *flext_base::GetClassArr(t_classid c,int ix) 
{
    if(ix >= (int)classarr.size()) classarr.resize(ix+1);
    ClassMap &map = classarr[ix];
    ClassMap::iterator it = map.find(c);
    if(it == map.end()) {
        ItemCont *cont = new ItemCont;
        map[c] = cont;
        return cont;
    }
    else
        return it->second;
}
