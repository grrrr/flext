/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
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

void flext_dsp::Setup(t_class *c)
{
#ifdef PD
//	CLASS_MAINSIGNALIN(c,flext_hdr,defsig);
#elif defined(MAXMSP)
//	dsp_initclass();
	dsp_initboxclass();
#endif
	
	add_dsp(c,cb_dsp);
#ifndef MAXMSP
	add_method1(c,cb_enable,"enable",A_FLINT);
#endif
}

flext_dsp::flext_dsp(): 
#ifndef MAXMSP
	dspon(true),
#endif
	srate(sys_getsr()),  // should we set it?
	blksz(sys_getblksize()),
#ifdef PD
	chnsin(sys_get_inchannels()),
	chnsout(sys_get_outchannels()),
#else // MAXMSP
	chnsin(sys_getch()),
	chnsout(sys_getch()),
#endif
	invecs(NULL),outvecs(NULL)
{}


flext_dsp::~flext_dsp()
{
	if(invecs) delete[] invecs;
	if(outvecs) delete[] outvecs;
}


t_int *flext_dsp::dspmeth(t_int *w) 
{ 
	flext_dsp *obj = (flext_dsp *)w[1];
#ifdef MAXMSP
	if(!obj->thisHdr()->z_disabled) 
#else
	if(obj->dspon) 
#endif
		obj->m_signal((int)w[2],obj->invecs,obj->outvecs); 
	return w+3;
}

void flext_dsp::m_dsp(int /*n*/,t_sample *const * /*insigs*/,t_sample *const * /*outsigs*/) {}

void flext_dsp::m_signal(int n,t_sample *const * /*insigs*/,t_sample *const *outsigs) 
{
	for(int i = 0; i < CntOutSig(); ++i)
		memset(outsigs[i],0,n*sizeof(*outsigs[i]));
}

void flext_dsp::cb_dsp(t_class *c,t_signal **sp) 
{ 
	flext_dsp *obj = thisObject(c); 

	// store current dsp parameters
	obj->srate = sp[0]->s_sr;
	obj->blksz = sp[0]->s_n;  // is this guaranteed to be the same as sys_getblksize() ?
	
#ifdef PD
	obj->chnsin = sys_get_inchannels();
	obj->chnsout = sys_get_outchannels();
#else // MAXMSP
	obj->chnsin = obj->chnsout = sys_getch();
#endif

	// store in and out signal vectors
	int i,in = obj->CntInSig(),out = obj->CntOutSig();
	if(obj->invecs) delete[] obj->invecs;
	obj->invecs = new t_sample *[in];
	for(i = 0; i < in; ++i) obj->invecs[i] = sp[i]->s_vec;

	if(obj->outvecs) delete[] obj->outvecs;
	obj->outvecs = new t_sample *[out];
	for(i = 0; i < out; ++i) obj->outvecs[i] = sp[in+i]->s_vec;

	// with the following call derived classes can do their eventual DSP setup
	obj->m_dsp(sp[0]->s_n,obj->invecs,obj->outvecs);

	// set the DSP function
	dsp_add((t_dspmethod)dspmeth,2,obj,sp[0]->s_n);  
}

#ifndef MAXMSP
void flext_dsp::cb_enable(t_class *c,t_flint on) { thisObject(c)->m_enable(on != 0); }
void flext_dsp::m_enable(bool en) { dspon = en; }
#endif


