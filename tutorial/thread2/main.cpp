/* 
flext tutorial - threads 2 

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

-------------------------------------------------------------------------

This shows an example of multiple threads and syncing with a thread conditional
*/

#define FLEXT_THREADS
#include <flext.h>

#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 300)
#error You need at least flext version 0.3.0
#endif

class thread2:
	public flext_base
{
	FLEXT_HEADER(thread2,flext_base)
 
public:
	thread2(int del,t_symptr txt);

protected:
	void m_start(int st);
	void m_stop();
	void m_blip();

private:
	FLEXT_THREAD_I(m_start); // define threaded callback for method m_start
	FLEXT_CALLBACK(m_stop);  // normal callback for m_stop
	FLEXT_THREAD(m_blip); // define threaded callback for method m_blip

	float delay;
	bool stopit,running,blipping;
	int count;
	t_symbol *bliptxt;

	ThrCond cond;
};

FLEXT_NEW_2("thread2",thread2,int,t_symptr)


thread2::thread2(int del,t_symptr txt):
	delay(del/1000.f),
	bliptxt(txt),
	stopit(false),
	running(false),blipping(false)
{ 
	AddInAnything();  
	AddOutInt(); 
	SetupInOut();  // set up inlets and outlets

	FLEXT_ADDMETHOD(0,m_start);
	FLEXT_ADDMETHOD_(0,"stop",m_stop);
}

void thread2::m_start(int st)
{
	post("start 1");

	if(running) { count = st; return; }
	running = true;

	FLEXT_CALLMETHOD(m_blip);

	post("start 2");

	for(count = st; !ShouldExit() && !stopit; ++count) 
	{
		Sleep(delay);
		ToOutInt(0,count);
	}

	post("start 3");

	cond.Lock();
	running = false;
	cond.Signal();
	cond.Unlock();

	post("start 4");
}

void thread2::m_stop()
{
	post("stop 1");

	cond.Lock();
	stopit = true;
	while(running || blipping) cond.Wait();
	stopit = false;
	cond.Unlock();

	post("stop 2");
}

void thread2::m_blip()
{
	blipping = true;

	while(!ShouldExit() && !stopit) {
		post(const_cast<char *>(GetString(bliptxt)));
		Sleep(0.1f);
	}

	cond.Lock();
	blipping = false;
	cond.Signal();
	cond.Unlock();
}