/* 
flext tutorial - simple 2

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This is an example of a simple object doing a float addition
*/

// include flext header
#include <flext.h>

// check for appropriate flext version
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 400)
#error You need at least flext version 0.4.0
#endif


class attr1:
	public flext_base
{
	FLEXT_HEADER(attr1,flext_base)
 
public:
	// constructor with float argument
	attr1(float init);

protected:
	void m_trigger(float f);   
	
	// stored argument of right inlet
	float arg; 

private:
	FLEXT_CALLBACK_F(m_trigger);  // callback for method "m_trigger" (with one float argument)

	FLEXT_ATTRVAR_F(arg);  // define attribute callbacks for variable "arg" 
};

// instantiate the class (constructor has one float argument)
FLEXT_NEW_1("attr1 op",attr1,float)


attr1::attr1(float init):
	arg(init)  // store argument
{ 
	// define inlets
	AddInAnything();  // first inlet of type anything (index 0)
	AddInFloat();     // additional float inlet (index 1)
	
	// define outlets
	AddOutFloat();  // one float outlet (has index 0)
	
	// register methods
	FLEXT_ADDMETHOD(0,m_trigger);  // register method (for floats) "m_trigger" for inlet 0

	FLEXT_ADDATTR_VAR1("arg",arg);  // register attribute "arg"
} 

void attr1::m_trigger(float f)
{
	float res;
	res = arg+f;
	
	// output value to outlet
	ToOutFloat(0,res); // (0 stands for the outlet index 0)
}

