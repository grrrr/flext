/* 
flext tutorial - stk 2

Copyright (c) 2002,2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This is an example of an external using the STK ("synthesis toolkit") library.
For STK see http://ccrma-www.stanford.edu/software/stk

STK needs C++ exceptions switched on.

The STK tutorial examples assume that a static stk library exists which contains all the
source files (except rt*.cpp) of the stk/src directory.
The library should be compiled multithreaded and with the appropriate compiler flags for 
the respective platform (e.g. __OS_WINDOWS__ and __LITTLE_ENDIAN__ for Windows)

*/

#include <flstk.h>

#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 401)
#error You need at least flext version 0.4.1
#endif

#include "PitShift.h"


class stk2:
	public flext_stk
{
	FLEXT_HEADER_S(stk2,flext_stk,Setup)

public:
	stk2();

	void m_sh1(float f) { if(inst[0]) inst[0]->setShift(f); }
	void m_sh2(float f) { if(inst[1]) inst[1]->setShift(f); }

	// these are obligatory!
	virtual bool NewObjs(); // create STK instruments
	virtual void FreeObjs(); // destroy STK instruments
	virtual void ProcessObjs(int n);  // do DSP processing

	PitShift *inst[2];
    MY_FLOAT *vec;

private:
	static void Setup(t_class *c);

	FLEXT_CALLBACK_F(m_sh1)
	FLEXT_CALLBACK_F(m_sh2)
};

FLEXT_NEW_DSP("stk2~",stk2)
 

stk2::stk2()
{ 
	AddInSignal();
	AddOutSignal(2);  

	inst[0] = inst[1] = NULL;
}

void stk2::Setup(t_class *c)
{
	FLEXT_CADDMETHOD_F(c,0,"shL",m_sh1);
	FLEXT_CADDMETHOD_F(c,0,"shR",m_sh2);
}


// create STK instruments
bool stk2::NewObjs()
{
	bool ok = true;

	try {
    	// set up objects
		for(int i = 0; i < 2; ++i)
			inst[i] = new PitShift;

        // reserve one signal vector too
        vec = new MY_FLOAT[Blocksize()];
	}
	catch (StkError &) {
		post("%s - Creation failed!",thisName());
		ok = false;
	}
	return ok;
}


// destroy the STK instruments
void stk2::FreeObjs()
{
	for(int i = 0; i < 2; ++i)
		if(inst[i]) delete inst[i];
    if(vec) delete[] vec;
}

// this is called on every DSP block
void stk2::ProcessObjs(int n)
{
	for(int i = 0; i < 2; ++i)
		Outlet(i).tick(
            inst[i]->tick(
                Inlet(0).tick(vec,n)
            ,n)
        ,n);
}


