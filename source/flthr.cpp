/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flthr.cpp
    \brief Implementation of the flext thread functionality.
*/
 
#include "flext.h"
#include "flinternal.h"

#ifdef FLEXT_THREADS

#include <errno.h>

//! Thread id of system thread
flext::thrid_t flext::thrid = 0;

//! Thread id of helper thread
flext::thrid_t flext::thrhelpid = 0;

/*
flext::thr_entry *flext::thrhead = NULL,*flext::thrtail = NULL;
flext::ThrMutex flext::tlmutex;
*/
static flext::thr_entry *thrhead = NULL,*thrtail = NULL;
static flext::ThrMutex tlmutex;

//! Helper thread should terminate
static bool thrhelpexit = false;

//! Helper thread conditional
static flext::ThrCond *thrhelpcond = NULL;


//! Start helper thread
bool flext::StartHelper()
{
	if(thrhelpid) return true;
	
	if(!thrid) {
		// system thread has not been set
		ERRINTERNAL();
		return false;
	}

	bool ok = false;
#if FLEXT_THREADS == FLEXT_THR_POSIX
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	thrhelpexit = false;
	int ret = pthread_create (&thrhelpid,&attr,(void *(*)(void *))ThrHelper,NULL);
	ok = !ret;
#elif FLEXT_THREADS == FLEXT_THR_MP
	if(!MPLibraryIsLoaded())
		error("Thread library is not loaded");
	else {
		OSStatus ret = MPCreateTask((TaskProc)ThrHelper,NULL,0,0,0,0,0,&thrhelpid);
		ok = ret == noErr;
	}
#else
#error
#endif
	if(!ok)
		error("flext - Could not launch helper thread!"); 
    else {
        // now we have to wait for thread helper to initialize
        while(!thrhelpid || !thrhelpcond) Sleep(0.001);

        // we are ready for threading now!
    }
	return ok;
}

#if 0
/*! \brief Stop helper thread
	\note Never called!
*/
bool flext::StopHelper()
{
	thrhelpexit = true;
	if(thrhelpcond) thrhelpcond->Signal();
}
#endif


//! Static helper thread function
void flext::ThrHelper(void *)
{
#if FLEXT_THREADS == FLEXT_THR_POSIX
	// set prototype thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
#endif

//	post("Helper thread started");

	// set thread priority one point below normal
	// so thread construction won't disturb real-time audio
	RelPriority(-1);

	thrhelpcond = new ThrCond;

	// helper loop
	for(;;) {
//		post("Helper loop");

		thrhelpcond->Wait();
		if(thrhelpexit) break;

//		post("Helper signalled");
		tlmutex.Lock();

//		post("Helper after lock");

		// start all inactive threads in queue
		thr_entry *prv = NULL,*ti;
		for(ti = thrhead; ti; prv = ti,ti = ti->nxt) {
			if(!ti->active) {
				bool ok;
				
//				post("Helper start thread");
#if FLEXT_THREADS == FLEXT_THR_POSIX
				ok = pthread_create (&ti->thrid,&attr,(void *(*)(void *))ti->meth,ti->params) == 0;
#elif FLEXT_THREADS == FLEXT_THR_MP
				ok = MPCreateTask((TaskProc)ti->meth,ti->params,0,0,0,0,0,&ti->thrid) == noErr;
#else
#error
#endif
				if(!ok) { 
					error("flext - Could not launch thread!"); 
					
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
//					post("Helper thread ok");

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


bool flext::LaunchThread(void (*meth)(thr_params *p),thr_params *p)
{
#ifdef FLEXT_DEBUG
	if(!thrhelpcond) {
		ERRINTERNAL(); 
		return false;
	}
#endif

//	post("make threads");

	tlmutex.Lock();

	// make an entry into thread list
	thr_entry *nt = new thr_entry(meth,p);
	if(thrtail) thrtail->nxt = nt; 
	else thrhead = nt;
	thrtail = nt;

	tlmutex.Unlock();

//	post("signal helper");

	// signal thread helper
	thrhelpcond->Signal();

	return true;
}

bool flext_base::ShouldExit() const 
{
	bool ret = true;

	tlmutex.Lock();
	for(thr_entry *ti = thrhead; ti; ti = ti->nxt)
		if(ti->Is()) { ret = ti->shouldexit; break; }
	tlmutex.Unlock();

	// thread was not found -> EXIT!!!
	return ret;
}

bool flext::PushThread()
{
	// set priority of newly created thread one point below the system thread's
	RelPriority(-1);
	return true;
}

void flext::PopThread()
{
	tlmutex.Lock();

//	post("Pop thread");

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
		post("flext - INTERNAL ERROR: Thread not found!");
#endif
	}
	
	tlmutex.Unlock();
}

//! Terminate all object threads
bool flext_base::StopThreads()
{
	thr_entry *t;

	// signal termination for all object's threads
	tlmutex.Lock();
	for(t = thrhead; t; t = t->nxt) {
		if(t->This() == this) t->shouldexit = true;
	}
	tlmutex.Unlock();

	// TODO: maybe there should be a thread conditional for every thread so that it can be signalled efficiently

	// wait for thread termination (1 second max.)
	int cnt;
	for(int wi = 0; wi < 100; ++wi) {
        // lock and count this object's threads
		tlmutex.Lock();
		for(cnt = 0,t = thrhead; t; t = t->nxt)
			if(t->This() == this) ++cnt;
		tlmutex.Unlock();

        // if no threads are remaining, we are done
		if(!cnt) break;

        // Wait
		Sleep(0.01f);
	}

	if(cnt) {
#ifdef FLEXT_DEBUG
		post("flext - doing hard thread termination");
#endif
	
		// --- all object threads have terminated by now -------

		qmutex.Lock(); // Lock message queue
		tlmutex.Lock();

		// timeout -> hard termination
		for(t = thrhead; t; )
			if(t->This() == this) {
		#if FLEXT_THREADS == FLEXT_THR_POSIX
				if(pthread_cancel(t->thrid)) post("%s - Thread could not be terminated!",thisName());
		#elif FLEXT_THREADS == FLEXT_THR_MP
				MPTerminateTask(t->thrid,0);
				// here, we should use a task queue to check whether the task has really terminated!!
		#else
		#error
		#endif
				thr_entry *tn = t->nxt;
				t->nxt = NULL; delete t;
				t = tn;
			}
			else t = t->nxt;

		tlmutex.Unlock();
		qmutex.Unlock();
	}
	
//	post("All threads have terminated");
	
	return true;
}

bool flext::RelPriority(int dp,thrid_t ref,thrid_t id)
{
#if FLEXT_THREADS == FLEXT_THR_POSIX
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
#elif FLEXT_THREADS == FLEXT_THR_MP
	thr_entry *t;
	for(t = thrhead; t && t->Id() != id; t = t->nxt) {}
	if(t) {
		// thread found in list
		int w = GetPriority(id);
		if(dp < 0) w /= 1<<(-dp);
		else w *= 1<<dp;
		if(w < 1) {
	#ifdef FLEXT_DEBUG		
			post("flext - minimum thread priority reached");
	#endif
			w = 1;
		}
		else if(w > 10000) {
	#ifdef FLEXT_DEBUG		
			post("flext - maximum thread priority reached");
	#endif
			w = 10000;
		}
		t->weight = w;
		return MPSetTaskWeight(id,w) == noErr;
	}
	else return false;
#else
#error
#endif
}


int flext::GetPriority(thrid_t id)
{
#if FLEXT_THREADS == FLEXT_THR_POSIX
	sched_param parm;
	int policy;
	if(pthread_getschedparam(id,&policy,&parm) < 0) {
#ifdef FLEXT_DEBUG
		post("flext - failed to get parms");
#endif
		return -1;
	}
	return parm.sched_priority;
#elif FLEXT_THREADS == FLEXT_THR_MP
	thr_entry *t;
	for(t = thrhead; t && t->Id() != id; t = t->nxt) {}
	return t?t->weight:-1;
#else
#error
#endif
}


bool flext::SetPriority(int p,thrid_t id)
{
#if FLEXT_THREADS == FLEXT_THR_POSIX
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
#elif FLEXT_THREADS == FLEXT_THR_MP
	thr_entry *t;
	for(t = thrhead; t && t->Id() != id; t = t->nxt) {}
	if(t)
		return MPSetTaskWeight(id,t->weight = p) == noErr;
	else
		return false;
#else
#error
#endif
}


flext_base::thr_params::thr_params(int n): cl(NULL),var(new _data[n]) {}
flext_base::thr_params::~thr_params() { if(var) delete[] var; }

void flext_base::thr_params::set_any(const t_symbol *s,int argc,const t_atom *argv) { var[0]._any.args = new AtomAnything(s,argc,argv); }
void flext_base::thr_params::set_list(int argc,const t_atom *argv) { var[0]._list.args = new AtomList(argc,argv); }


flext_base::thr_entry::thr_entry(void (*m)(thr_params *),thr_params *p,thrid_t id): 
	th(p?p->cl:NULL),meth(m),params(p),thrid(id),
	active(false),shouldexit(false),
#if FLEXT_THREADS == FLEXT_THR_MP
	weight(100), // MP default weight
#endif
	nxt(NULL) 
{}


#endif // FLEXT_THREADS
