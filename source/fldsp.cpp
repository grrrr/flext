/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fldsp.cpp
    \brief Implementation of the flext dsp base class.
*/
 
#include "flext.h"
#include "flinternal.h"
#include <string.h>

// === flext_dsp ==============================================

#if FLEXT_SYS == FLEXT_SYS_JMAX
const t_symbol *flext_dsp::dspsym = MakeSymbol("__flext_dspfun__");
#endif

void flext_dsp::Setup(t_classid id)
{
    t_class *c = getClass(id);

#if FLEXT_SYS == FLEXT_SYS_MAX
    dsp_initclass();
    add_dsp(c,cb_dsp);
#elif FLEXT_SYS == FLEXT_SYS_PD
    CLASS_MAINSIGNALIN(c,flext_hdr,defsig); // float messages going into the left inlet are converted to signal
    add_dsp(c,cb_dsp);
    add_method1(c,cb_enable,"enable",A_FLOAT);
#elif FLEXT_SYS == FLEXT_SYS_JMAX
    fts_dsp_declare_function(dspsym,dspmeth);   
    fts_class_message_varargs(c, fts_s_put, cb_dsp);
    fts_class_message_varargs(c, MakeSymbol("enable"), cb_enable);
#endif
}

flext_dsp::FLEXT_CLASSDEF(flext_dsp)(): 
#if FLEXT_SYS == FLEXT_SYS_JMAX
    srate(fts_dsp_get_sample_rate()),  // should we set it?
    blksz(fts_dsp_get_tick_size()),
#else
    srate(sys_getsr()),blksz(sys_getblksize()),
#endif
    chnsin(0),chnsout(0),
#if FLEXT_SYS != FLEXT_SYS_MAX
    dspon(true),
#endif
    invecs(NULL),outvecs(NULL)
{
#if FLEXT_SYS == FLEXT_SYS_JMAX
    fts_dsp_object_init(thisHdr());
#endif
}


flext_dsp::~FLEXT_CLASSDEF(flext_dsp)()
{
#if FLEXT_SYS == FLEXT_SYS_JMAX
    fts_dsp_object_delete(thisHdr());
#endif

    if(invecs) delete[] invecs;
    if(outvecs) delete[] outvecs;

/*
#if FLEXT_SYS == FLEXT_SYS_MAX
    // switch off dsp as the dsp function might get called afterwards (?!)
    thisHdr()->z_disabled = true;
    
    if(invecs) delete[] invecs;
    if(outvecs) delete[] outvecs;
#elif FLEXT_SYS == FLEXT_SYS_PD
    dspon = false;
#endif
*/
}

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::dspmeth(fts_word_t *w) 
{ 
}
#else
t_int *flext_dsp::dspmeth(t_int *w) 
{ 
    flext_dsp *obj = (flext_dsp *)(size_t)w[1];
/*
#ifdef FLEXT_DEBUG
    if(!obj->thisHdr()) {
        // object is already deleted!
        ERRINTERNAL();
        return w+3;
    }
#endif
*/
#if FLEXT_SYS == FLEXT_SYS_MAX
    if(!obj->thisHdr()->z_disabled)
#else
    if(obj->dspon)
#endif
    {
        obj->indsp = true;
        obj->m_signal(obj->blksz,obj->invecs,obj->outvecs); 
        obj->indsp = false;
    }
    return w+2;
}
#endif

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::cb_dsp(fts_object_t *c, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
#elif FLEXT_SYS == FLEXT_SYS_MAX
void flext_dsp::cb_dsp(t_class *c,t_signal **sp,short *count) 
#else
void flext_dsp::cb_dsp(t_class *c,t_signal **sp) 
#endif
{ 
    flext_dsp *obj = thisObject(c); 

    int i;
    int in = obj->CntInSig();
    int out = obj->CntOutSig();
#if FLEXT_SYS == FLEXT_SYS_PD
    // min. 1 input channel! (CLASS_MAININLET in pd...)
    if(!in) in = 1;
#else
    if(in+out == 0) return;
#endif

    // store current dsp parameters
#if FLEXT_SYS == FLEXT_SYS_JMAX
    fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at+0);
    obj->srate = fts_dsp_get_input_srate(dsp,0);
    obj->blksz = fts_dsp_get_input_size(dsp,0);  // is this guaranteed to be the same as sys_getblksize() ?
#else
    obj->srate = sp[0]->s_sr;
    obj->blksz = sp[0]->s_n;  // is this guaranteed to be the same as sys_getblksize() ?
#endif

    // store in and out signal vectors

    if(in != obj->chnsin) {
        if(obj->invecs) delete[] obj->invecs;
        obj->invecs = in?new t_signalvec[in]:NULL;
        obj->chnsin = in;
    }
    for(i = 0; i < in; ++i) 
        obj->invecs[i] = 
#if FLEXT_SYS == FLEXT_SYS_JMAX
            fts_dsp_get_input_name(dsp,i);
#else
            sp[i]->s_vec;
#endif

    if(out != obj->chnsout) {
        if(obj->outvecs) delete[] obj->outvecs;
        obj->outvecs = out?new t_signalvec[out]:NULL;
        obj->chnsout = out;
    }
    for(i = 0; i < out; ++i) 
        obj->outvecs[i] = 
#if FLEXT_SYS == FLEXT_SYS_JMAX
            fts_dsp_get_output_name(dsp,i);
#else
            sp[in+i]->s_vec;
#endif

    // with the following call derived classes can do their eventual DSP setup
    obj->m_dsp(obj->blksz,obj->invecs,obj->outvecs);

    // set the DSP function
#if FLEXT_SYS == FLEXT_SYS_JMAX
    fts_atom_t args;
    fts_set_pointer(args,obj);
    fts_dsp_add_function(dspsym,1,args);
#else
    dsp_add((t_dspmethod)dspmeth,1,obj);  
#endif
}

/*
#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::cb_dsp_init(fts_object_t *c, int winlet, fts_symbol_t *s, int ac, const fts_atom_t *at)
{
    fts_dsp_add_object(c);
}

void flext_dsp::cb_dsp_delete(fts_object_t *c, int winlet, fts_symbol_t *s, int ac, const fts_atom_t *at)
{
    fts_dsp_remove_object(c);
}
#endif
*/

void flext_dsp::m_dsp(int /*n*/,t_signalvec const * /*insigs*/,t_signalvec const * /*outsigs*/) {}

void flext_dsp::m_signal(int n,t_sample *const * /*insigs*/,t_sample *const *outs) 
{
    for(int i = 0; i < CntOutSig(); ++i) ZeroSamples(outs[i],n);
}

#if FLEXT_SYS == FLEXT_SYS_PD
void flext_dsp::cb_enable(t_class *c,t_float on) { thisObject(c)->dspon = on != 0; }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::cb_enable(fts_object_t *c, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ thisObject(c)->dspon = fts_get_int(at+0) != 0; }
#endif
