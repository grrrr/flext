/* 
flext tutorial - simple 3

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This is an example of a simple object doing number or string concatenation
*/

#include <flext.h>

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
	void m_any(t_symbol *s,int argc,t_atom *argv);  // method which digests anything
	void m_int(int f);   
	void m_sym(t_symbol *s); 
	
	// stored argument of right inlet
	bool isnumeric;
	t_symbol *arg;

private:
	FLEXT_CALLBACK_A(m_any);  // callback for method "m_any" (with anything argument)
	FLEXT_CALLBACK_I(m_int);  // callback for method "m_int" (with one integer argument)
	
	FLEXT_CALLBACK_S(m_sym);  // callback for method "m_sym" (with one symbol argument)
	// that's a shortcut for FLEXT_CALLBACK_1(m_sym,t_symptr) ... you have to use t_symptr instead of t_symbol *  !!!
};

// instantiate the class (constructor has a variable argument list)
FLEXT_NEW_V("simple3",simple3)


simple3::simple3(int argc,t_atom *argv):
	// initalize default argument values
	isnumeric(false),
	arg(gensym(""))
{ 
	// define inlets
	AddInAnything(2);  // make 2 inlets that can receive anything (indices 0 and 1)
	
	// define outlets
	AddOutAnything();  // one outlet for anything (has index 0)
	
	 // set up inlets and outlets
	SetupInOut(); 

	// register methods
	FLEXT_ADDMETHOD(0,m_any);  // register method "m_any" for inlet 0
	FLEXT_ADDMETHOD(1,m_int);  // register method (for integers) "m_int" for inlet 1
	FLEXT_ADDMETHOD(1,m_sym);  // register method (for symbols) "m_sym" for inlet 1
	
	// check for too many arguments
	if(argc > 1) {
		post("%s - too many arguments given: only the first one is used",thisName());
	}
	
	// digest argument list
	if(argc < 1) {
		post("%s - no argument given: assume empty string",thisName());
		// leave default values
	}
	else if(IsInt(argv[0])) { // check if first argument is an integer
		// get the integer argument
		m_int(GetInt(argv[0]));
	}
	else if(IsSymbol(argv[0])) { // check for symbol type
		// get the symbol argument
		m_sym(GetSymbol(argv[0]));
	}
	else { // unhandled type
		post("%s - wrong argument type: assume empty string",thisName());
		
		// tell flext that the initialization was not successful... object will not live
		InitProblem();
	}
} 

void simple3::m_any(t_symbol *s,int argc,t_atom *argv)
{
	char tmp[100]; // temporary space
	int needed;

	// sym_int and sym_float are predefined for gensym(int) and gensym(float), respectively
	if(s == sym_int || s == sym_float) { // check for int or float tag
		needed = 1;
		int v = GetAInt(argv[0]);
	
		// print concatenation
		sprintf(tmp,"%i%s",v,GetString(arg));	
	
		if(isnumeric) // if both were numbers -> output as number
			ToOutInt(0,atoi(tmp));
		else 
			ToOutString(0,tmp);
	} 
	else { // take s as symbol
		needed = 0;
	
		sprintf(tmp,"%s%s",GetString(s),GetString(arg));
		ToOutString(0,tmp);		
	}
	
	if(argc > needed)
		post("%s - superfluous arguments ignored!",thisName());
}

void simple3::m_int(int n)
{
	char tmp[20];
	sprintf(tmp,"%i",n);
	isnumeric = true;
	arg = gensym(tmp);  
}

void simple3::m_sym(t_symbol *s)
{
	isnumeric = false;
	arg = s;
}

