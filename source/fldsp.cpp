/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>
#include <fldefs.h>


// === flext_dsp ==============================================

void flext_dsp::cb_setup(t_class *c)
{
#ifdef PD
	CLASS_MAINSIGNALIN(c,flext_hdr,defsig);
#elif defined(MAXMSP)
	dsp_initclass();
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
	srate(sys_getsr()),
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

void flext_dsp::m_dsp(int /*n*/,float *const * /*insigs*/,float *const * /*outsigs*/) {}

void flext_dsp::cb_dsp(t_class *c,t_signal **sp) 
{ 
	flext_dsp *obj = thisObject(c); 

	// store current sample rate
	obj->srate = sp[0]->s_sr;

	// store in and out signal vectors
	int i,in = obj->CntInSig(),out = obj->CntOutSig();
	if(obj->invecs) delete[] obj->invecs;
	obj->invecs = new float *[in];
	for(i = 0; i < in; ++i) obj->invecs[i] = sp[i]->s_vec;

	if(obj->outvecs) delete[] obj->outvecs;
	obj->outvecs = new float *[out];
	for(i = 0; i < out; ++i) obj->outvecs[i] = sp[in+i]->s_vec;

	// with the following call derived classes can do their eventual DSP setup
	obj->m_dsp(sp[0]->s_n,obj->invecs,obj->outvecs);

	// set the DSP function
	dsp_add((t_dspmethod)dspmeth,2,obj,sp[0]->s_n);  
}

#ifndef MAXMSP
void flext_dsp::cb_enable(t_class *c,flint on) { thisObject(c)->m_enable(on != 0); }
void flext_dsp::m_enable(bool en) { dspon = en; }
#endif


