/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fldsp.h
    \brief Declares the flext dsp class
    
*/

#ifndef __FLDSP_H
#define __FLDSP_H

// include the header file declaring the base classes
#include "flext.h"


// === flext_dsp ==================================================

/*! \brief Flext dsp enabled base object
*/
class flext_dsp:
	public flext_base
{
	FLEXT_HEADER_S(flext_dsp,flext_base,Setup)
	
public:

/*!	\defgroup FLEXT_DSP Flext dsp class

	@{ 
*/

/*!	\defgroup FLEXT_C_DSP Basic dsp functionality

	@{ 
*/

	//! returns current sample rate
	float Samplerate() const { return srate; }
	
	//! returns current block (aka vector) size
	int Blocksize() const { return blksz; }
	
	/*! \brief returns number of audio system input channels
		\bug Doesn't work in Max/MSP - is always 0
	*/
	int ChannelsIn() const { return chnsin; }

	/*! \brief returns number of audio system output channels
		\bug Doesn't work in Max/MSP - is always 0
	*/
	int ChannelsOut() const { return chnsout; }
	
//!	@} 


// --- inheritable virtual methods --------------------------------

/*!	\defgroup FLEXT_C_DSP_VIRTUAL Flext virtual dsp functions

	@{ 
*/
	/*! \brief Called on every dsp init.
		\note Don't expect any valid data in the signal vectors!

		\param n: frames (aka samples) in one signal vector
		\param insigs: array of input vectors  (get number with function CntInSig())
		\param outsigs: array of output vectors  (get number with function CntOutSig())
	*/
	virtual void m_dsp(int n,t_sample *const *insigs,t_sample *const *outsigs);

	/*! \brief Called with every signal vector - here you do the dsp calculation

		\param n: frames (aka samples) in one signal vector
		\param insigs: array of input vectors  (get number with function CntInSig())
		\param outsigs: array of output vectors  (get number with function CntOutSig())
	*/
	virtual void m_signal(int n,t_sample *const *insigs,t_sample *const *outsigs);

#ifndef MAXMSP
	/*! \brief called with "enable" message: pauses/resumes dsp
		\note PD only - implicitely defined in MaxMSP
	*/
	virtual void m_enable(bool on);
#endif

//!	@} 


/*!	\defgroup FLEXT_C_DSP_INOUT Flext dsp in-/outlet functions
	\note These must be called in the class' constructor

	@{ 
*/
// --- inlet/outlet stuff -----------------------------------------	

	/*! \brief Add signal inlet(s)
		\param m Number of inlets to add
	*/
	void AddInSignal(int m = 1) { AddInlet(xlet::tp_sig,m); }

	/*! \brief Add signal outlet(s)
		\param m Number of inlets to add
	*/
	void AddOutSignal(int m = 1) { AddOutlet(xlet::tp_sig,m); }

//!	@} 

//!	@} 

protected:
	
	flext_dsp();
	virtual ~flext_dsp();

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
