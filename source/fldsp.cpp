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
#else
#error Platform not supported!
#endif
}

flext_dsp::FLEXT_CLASSDEF(flext_dsp)()
    : srate(sys_getsr()),blksz(sys_getblksize())
    , vecs(NULL)
#if FLEXT_SYS != FLEXT_SYS_MAX
    , dspon(true)
#endif
{}

#if FLEXT_SYS == FLEXT_SYS_MAX
bool flext_dsp::Init()
{
    if(!flext_base::Init())
		return false;

	// according to the Max/MSP SDK this should be prior to any inlet creation, BUT
	// that doesn't seem to be true... multiple signal ins and additional inlets don't seem to work then      
	dsp_setup(thisHdr(),CntInSig()); // signal inlets   
	return true;
}
#endif    

void flext_dsp::Exit()
{
#if FLEXT_SYS == FLEXT_SYS_MAX
    // according to David Z. one should do that first...
	dsp_free(thisHdr());
#endif

    flext_base::Exit();
    
    if(vecs) delete[] vecs;
}


t_int *flext_dsp::dspmeth(t_int *w) 
{ 
    flext_dsp *obj = (flext_dsp *)(size_t)w[1];

#if FLEXT_SYS == FLEXT_SYS_MAX
    if(!obj->thisHdr()->z_disabled)
#else
    if(obj->dspon)
#endif
    {
        flext_base::indsp = true;
        obj->CbSignal(); 
        flext_base::indsp = false;
    }
    return w+2;
}

#if FLEXT_SYS == FLEXT_SYS_MAX
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
#endif

    // store current dsp parameters
    obj->srate = sp[0]->s_sr;
    obj->blksz = sp[0]->s_n;  // is this guaranteed to be the same as sys_getblksize() ?

    // store in and out signal vectors

    if((in+out) && !obj->vecs)
        obj->vecs = new t_signalvec[in+out];

    for(i = 0; i < in; ++i) 
        obj->vecs[i] = sp[i]->s_vec;
    for(i = 0; i < out; ++i) 
        obj->vecs[in+i] = sp[in+i]->s_vec;

    // with the following call derived classes can do their eventual DSP setup
    if(obj->CbDsp()) {
        // set the DSP function
        dsp_add((t_dspmethod)dspmeth,1,obj);  
    }
}


void flext_dsp::m_dsp(int /*n*/,t_signalvec const * /*insigs*/,t_signalvec const * /*outsigs*/) {}

bool flext_dsp::CbDsp() 
{ 
	// invoke legacy method
    m_dsp(Blocksize(),InSig(),OutSig()); 
    return true;
}

// this function will be overridden anyway - the probably useless default is clearing all outputs
void flext_dsp::m_signal(int n,t_sample *const * /*insigs*/,t_sample *const *outs) 
{
    for(int i = 0; i < CntOutSig(); ++i) ZeroSamples(outs[i],n);
}

void flext_dsp::CbSignal() 
{ 
	// invoke legacy method
	m_signal(Blocksize(),InSig(),OutSig()); 
}


#if FLEXT_SYS == FLEXT_SYS_PD
void flext_dsp::cb_enable(t_class *c,t_float on) { thisObject(c)->dspon = on != 0; }
#endif
