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

flext_base::binditem::binditem(int in,const t_symbol *sym,bool (*f)(flext_base *,t_symbol *s,int,t_atom *,void *data),pxbnd_object *p):
	item(sym,0,NULL),fun(f),px(p)
{}


bool flext_base::BindMethod(const t_symbol *sym,bool (*fun)(flext_base *,t_symbol *s,int argc,t_atom *argv,void *data),void *data)
{
    if(!bindhead) 
        bindhead = new itemarr;
    else {
        // Search for symbol
        if(bindhead->Find(sym,0)) {
            post("%s - Symbol already bound",thisName());
            return false;
        }
    }

    FLEXT_ASSERT(pxbnd_class); 

    pxbnd_object *px = (pxbnd_object *)object_new(pxbnd_class);

    if(px) {
	    binditem *mi = new binditem(0,sym,fun,px);
	    bindhead->Add(mi);

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


bool flext_base::UnbindMethod(const t_symbol *sym)
{
    item *it = bindhead?bindhead->Find(sym,0):NULL;
    if(it) {
        bool ok = bindhead->Remove(it);
        if(ok) {
            pxbnd_object *px = ((binditem *)it)->px;
#if FLEXT_SYS == FLEXT_SYS_PD
    	    pd_unbind(&px->obj.ob_pd,const_cast<t_symbol *>(sym)); 
#elif FLEXT_SYS == FLEXT_SYS_MAX
    	    if(sym->s_thing == (t_object *)px) 
                const_cast<t_symbol *>(sym)->s_thing = NULL; 
            else
                error("%s - Binding not found",thisName());
#else
#           pragma warning("Not implemented")
#endif
            object_free(&px->obj);
            delete it;
        }
        return ok;
    }
    else
        return true;
}

void flext_base::pxbnd_object::px_method(pxbnd_object *c,const t_symbol *s,int argc,t_atom *argv)
{
    c->item->fun(c->base,(t_symbol *)s,argc,(t_atom *)argv,c->data);
}
