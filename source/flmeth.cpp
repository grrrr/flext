/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flmeth.cpp
    \brief Method processing of flext base class.
*/
 
#include "flext.h"
#include <string.h>
#include <stdarg.h>
#include "flinternal.h"

flext_base::MethItem::MethItem(AttrItem *conn): 
    Item(conn),index(0),
    argc(0),args(NULL)
    ,fun(NULL)
{}

flext_base::MethItem::~MethItem() 
{ 
    if(args) delete[] args; 
}

void flext_base::MethItem::SetArgs(methfun _fun,int _argc,metharg *_args)
{
    fun = _fun;
    if(args) delete[] args;
    argc = _argc,args = _args;
}

/*! \brief Add a method to the queue
*/
void flext_base::AddMethod(ItemCont *ma,int inlet,const t_symbol *tag,methfun fun,metharg tp,...)
{
    va_list marker; 

    // at first just count the arg type list (in argc)
    int argc = 0;
    va_start(marker,tp);
    metharg *args = NULL,arg = tp;
    for(; arg != a_null; ++argc) arg = (metharg)va_arg(marker,int); //metharg);
    va_end(marker);
    
    if(argc > 0) {
        if(argc > FLEXT_MAXMETHARGS) {
            error("flext - method %s: only %i arguments are type-checkable: use variable argument list for more",tag?GetString(tag):"?",FLEXT_MAXMETHARGS);
            argc = FLEXT_MAXMETHARGS;
        }

        args = new metharg[argc];

        va_start(marker,tp);
        metharg a = tp;
        for(int ix = 0; ix < argc; ++ix) {
#ifdef FLEXT_DEBUG
            if(a == a_list && ix > 0) {
                ERRINTERNAL();
            }
#endif
#if FLEXT_SYS == FLEXT_SYS_PD
            if(a == a_pointer && flext_base::compatibility) {
                post("Pointer arguments are not allowed in compatibility mode"); 
            }
#endif
            args[ix] = a;
            a = (metharg)va_arg(marker,int); //metharg);
        }
        va_end(marker);
    }
    
    MethItem *mi = new MethItem;
    mi->index = ma->Members();
    mi->SetArgs(fun,argc,args);
    ma->Add(mi,tag,inlet);
}

void flext_base::ListMethods(AtomList &la,int inlet) const
{
	typedef TablePtrMap<int,const t_symbol *,32> MethList;
    MethList list[2];
    ItemCont *clmethhead = ClMeths(thisClassId());

    int i;
    for(i = 0; i <= 1; ++i) {
        ItemCont *a = i?methhead:clmethhead;
        if(a && a->Contained(inlet)) {
            ItemSet &ai = a->GetInlet(inlet);
            for(ItemSet::iterator as(ai); as; ++as) {
                for(Item *al = as.data(); al; al = al->nxt) {
                    MethItem *aa = (MethItem *)al;
                    // check it's not related to an attribute
                    if(!aa->IsAttr()) {
                        list[i].insert(aa->index,as.key());
                        break;
                    }
                }
            }
        }
    }

    la((int)list[0].size()+(int)list[1].size());
    int ix = 0;
    for(i = 0; i <= 1; ++i)
        for(MethList::iterator it(list[i]); it; ++it) 
            SetSymbol(la[ix++],it.data());
}

bool flext_base::cb_ListMethods(flext_base *c,int argc,const t_atom *argv) 
{ 
    if(c->HasAttributes() && (argc == 0 || (argc == 1 && CanbeInt(argv[0])))) {
        // defined in flsupport.cpp
        extern const t_symbol *sym_methods;

        int inlet = argc?GetAInt(argv[0]):0;
        AtomListStatic<32> la;
        c->ListMethods(la,inlet);
        c->ToOutAnything(c->GetOutAttr(),sym_methods,la.Count(),la.Atoms());
        return true;
    }
    else
        return false;
}

