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

//! Thread id of system thread
flext::thrid_t flext::thrid;

//! Thread id of helper thread
flext::thrid_t flext_base::thrhelpid;

flext_base::thr_entry *flext_base::thrhead = NULL,*flext_base::thrtail = NULL;
flext::ThrMutex flext_base::tlmutex;

//! Helper thread should terminate
bool thrhelpexit = false;

//! Helper thread conditional
flext::ThrCond *thrhelpcond = NULL;


flext_base::thr_entry::thr_entry(flext_base *t,void *(*m)(thr_params *),thr_params *p,pthread_t id): 
	th(t),meth(m),params(p),thrid(id),
	active(false),shouldexit(false),
	nxt(NULL) 
{}


//! Start helper thread
bool flext_base::StartHelper()
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	thrhelpexit = false;
	int ret = pthread_create (&thrhelpid,&attr,(void *(*)(void *))ThrHelper,NULL);
	if(ret) { 
		error((char *)("flext - Could not launch helper thread!")); 
		return false;
	}
	else
		return true;
}

#if 0
/*! \brief Stop helper thread
	\note not called!
*/
bool flext_base::StopHelper()
{
	thrhelpexit = true;
	if(thrhelpcond) thrhelpcond->Signal();
}
#endif

//! Static helper thread function
void flext_base::ThrHelper(void *)
{
	// set prototype thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	// set thread priority one point below normal
	// so thread construction won't disturb real-time audio
	RelPriority(-1);

	thrhelpcond = new ThrCond;

	// helper loop
	for(;;) {
		thrhelpcond->Wait();
		if(thrhelpexit) break;

		tlmutex.Lock();

		// start all inactive threads in queue
		thr_entry *prv = NULL,*ti;
		for(ti = thrhead; ti; prv = ti,ti = ti->nxt) {
			if(!ti->active) {
				int ret = pthread_create (&ti->thrid,&attr,(void *(*)(void *))ti->meth,ti->params);
				if(ret) { 
					error((char *)("flext - Could not launch thread!")); 
					
					// delete from queue
					if(prv) 
						prv->nxt = ti->nxt;
					else 
						thrhead = ti->nxt;
					if(thrtail == ti) thrtail = prv;

					ti->nxt = NULL;
					delete ti;
				}
				else {
					// set active flag
					ti->active = true;
				}
			}
		}

		tlmutex.Unlock();
	}

	delete thrhelpcond;
	thrhelpcond = NULL;
}


bool flext_base::StartThread(void *(*meth)(thr_params *p),thr_params *p,char *methname)
{
#ifdef FLEXT_DEBUG
	if(!p || !thrhelpcond) {
		ERRINTERNAL(); 
		return false;
	}
#endif

	tlmutex.Lock();

	// make an entry into thread list
	thr_entry *nt = new thr_entry(this,meth,p);
	if(thrtail) thrtail->nxt = nt; 
	else thrhead = nt;
	thrtail = nt;

	// signal thread helper
	thrhelpcond->Signal();

	tlmutex.Unlock();
	return true;
}

bool flext_base::ShouldExit() const 
{
	for(thr_entry *ti = thrhead; ti; ti = ti->nxt)
		if(ti->Is()) return ti->shouldexit;

	// thread was not found -> EXIT!!!
	return true;
}

bool flext_base::PushThread()
{
	// set priority of newly created thread one point below the system thread's
	RelPriority(-1);
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

//! Terminate all object threads
void flext_base::TermThreads()
{
	thr_entry *t;

	// signal termination
	for(t = thrhead; t; t = t->nxt)
		if(t->th == this) t->shouldexit = true;

	// TODO: maybe there should be a thread conditional for every thread so that it can be signaled

	// wait for thread termination
	for(int wi = 0; wi < 100; ++wi) {
		int cnt = 0;
		for(t = thrhead; t; t = t->nxt)
			if(t->th == this) ++cnt;
		if(!cnt) break;
		Sleep(0.01f);
	}

	// --- all object threads have terminated by now -------

	qmutex.Lock(); // Lock message queue
	tlmutex.Lock();

	// timeout -> hard termination
	for(t = thrhead; t; )
		if(t->th == this) {
			if(pthread_cancel(t->thrid)) post("%s - Thread could not be terminated!",thisName());
			thr_entry *tn = t->nxt;
			t->nxt = NULL; delete t;
			t = tn;
		}
		else t = t->nxt;

	tlmutex.Unlock();
	qmutex.Unlock();
}

bool flext::RelPriority(int dp,thrid_t ref,thrid_t id)
{
	sched_param parm;
	int policy;
	if(pthread_getschedparam(ref,&policy,&parm) < 0) {
#ifdef FLEXT_DEBUG
		post("flext - failed to get thread priority");
#endif
		return false;
	}
	else {
		parm.sched_priority += dp;

		// MSVC++ 6 produces wrong code with the following lines!!!
//		int schmin = sched_get_priority_min(policy);
//		int schmax = sched_get_priority_max(policy);

		if(parm.sched_priority < sched_get_priority_min(policy)) {
#ifdef FLEXT_DEBUG		
			post("flext - minimum thread priority reached");
#endif
			parm.sched_priority = sched_get_priority_min(policy);
		}
		else if(parm.sched_priority > sched_get_priority_max(policy)) {
#ifdef FLEXT_DEBUG		
			post("flext - maximum thread priority reached");
#endif
			parm.sched_priority = sched_get_priority_max(policy);
		}
		
		if(pthread_setschedparam(id,policy,&parm) < 0) {
#ifdef FLEXT_DEBUG		
			post("flext - failed to change thread priority");
#endif
			return false;
		}
	}
	return true;
}


int flext::GetPriority(thrid_t id)
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


bool flext::SetPriority(int p,thrid_t id)
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
