/* 
flext tutorial - threads 1

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This shows an example of a method running as a thread
*/

// define FLEXT_THREADS for thread usage. Flext must also have been compiled with that defined!
#define FLEXT_THREADS

#include <flext.h>

#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 300)
#error You need at least flext version 0.3.0
#endif



class thread1:
	public flext_base
{
	FLEXT_HEADER(thread1,flext_base)
 
public:
	thread1(); 

protected:
	void m_start(); // method function

private:
	// define threaded callback for method m_start (with boolean argument)
	// the same syntax as with FLEXT_CALLBACK is used here
	FLEXT_THREAD(m_start); 
};

FLEXT_NEW("thread1",thread1)



thread1::thread1()
{ 
	AddInAnything();
	AddOutInt(); 
	SetupInOut();  // set up inlets and outlets

	FLEXT_ADDBANG(0,m_start); // register method
} 

void thread1::m_start()
{
	// Please note that this functions needs about 10 seconds to complete
	// Without threads it would block PD

	for(int i = 0; i < 20; ++i) {
		ToOutInt(0,i); // output loop count
		Sleep(0.5f); // wait for half a second
	}

	// output a final zero
	ToOutInt(0,0);
}

