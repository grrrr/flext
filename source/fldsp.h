/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fldsp.h
    \brief Declares the flext dsp class
    
    !Lacking Details.!
*/

#ifndef __FLDSP_H
#define __FLDSP_H

//! include the header file declaring the base classes
#include "flext.h"


// === flext_dsp ==================================================

/*! \class flext_dsp
	\brief dsp enabled base object
*/

class flext_dsp:
	public flext_base
{
	FLEXT_HEADER_S(flext_dsp,flext_base,Setup)
	
public:

	//! returns current sample rate
	float Samplerate() const { return srate; }
	
	//! returns current block size
	int Blocksize() const { return blksz; }
	
	//! returns number of audio system input channels
	int ChannelsIn() const { return chnsin; }
	//! returns number of audio system output channels
	int ChannelsOut() const { return chnsout; }
	

// --- inheritable virtual methods --------------------------------

	/*! \brief Called on every dsp init.
		\remark Don't expect any valid data in the signal vectors!
		@param n: frames (aka samples) in one signal vector
		@param insigs: array of input vectors  (get number with function CntInSig())
		@param outsigs: array of output vectors  (get number with function CntOutSig())
	*/
	virtual void m_dsp(int n,t_sample *const *insigs,t_sample *const *outsigs);

	/*! \brief Called with every signal vector.
		Here you do the dsp calculation
		@param n: frames (aka samples) in one signal vector
		@param insigs: array of input vectors  (get number with function CntInSig())
		@param outsigs: array of output vectors  (get number with function CntOutSig())
	*/
	virtual void m_signal(int n,t_sample *const *insigs,t_sample *const *outsigs) = 0;

#ifndef MAXMSP
	//! called with "enable" message: pauses/resumes dsp - implicitely defined in MaxMSP
	virtual void m_enable(bool on);
#endif

// --- inlet/outlet stuff -----------------------------------------	

	//! add signal inlet
	void AddInSignal(int m = 1) { AddInlet(xlet::tp_sig,m); }

	//! add signal outlet
	void AddOutSignal(int m = 1) { AddOutlet(xlet::tp_sig,m); }


protected:
	
	flext_dsp();
	~flext_dsp();
	
private:

	// not static, could be different in different patchers..
	float srate; 
	int blksz;
	int chnsin,chnsout;

	// setup function
	static void Setup(t_class *c);

	// callback functions

	static void cb_dsp(t_class *c,t_signal **s);
#ifndef MAXMSP
	static void cb_enable(t_class *c,t_flint on);
	bool dspon;
#endif

	// dsp stuff

	static t_int *dspmeth(t_int *w); 
	t_sample **invecs,**outvecs;
};

#endif
