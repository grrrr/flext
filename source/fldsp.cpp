/*
flext - C++ layer for Max and Pure Data externals

Copyright (c) 2001-2015 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.
*/

/*! \file fldsp.cpp
    \brief Implementation of the flext dsp base class.
*/
 
#ifndef __FLEXT_DSP_CPP
#define __FLEXT_DSP_CPP

#include "flext.h"
#include "flinternal.h"
#include <cstring>

#include "flpushns.h"

// === flext_dsp ==============================================

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::Setup(t_classid id)
{
#if FLEXT_SYS == FLEXT_SYS_PD
//    add_method1(c,cb_enable,"enable",A_FLOAT);
    AddMethod(id,0,MakeSymbol("enable"),&cb_enable);
#endif
}

FLEXT_TEMPIMPL(FLEXT_CLASSDEF(flext_dsp))::FLEXT_CLASSDEF(flext_dsp)()
    : srate(sys_getsr()),blksz(sys_getblksize())
#if MSP64
    ,inVec(NULL),outVec(NULL)
#else
    , vecs(NULL)
#endif
#if FLEXT_SYS != FLEXT_SYS_MAX
    , dspon(true)
#endif
{}

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::Exit()
{
    flext_base::Exit();
#if !MSP64
    if(vecs) delete[] vecs;
#endif
}


#if MSP64
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::dspmeth64(flext_hdr *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    flext_dsp *obj = (flext_dsp *)userparam;
    
    obj->inVec = ins;
    obj->outVec = outs;
    

    if(!obj->thisHdr()->z_disabled)

        {
            flext_base::indsp = true;
            obj->CbSignal64();
            flext_base::indsp = false;
        }
    
}
#else

FLEXT_TEMPIMPL(t_int *FLEXT_CLASSDEF(flext_dsp))::dspmeth(t_int *w)
{ 
    flext_dsp *obj = (flext_dsp *)(size_t)w[1];

#if FLEXT_SYS == FLEXT_SYS_MAX
    if(!obj->thisHdr()->z_disabled)
#else
    if(LIKELY(obj->dspon))
#endif
    {
        flext_base::indsp = true;
        obj->CbSignal(); 
        flext_base::indsp = false;
    }
    return w+2;
}
#endif





#if MSP64
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::SetupDsp64(flext_hdr *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    
    // store current dsp parameters
    srate = sys_getsr();   // \TODO need not be stored in each object....
    // overlap = sp[0]->s_sr/srate;  // currently not used/exposed
    blksz = sys_getblksize() ;

    // with the following call derived classes can do their eventual DSP setup
    if(CbDsp64()) {
        // set the DSP function
//        object_method(dsp64, gensym("dsp_add64"), x, dspmeth64, 0, NULL);
        dsp_add64(dsp64,(t_object *)&x->obj,(t_dspmethod)dspmeth64,flags, this);
    }
}
#else

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::SetupDsp(t_signal **sp)
{ 
    int i;
    int in = CntInSig();
    int out = CntOutSig();
#if FLEXT_SYS == FLEXT_SYS_PD
    // min. 1 input channel! (CLASS_MAININLET in pd...)
    if(!in) in = 1;
#endif

    // store current dsp parameters
    srate = sys_getsr();   // \TODO need not be stored in each object....
    // overlap = sp[0]->s_sr/srate;  // currently not used/exposed
    
    blksz = sp[0]->s_n;  // is this guaranteed to be the same as sys_getblksize() ?

    // store in and out signal vectors

    if((in+out) && !vecs)
        vecs = new t_signalvec[in+out];

    for(i = 0; i < in; ++i) 
        vecs[i] = sp[i]->s_vec;
    for(i = 0; i < out; ++i) 
        vecs[in+i] = sp[in+i]->s_vec;

    // with the following call derived classes can do their eventual DSP setup
    if(CbDsp()) {
        // set the DSP function
        dsp_add((t_dspmethod)dspmeth,1,this);
        
    }
}
#endif


FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::m_dsp(int /*n*/,t_signalvec const * /*insigs*/,t_signalvec const * /*outsigs*/) {}

FLEXT_TEMPIMPL(bool FLEXT_CLASSDEF(flext_dsp))::CbDsp()
{ 
	// invoke legacy method
    m_dsp(Blocksize(),InSig(),OutSig()); 
    return true;
}


FLEXT_TEMPIMPL(bool FLEXT_CLASSDEF(flext_dsp))::CbDsp64()
{
	// invoke legacy method
    m_dsp(Blocksize(),InSig(),OutSig());
    return true;
}


// this function will be overridden anyway - the probably useless default is clearing all outputs
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::m_signal(int n,t_sample *const * /*insigs*/,t_sample *const *outs)
{
    for(int i = 0; i < CntOutSig(); ++i) ZeroSamples(outs[i],n);
}

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::CbSignal()
{ 
	// invoke legacy method
	m_signal(Blocksize(),InSig(),OutSig()); 
}
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_dsp))::CbSignal64()
{
	// invoke legacy method
	m_signal(Blocksize(),InSig(),OutSig());
}

#if FLEXT_SYS == FLEXT_SYS_PD
//void flext_dsp::cb_enable(flext_hdr *c,t_float on) { thisObject(c)->dspon = on != 0; }
FLEXT_TEMPIMPL(bool FLEXT_CLASSDEF(flext_dsp))::cb_enable(flext_base *b,float &on)
{
    static_cast<flext_dsp *>(b)->dspon = on != 0;
    return true;
}
#endif

#include "flpopns.h"

#endif // __FLEXT_DSP_CPP


