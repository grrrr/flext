/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flthr.cpp
    \brief Implementation of the flext thread functionality.
*/
 
#include "flext.h"
#include "flinternal.h"

#ifdef FLEXT_THREADS

#if FLEXT_OS == FLEXT_OS_WIN
#include <windows.h>
#endif

#include <errno.h>

bool flext_base::StartThread(void *(*meth)(thr_params *p),thr_params *p,char *methname)
{
	static bool init = false;
	static pthread_attr_t attr;
#ifdef FLEXT_DEBUG
	if(!p) {
		ERRINTERNAL(); 
		return false;
	}
#endif
	if(!init) {
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		init = true;
	}

	// set thread priority one point below normal
	// so thread construction won't disturb real-time audio
	pthread_t id = pthread_self();
	sched_param parm;
	int policy;
	pthread_getschedparam(id,&policy,&parm);
	int prio = parm.sched_priority;
	int schmin = sched_get_priority_min(policy);
	if(prio > schmin) {
		parm.sched_priority = prio-1;
		pthread_setschedparam(id,policy,&parm);
	}
	
	pthread_t thrid; 
	int ret = pthread_create (&thrid,&attr,(void *(*)(void *))meth,p);

	// set thread priority back to normal
	parm.sched_priority = prio;
	pthread_setschedparam(id,policy,&parm);

	if(ret) { 
#ifdef FLEXT_DEBUG	
		error((char *)(ret == EAGAIN?"%s - Unsufficient resources to launch thread!":"%s - Could not launch method!"),methname); 
#endif		
		error((char *)("%s - Could not launch method!"),methname); 

		delete p; 
		return false;
	}
	else
		return true;
}

bool flext_base::PushThread()
{
	tlmutex.Lock();

//	post("Push thread");

	// make an entry into thread list
	thr_entry *nt = new thr_entry;
	if(thrtail) thrtail->nxt = nt; 
	else thrhead = nt;
	thrtail = nt;

	{
#if FLEXT_OS == FLEXT_OS_WIN
	// set detached thread to lower priority class
	DWORD err;
	HANDLE thr = GetCurrentThread();
	DWORD cl = GetThreadPriority(thr);
	if(!cl && (err = GetLastError()))
		post("flext - error getting thread priority");
	else {
		BOOL ret = SetThreadPriority(thr,cl-2);
		if(!ret) {
			err = GetLastError();
			if(!err) post("flext - error setting thread priority");
		}
	}
#else
	// set initial detached thread priority two points below normal
	sched_param parm;
	int policy;
	if(pthread_getschedparam(nt->thrid,&policy,&parm))
		post("flext - can't get thread parameters");
	int prio = parm.sched_priority;
	int schmin = sched_get_priority_min(policy);
	if(prio > schmin) {
		parm.sched_priority = prio-2;
		if(pthread_setschedparam(nt->thrid,policy,&parm))
			post("flext - can't set thread parameters");
	}
#endif
	}

	tlmutex.Unlock();
	
	return true;
}

void flext_base::PopThread()
{
	tlmutex.Lock();

//	post("Pop thread");

	pthread_t id = pthread_self();

	thr_entry *prv = NULL,*ti;
	for(ti = thrhead; ti; prv = ti,ti = ti->nxt)
		if(ti->Is()) break;

	if(ti) {
		if(prv) 
			prv->nxt = ti->nxt;
		else 
			thrhead = ti->nxt;
		if(thrtail == ti) thrtail = prv;

		ti->nxt = NULL;
		delete ti;
	}
	else {
#ifdef FLEXT_DEBUG
		post("%s - INTERNAL ERROR: Thread not found!",thisName());
#endif
	}
	
	tlmutex.Unlock();
}

flext_base::thrid_t flext_base::GetThreadId() 
{ 
	return pthread_self(); 
}

bool flext_base::ChangePriority(int dp,thrid_t id)
{
	sched_param parm;
	int policy;
	if(pthread_getschedparam(id,&policy,&parm) < 0) {
#ifdef FLEXT_DEBUG
		post("flext - failed to get parms");
#endif
		return false;
	}
	else {
		parm.sched_priority += dp;
		if(pthread_setschedparam(id,policy,&parm) < 0) {
#ifdef FLEXT_DEBUG		
			post("flext - failed to change priority");
#endif
			return false;
		}
	}
	return true;
}


int flext_base::GetPriority(thrid_t id)
{
	sched_param parm;
	int policy;
	if(pthread_getschedparam(id,&policy,&parm) < 0) {
#ifdef FLEXT_DEBUG
		post("flext - failed to get parms");
#endif
		return -1;
	}
	return parm.sched_priority;
}


bool flext_base::SetPriority(int p,thrid_t id)
{
	sched_param parm;
	int policy;
	if(pthread_getschedparam(id,&policy,&parm) < 0) {
#ifdef FLEXT_DEBUG
		post("flext - failed to get parms");
#endif
		return false;
	}
	else {
		parm.sched_priority = p;
		if(pthread_setschedparam(id,policy,&parm) < 0) {
#ifdef FLEXT_DEBUG		
			post("flext - failed to change priority");
#endif
			return false;
		}
	}
	return true;
}

flext_base::thr_params::thr_params(flext_base *c,int n): cl(c),var(new _data[n]) {}
flext_base::thr_params::~thr_params() { if(var) delete[] var; }

void flext_base::thr_params::set_any(const t_symbol *s,int argc,const t_atom *argv) { var[0]._any.args = new AtomAnything(s,argc,argv); }
void flext_base::thr_params::set_list(int argc,const t_atom *argv) { var[0]._list.args = new AtomList(argc,argv); }

#endif // FLEXT_THREADS
