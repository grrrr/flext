/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flbind.cpp
    \brief Functionality for symbol-bound methods.
*/
 
#include "flext.h"
#include "flinternal.h"


t_class *flext_base::pxbnd_class = NULL;

#if FLEXT_SYS == FLEXT_SYS_MAX
t_object *px_freelist = NULL;
t_messlist px_messlist[3];
#endif

/*! \brief Set up the proxy class for symbol-bound methods
*/
void flext_base::SetupBindProxy()
{
	// already initialized?
	if(!pxbnd_class) {
#if FLEXT_SYS == FLEXT_SYS_PD
    	pxbnd_class = class_new(gensym("flext_base bind proxy"),NULL,NULL,sizeof(pxbnd_object),CLASS_PD|CLASS_NOINLET, A_NULL);
		add_anything(pxbnd_class,pxbnd_object::px_method); // for symbol-bound methods
#elif FLEXT_SYS == FLEXT_SYS_MAX
		pxbnd_class = new t_class;

		pxbnd_class->c_sym = gensym("");
		pxbnd_class->c_freelist = &px_freelist;
		pxbnd_class->c_freefun = NULL;
		pxbnd_class->c_size = sizeof(pxbnd_object);
		pxbnd_class->c_tiny = 0;
		pxbnd_class->c_noinlet = 1;
		px_messlist[0].m_sym = (t_symbol *)pxbnd_class;

		px_messlist[1].m_sym = gensym("anything");
		px_messlist[1].m_fun = (method)pxbnd_object::px_method;
		px_messlist[1].m_type[0] = A_GIMME;
		px_messlist[1].m_type[1] = 0;

		px_messlist[2].m_sym = 0;
#else
#pragma warning("Not implemented!")
#endif
	}
}


flext_base::BindItem::BindItem(bool (*f)(flext_base *,t_symbol *s,int,t_atom *,void *data),pxbnd_object *p):
	Item(NULL),fun(f),px(p)
{}

flext_base::BindItem::~BindItem()
{
    if(px) object_free(&px->obj);
}

void flext_base::BindItem::Unbind(const t_symbol *tag)
{
    if(px) {
#if FLEXT_SYS == FLEXT_SYS_PD
        pd_unbind(&px->obj.ob_pd,const_cast<t_symbol *>(tag)); 
#elif FLEXT_SYS == FLEXT_SYS_MAX
        if(tag->s_thing == (t_object *)px) 
            const_cast<t_symbol *>(tag)->s_thing = NULL; 
        else
            error("flext - Binding to symbol %s not found",tag->s_name);
#else
#           pragma warning("Not implemented")
#endif
    }
}

#if FLEXT_SYS == FLEXT_SYS_PD
    //! Bind object to a symbol
    bool flext_base::Bind(const t_symbol *sym) { pd_bind(&thisHdr()->ob_pd,const_cast<t_symbol *>(sym)); return true; }
    //! Unbind object from a symbol
    bool flext_base::Unbind(const t_symbol *sym) { pd_unbind(&thisHdr()->ob_pd,const_cast<t_symbol *>(sym)); return true; }
#elif FLEXT_SYS == FLEXT_SYS_MAX
    //! Bind object to a symbol
    bool flext_base::Bind(const t_symbol *sym) { if(sym->s_thing) return false; else { const_cast<t_symbol *>(sym)->s_thing = (t_object *)thisHdr(); return true; } }
    //! Unbind object from a symbol
    bool flext_base::Unbind(const t_symbol *sym) { if(sym->s_thing != (t_object *)thisHdr()) return false; else { const_cast<t_symbol *>(sym)->s_thing = NULL; return true; } }
#endif

bool flext_base::BindMethod(const t_symbol *sym,bool (*fun)(flext_base *,t_symbol *s,int argc,t_atom *argv,void *data),void *data)
{
    if(!bindhead) 
        bindhead = new ItemCont;
    else {
        // Search for symbol
        for(Item *it = bindhead->FindList(sym); it; it = it->nxt) {
            BindItem *item = (BindItem *)it;

            // go through all items with matching tag
            if(item->fun == fun) {
                // function already registered -> bail out!
                post("%s - Symbol already bound with this method",thisName());
                return false;
            }
        }
    }

    SetupBindProxy(); 

#if FLEXT_SYS == FLEXT_SYS_PD
    pxbnd_object *px = (pxbnd_object *)object_new(pxbnd_class);
#elif FLEXT_SYS == FLEXT_SYS_MAX
    pxbnd_object *px = (pxbnd_object *)newobject(px_messlist);
#else
#pragma warning("Not implemented!")
#endif

    if(px) {
	    BindItem *mi = new BindItem(fun,px);
	    bindhead->Add(mi,sym);

        px->init(this,mi,data);

#if FLEXT_SYS == FLEXT_SYS_PD
    	pd_bind(&px->obj.ob_pd,const_cast<t_symbol *>(sym)); 
#elif FLEXT_SYS == FLEXT_SYS_MAX
        if(!sym->s_thing) 
            const_cast<t_symbol *>(sym)->s_thing = (t_object *)px;
        else
            error("%s - Symbol is already bound",thisName());
#else
#       pragma warning("Not implemented")
#endif
    }
    else 
        error("%s - Symbol proxy could not be created",thisName());

    return true;
}


bool flext_base::UnbindMethod(const t_symbol *sym,bool (*fun)(flext_base *,t_symbol *s,int argc,t_atom *argv,void *data),void **data)
{
    bool ok = false;
    
    if(bindhead && bindhead->Contained(0)) {
        ItemSet &set = bindhead->GetInlet();
        ItemSet::iterator it1,it2;
        if(sym) { 
            // specific tag
            it1 = it2 = set.find(sym); it2++; 
        }
        else { 
            // any tag
            it1 = set.begin(),it2 = set.end(); 
        }

        BindItem *it = NULL;
        for(ItemSet::iterator si = it1; si != it2 && !it; ++si) {
            for(Item *i = si.data(); i; i = i->nxt) {
                BindItem *item = (BindItem *)i;
                if(!fun || item->fun == fun) { it = item; break; }
            }
        }

        if(it) {
            if(data) *data = it->px->data;
            ok = bindhead->Remove(it,sym);
            if(ok) {
                it->Unbind(sym);
                delete it;
            }
        }
    }
    return ok;
}

bool flext_base::GetBoundMethod(const t_symbol *sym,bool (*fun)(flext_base *,t_symbol *s,int argc,t_atom *argv,void *data),void *&data)
{
    if(bindhead) {
        // Search for symbol
        for(Item *it = bindhead->FindList(sym); it; it = it->nxt) {
            BindItem *item = (BindItem *)it;

            // go through all items with matching tag
            if(item->fun == fun) {
                data = item->px->data;
                return true;
            }
        }
    }
    return false;
}

bool flext_base::UnbindAll()
{
    if(bindhead && bindhead->Contained(0)) {
        ItemSet &set = bindhead->GetInlet();
        for(ItemSet::iterator si = set.begin(); si != set.end(); ++si) {
            Item *lst = si.data();
            while(lst) {
				Item *nxt = lst->nxt;
                BindItem *it = (BindItem *)lst;
                it->Unbind(si.key());
                delete it;
				lst = nxt;
            }
        }
        set.clear();
    }
	return true;
}

void flext_base::pxbnd_object::px_method(pxbnd_object *c,const t_symbol *s,int argc,t_atom *argv)
{
    c->item->fun(c->base,(t_symbol *)s,argc,(t_atom *)argv,c->data);
}
