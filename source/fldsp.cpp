/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
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
//	dsp_initclass();
	dsp_initboxclass();
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

flext_dsp::flext_dsp(): 
#if FLEXT_SYS == FLEXT_SYS_JMAX
	srate(fts_dsp_get_sample_rate()),  // should we set it?
	blksz(fts_dsp_get_tick_size()),
#else
	srate(sys_getsr()),  // should we set it?
	blksz(sys_getblksize()),
#endif
#if FLEXT_SYS == FLEXT_SYS_PD
	chnsin(sys_get_inchannels()),
	chnsout(sys_get_outchannels()),
#elif FLEXT_SYS == FLEXT_SYS_MAX
	chnsin(sys_getch()),
	chnsout(sys_getch()),
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	#pragma message("not implemented")
	chnsin(0),chnsout(0),
#else
#error
#endif
#if FLEXT_SYS != FLEXT_SYS_MAX
	dspon(true),
#endif
	invecs(NULL),outvecs(NULL)
{
#if FLEXT_SYS == FLEXT_SYS_JMAX
	fts_dsp_object_init(thisHdr());
#endif
}


flext_dsp::~flext_dsp()
{
#if FLEXT_SYS == FLEXT_SYS_JMAX
	fts_dsp_object_delete(thisHdr());
#endif
	if(invecs) delete[] invecs;
	if(outvecs) delete[] outvecs;
}

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::dspmeth(fts_word_t *w) 
{ 
}
#else
t_int *flext_dsp::dspmeth(t_int *w) 
{ 
	flext_dsp *obj = (flext_dsp *)w[1];
#ifdef FLEXT_DEBUG
	if(!obj->thisHdr()) {
		// object is already deleted!
		ERRINTERNAL();
		return w+3;
	}
#endif
#if FLEXT_SYS == FLEXT_SYS_MAX
	if(!obj->thisHdr()->z_disabled) 
#else
	if(obj->dspon) 
#endif
		obj->m_signal((int)w[2],obj->invecs,obj->outvecs); 
	return w+3;
}
#endif

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::cb_dsp(fts_object_t *c, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
#else
void flext_dsp::cb_dsp(t_class *c,t_signal **sp) 
#endif
{ 
	flext_dsp *obj = thisObject(c); 

	if(obj->CntInSig()+obj->CntOutSig() == 0) return;

	// store current dsp parameters
#if FLEXT_SYS == FLEXT_SYS_JMAX
	fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at+0);
	obj->srate = fts_dsp_get_input_srate(dsp,0);
	obj->blksz = fts_dsp_get_input_size(dsp,0);  // is this guaranteed to be the same as sys_getblksize() ?
#else
	obj->srate = sp[0]->s_sr;
	obj->blksz = sp[0]->s_n;  // is this guaranteed to be the same as sys_getblksize() ?
#endif
	
#if FLEXT_SYS == FLEXT_SYS_PD
	obj->chnsin = sys_get_inchannels();
	obj->chnsout = sys_get_outchannels();
#elif FLEXT_SYS == FLEXT_SYS_MAX
	obj->chnsin = obj->chnsout = sys_getch();
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	#pragma message ("How to query the channels?")
#else
#error
#endif

	// store in and out signal vectors
	int i,in = obj->CntInSig(),out = obj->CntOutSig();

#if FLEXT_SYS == FLEXT_SYS_PD
	// min. 1 input channel! (CLASS_MAININLET in pd...)
	if(!in) in = 1;
#endif

	if(obj->invecs) delete[] obj->invecs;
	obj->invecs = new t_signalvec[in];
	for(i = 0; i < in; ++i) 
		obj->invecs[i] = 
#if FLEXT_SYS == FLEXT_SYS_JMAX
		fts_dsp_get_input_name(dsp,i);
#else
		sp[i]->s_vec;
#endif

	if(obj->outvecs) delete[] obj->outvecs;
	obj->outvecs = new t_signalvec[out];
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
	fts_atom_t args[2];
	fts_set_pointer(args+0,obj);
	fts_set_int(args+1,obj->blksz);
	fts_dsp_add_function(dspsym,2,args);
#else
	dsp_add((t_dspmethod)dspmeth,2,obj,obj->blksz);  
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

#if FLEXT_SYS != FLEXT_SYS_MAX

#if FLEXT_SYS == FLEXT_SYS_PD
void flext_dsp::cb_enable(t_class *c,t_float on) { thisObject(c)->m_enable(on != 0); }
#elif FLEXT_SYS == FLEXT_SYS_JMAX
void flext_dsp::cb_enable(fts_object_t *c, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at) 
{ thisObject(c)->m_enable(fts_get_int(at+0) != 0); }
#endif

void flext_dsp::m_enable(bool en) { dspon = en; }
#endif

