/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flout.cpp
    \brief Implementation of the flext outlet functionality.
*/

#include "flext.h"
#include "flinternal.h"
#include <string.h>
 
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
void flext_base::ToSysAtom(int n,const t_atom &at) const 
{ 
    outlet *o = GetOut(n); 
    if(o) { 
        CRITON(); 
        if(IsSymbol(at))
            outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(GetSymbol(at))); 
        else if(IsFloat(at))
            outlet_float((t_outlet *)o,GetFloat(at)); 
#if FLEXT_SYS == FLEXT_SYS_MAX
        else if(IsInt(at))
            outlet_flint((t_outlet *)o,GetInt(at));
#endif
#if FLEXT_SYS == FLEXT_SYS_PD
        else if(IsPointer(at))
            outlet_pointer((t_outlet *)o,GetPointer(at)); 
#endif
        else
            error("Atom type not supported");
        CRITOFF(); 
    } 
}
#else
#error Not implemented
#endif

#if defined(FLEXT_THREADS)
    #if FLEXT_QMODE == 2
        #define CHKTHR() ((IsSystemThread() || IsThread(flext::thrmsgid)) && !InDsp())
    #else
        #define CHKTHR() (IsSystemThread() && !InDsp())
    #endif
#else
    #define CHKTHR() (!InDsp())
#endif

void flext_base::ToOutBang(int n) const { if(CHKTHR()) ToSysBang(n); else ToQueueBang(n); }
void flext_base::ToOutFloat(int n,float f) const { if(CHKTHR()) ToSysFloat(n,f); else ToQueueFloat(n,f); }
void flext_base::ToOutInt(int n,int f) const { if(CHKTHR()) ToSysInt(n,f); else ToQueueInt(n,f); }
void flext_base::ToOutSymbol(int n,const t_symbol *s) const { if(CHKTHR()) ToSysSymbol(n,s); else ToQueueSymbol(n,s); }
void flext_base::ToOutAtom(int n,const t_atom &at) const { if(CHKTHR()) ToSysAtom(n,at); else ToQueueAtom(n,at); }
void flext_base::ToOutList(int n,int argc,const t_atom *argv) const { if(CHKTHR()) ToSysList(n,argc,argv); else ToQueueList(n,argc,argv); }
void flext_base::ToOutAnything(int n,const t_symbol *s,int argc,const t_atom *argv) const { if(CHKTHR()) ToSysAnything(n,s,argc,argv); else ToQueueAnything(n,s,argc,argv); }


bool flext_base::InitInlets()
{
    bool ok = true;

    // ----------------------------------
    // create inlets
    // ----------------------------------

    incnt = insigs = 0; 

    // digest inlist

    xlet *xi;
    incnt = 0;
    for(xi = inlist; xi; xi = xi->nxt) ++incnt;
    xlet::type *list = new xlet::type[incnt];
    int i;
    for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;
    
#if FLEXT_SYS == FLEXT_SYS_MAX      
    // copy inlet descriptions
    indesc = new char *[incnt];
    for(xi = inlist,i = 0; xi; xi = xi->nxt,++i) {
        int l = xi->desc?strlen(xi->desc):0;
        if(l) {
            indesc[i] = new char[l+1];
            memcpy(indesc[i],xi->desc,l);
            indesc[i][l] = 0;
        }
        else
            indesc[i] = NULL;
    }
#endif

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
    inlets = new px_object *[incnt];
    for(i = 0; i < incnt; ++i) inlets[i] = NULL;
#endif
    
    // type info is now in list array
#if FLEXT_SYS == FLEXT_SYS_PD
    {
        int cnt = 0;
        xi = inlist; // points to first inlet

        if(incnt >= 1) {
            switch(list[0]) {
                case xlet::tp_sig:
                    ++insigs;
                    break;
                default:
                    // leftmost inlet is already there...
                    break;
            } 
            ++cnt;

#if PD_MINOR_VERSION >= 37 && defined(PD_DEVEL_VERSION)
            // set tooltip
            if(xi->desc && *xi->desc) class_settip(thisClass(),gensym(xi->desc));
#endif
        }       

        for(int ix = 1; ix < incnt; ++ix,++cnt) {
            xi = xi->nxt; // points to next inlet

            t_inlet *in = NULL;
            switch(list[ix]) {
                case xlet::tp_float:
                case xlet::tp_int: {
                    char sym[] = "ft??";
                    if(ix >= 10) { 
                        if(compatibility) {
                            // Max allows max. 9 inlets
                            post("%s: Only 9 float/int inlets allowed in compatibility mode",thisName());
                            ok = false;
                        }
                        else {
                            if(ix > 99)
                                post("%s: Inlet index > 99 not allowed for float/int inlets",thisName());
                            sym[2] = '0'+ix/10,sym[3] = '0'+ix%10;
                        }
                    }
                    else 
                        sym[2] = '0'+ix,sym[3] = 0;  
                    if(ok) in = inlet_new(&x_obj->obj, &x_obj->obj.ob_pd, (t_symbol *)sym_float, gensym(sym)); 
                    break;
                }
                case xlet::tp_sym: 
                    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
                    in = inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, (t_symbol *)sym_symbol, (t_symbol *)sym_symbol);  
                    break;
                case xlet::tp_list:
                    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
                    in = inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, (t_symbol *)sym_list, (t_symbol *)sym_list);  
                    break;
                case xlet::tp_any:
                    (inlets[ix] = (px_object *)pd_new(px_class))->init(this,ix);  // proxy for 2nd inlet messages 
                    in = inlet_new(&x_obj->obj,&inlets[ix]->obj.ob_pd, 0, 0);  
                    break;
                case xlet::tp_sig:
                    if(compatibility && list[ix-1] != xlet::tp_sig) {
                        post("%s: All signal inlets must be left-aligned in compatibility mode",thisName());
                        ok = false;
                    }
                    else {
                        // pd doesn't seem to be able to handle signals and messages into the same inlet...
                        
                        in = inlet_new(&x_obj->obj, &x_obj->obj.ob_pd, (t_symbol *)sym_signal, (t_symbol *)sym_signal);  
                        ++insigs;
                    }
                    break;
                default:
                    error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
                    ok = false;
            } 

#if PD_MINOR_VERSION >= 37 && defined(PD_DEVEL_VERSION)
            // set tooltip
            if(in && xi->desc && *xi->desc) inlet_settip(in,gensym(xi->desc));
#endif
        }

        incnt = cnt;
    }
#elif FLEXT_SYS == FLEXT_SYS_MAX
    {
        int ix,cnt;
        // count leftmost signal inlets
        while(insigs < incnt && list[insigs] == xlet::tp_sig) ++insigs;
        
        for(cnt = 0,ix = incnt-1; ix >= insigs; --ix,++cnt) {
            if(ix == 0) {
                if(list[ix] != xlet::tp_any) {
                    error("%s: Leftmost inlet must be of type signal or anything",thisName());
                    ok = false;
                }
            }
            else {
                switch(list[ix]) {
                    case xlet::tp_sig:
                        error("%s: All signal inlets must be left-aligned",thisName());
                        ok = false;
                        break;
                    case xlet::tp_float:
                        if(ix >= 10) { 
                            post("%s: Only 9 float inlets possible",thisName());
                            ok = false;
                        }
                        else
                            floatin(x_obj,ix);  
                        break;
                    case xlet::tp_int:
                        if(ix >= 10) { 
                            post("%s: Only 9 int inlets possible",thisName());
                            ok = false;
                        }
                        else
                            intin(x_obj,ix);  
                        break;
                    case xlet::tp_any: // non-leftmost
                    case xlet::tp_sym:
                    case xlet::tp_list:
                        inlets[ix] = (px_object *)proxy_new(x_obj,ix,&((flext_hdr *)x_obj)->curinlet);  
                        break;
                    default:
                        error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
                        ok = false;
                } 
            }
        }

//          incnt = cnt;

        if(insigs) 
              dsp_setup(thisHdr(),insigs); // signal inlets   
//            dsp_setupbox(thisHdr(),insigs); // signal inlets    
    }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
    {
        t_class *cl = thisClass();
        int cnt = 0;
        for(int ix = 0; ix < incnt; ++ix,++cnt) {
            switch(list[ix]) {
                case xlet::tp_float:
                case xlet::tp_int:
//                      fts_class_inlet_number(cl, ix, jmax_proxy);
                    break;
                case xlet::tp_sym: 
//                      fts_class_inlet_symbol(cl, ix, jmax_proxy);
                    break;
                case xlet::tp_sig:
                    if(compatibility && list[ix-1] != xlet::tp_sig) {
                        post("%s: All signal inlets must be left-aligned in compatibility mode",thisName());
                        ok = false;
                    }
                    else {
                        if(!insigs) fts_dsp_declare_inlet(cl,0);                            
                        ++insigs;
                    }
                    // no break -> let a signal inlet also accept any messages
                case xlet::tp_list:
                case xlet::tp_any:
//                      fts_class_inlet_varargs(cl,ix, jmax_proxy);
                    break;
                default:
                    error("%s: Wrong type for inlet #%i: %i",thisName(),ix,(int)list[ix]);
                    ok = false;
            } 
        }

        incnt = cnt;

        fts_object_set_inlets_number((fts_object_t *)thisHdr(), incnt);
    }
#else
#error
#endif

    delete inlist; inlist = NULL;
    
    delete[] list;

    return ok;  
}

bool flext_base::InitOutlets()
{
    bool ok = true;

    // ----------------------------------
    // create outlets
    // ----------------------------------

    outcnt = outsigs = 0; 
    
#if FLEXT_SYS == FLEXT_SYS_MAX
    // for Max/MSP the rightmost outlet has to be created first
    outlet *attrtmp = NULL;
    if(procattr) 
        attrtmp = (outlet *)newout_anything(thisHdr());
#endif

    // digest outlist
    {
        xlet *xi;

        // count outlets
        outcnt = 0;
        for(xi = outlist; xi; xi = xi->nxt) ++outcnt;

        xlet::type *list = new xlet::type[outcnt];
        int i;
        for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) list[i] = xi->tp;

#if FLEXT_SYS == FLEXT_SYS_MAX      
        // copy outlet descriptions
        outdesc = new char *[outcnt];
        for(xi = outlist,i = 0; xi; xi = xi->nxt,++i) {
            int l = xi->desc?strlen(xi->desc):0;
            if(l) {
                outdesc[i] = new char[l+1];
                memcpy(outdesc[i],xi->desc,l);
                outdesc[i][l] = 0;
            }
            else
                outdesc[i] = NULL;
        }
#endif

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
        outlets = new outlet *[outcnt+(procattr?1:0)];

        // type info is now in list array
#if FLEXT_SYS == FLEXT_SYS_PD
        for(int ix = 0; ix < outcnt; ++ix) 
#elif FLEXT_SYS == FLEXT_SYS_MAX
        for(int ix = outcnt-1; ix >= 0; --ix) 
#else
#error
#endif
        {
            switch(list[ix]) {
                case xlet::tp_float:
                    outlets[ix] = (outlet *)newout_float(&x_obj->obj);
                    break;
                case xlet::tp_int: 
                    outlets[ix] = (outlet *)newout_flint(&x_obj->obj);
                    break;
                case xlet::tp_sig:
                    outlets[ix] = (outlet *)newout_signal(&x_obj->obj);
                    ++outsigs;
                    break;
                case xlet::tp_sym:
                    outlets[ix] = (outlet *)newout_symbol(&x_obj->obj);
                    break;
                case xlet::tp_list:
                    outlets[ix] = (outlet *)newout_list(&x_obj->obj);
                    break;
                case xlet::tp_any:
                    outlets[ix] = (outlet *)newout_anything(&x_obj->obj);
                    break;
#ifdef FLEXT_DEBUG
                default:
                    ERRINTERNAL();
                    ok = false;
#endif
            } 
        }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
        t_class *cl = thisClass();
        for(int ix = 0; ix < outcnt; ++ix) {
            switch(list[ix]) {
                case xlet::tp_float:
                case xlet::tp_int: 
//                  fts_class_outlet_number(cl, ix);
                    break;
                case xlet::tp_sym:
//                  fts_class_outlet_symbol(cl, ix);
                    break;
                case xlet::tp_list:
                case xlet::tp_any:
//                  fts_class_outlet_anything(cl, ix);
                    break;
                case xlet::tp_sig:
                    if(!outsigs) fts_dsp_declare_outlet(cl,0);
                    ++outsigs;
                    break;
#ifdef FLEXT_DEBUG
                default:
                    ERRINTERNAL();
                    ok = false;
#endif
            } 
        }

        fts_object_set_outlets_number((fts_object_t *)thisHdr(), outcnt+(procattr?1:0));
#endif

        delete[] list;
    }

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
    if(procattr) {
        // attribute dump outlet is the last one
        outlets[outcnt] = 
#if FLEXT_SYS == FLEXT_SYS_PD
        // attribute dump outlet is the last one
            (outlet *)newout_anything(&x_obj->obj);
#elif FLEXT_SYS == FLEXT_SYS_MAX
            attrtmp;
#endif

    }
#endif
    
    delete outlist; outlist = NULL;
        
    return ok;
}


