/* 
flext tutorial - advanced 2

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This is an optimized version of the example "simple 3"

It has the exact same functionality but methods are registered at class setup opposed to
object setup (in the constructor) in "simple 3"

The advantage of this lies in the fact that the message database needs only be constructed
once for all objects, namely on creation of the first object of this class.
All objects [adv2] will share the same database, saving memory.

*/

// include flext header
#include <flext.h>

// check for appropriate flext version
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 401)
#error You need at least flext version 0.4.1
#endif

class adv2:
	public flext_base
{
	// flext header with a setup function called "setup"
	FLEXT_HEADER_S(adv2,flext_base,setup)
 
public:
	adv2();

protected:
	void m_tag();   
	void m_tag_and_int(int i);   
	void m_sym(t_symbol *s);   

	virtual void m_help();
	
private:
	// define the _static_ class setup function
	static void setup(t_class *c);

	FLEXT_CALLBACK(m_tag);  
	FLEXT_CALLBACK_I(m_tag_and_int);  
	FLEXT_CALLBACK_S(m_sym);  
};

// instantiate the class (constructor has a variable argument list)
FLEXT_NEW("adv2",adv2)


adv2::adv2()
{ 
	// define inlets
	AddInAnything();  // add inlet of type anything (index 0)
} 

void adv2::setup(t_class *c)
{
	// register methods:
	// notice the naming FLEXT_CADD_METHOD* instead of FLEXT_ADD_METHOD* 
	// there is also an additional parameter c pointing to the class definition
	FLEXT_CADDMETHOD_(c,0,"born",m_tag);  
	FLEXT_CADDMETHOD_(c,0,"to",m_tag);  
	FLEXT_CADDMETHOD_(c,0,"hula",m_tag);  
	FLEXT_CADDMETHOD_I(c,0,"hula",m_tag_and_int);  

	FLEXT_CADDMETHOD(c,0,m_sym);  
}

void adv2::m_tag()
{
	post("tag recognized");
}

void adv2::m_tag_and_int(int i)
{
	post("tag recognized (has int arg: %i)",i);
}

void adv2::m_sym(t_symbol *s)
{
	post("symbol: %s",GetString(s));
}

void adv2::m_help()
{
	post("%s - example for tagged messages",thisName());
}
	

