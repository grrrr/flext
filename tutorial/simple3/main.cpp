/* 
flext tutorial - simple 3

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

*/

// include flext header
#include <flext.h>

// check for appropriate flext version
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 300)
#error You need at least flext version 0.3.0
#endif


class simple3:
	public flext_base
{
	FLEXT_HEADER(simple3,flext_base)
 
public:
	// constructor with variable argument list
	simple3(int argc,t_atom *argv);

protected:
	void m_tag();   
	void m_sym(t_symbol *s);   
	
private:

	FLEXT_CALLBACK(m_tag);  // callback for method "m_tag" (no arguments)
	FLEXT_CALLBACK_S(m_sym);  // callback for method "m_sym" (with one symbol argument)
};

// instantiate the class (constructor has a variable argument list)
FLEXT_NEW_V("simple3",simple3)


simple3::simple3(int argc,t_atom *argv)
{ 
	post("%i creation arguments",argc);

	// define inlets
	AddInAnything();  // first inlet of type anything (index 0)
	
	 // set up inlets and outlets 
	SetupInOut(); 

	// register methods
	FLEXT_ADDMETHOD_(0,"born",m_tag);  // register method for tag "born"
	FLEXT_ADDMETHOD_(0,"to",m_tag);  // register method for tag "to"
	FLEXT_ADDMETHOD_(0,"hula",m_tag);  // register method for tag "hula"
	FLEXT_ADDMETHOD(0,m_sym);  // register method for all other symbol
} 

void simple3::m_tag()
{
	post("tag used");
}

void simple3::m_sym(t_symbol *s)
{
	post("symbol: %s",GetString(s));
}


