/* 
flext tutorial - stk 2

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This is an example of an external using the STK ("synthesis toolkit") library.
For STK see http://ccrma-www.stanford.edu/software/stk

STK needs C++ exceptions switched on.

The STK tutorial examples assume that a stk.lib static library exists which contains all the
source files of the stk/src directory (except the rt*.cpp files).
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
	FLEXT_HEADER(stk2,flext_stk)

public:
	stk2();

protected:
	// these are obligatory!
	virtual bool NewObjs(); // create STK instruments
	virtual void FreeObjs(); // destroy STK instruments
	virtual void ProcessObjs(int n);  // do DSP processing

private:
	PitShift *inst[2];
	static void Setup(t_class *c); 
};

FLEXT_NEW_DSP("stk2~",stk2)
 

stk2::stk2()
{ 
	AddInSignal();
	AddOutSignal(2);  

	inst[0] = inst[1] = NULL;
}


// create STK instruments
bool stk2::NewObjs()
{
	bool ok = true;

	// set up objects
	try {
		inst[0] = new PitShift;
		inst[1] = new PitShift;
	}
	catch (StkError &) {
		post("%s - Noise() setup failed!",thisName());
		ok = false;
	}
	return ok;
}


// destroy the STK instruments
void stk2::FreeObjs()
{
	if(inst[0]) delete inst[0];
	if(inst[1]) delete inst[1];
}

// this is called on every DSP block
void stk2::ProcessObjs(int n)
{
	while(n--) {
		MY_FLOAT f = Inlet(0).tick();
		for(int i = 0; i < 2; ++i)
			Outlet(i).tick(inst[i]->tick(f));
	}
}


