/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flthr.cpp
    \brief Implementation of the flext thread functionality.
*/
 
#ifdef FLEXT_THREADS

#include "flext.h"
#include "flinternal.h"

#ifdef MAXMSP
#define SCHEDTICK 1
#endif

#ifdef NT
#include <windows.h>
#endif
#include <errno.h>

//namespace flext {

bool flext_base::StartThread(void *(*meth)(thr_params *p),thr_params *p,char *methname)
{
	static bool init = false;
	static pthread_attr_t attr;
#ifdef _DEBUG
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
#ifdef _DEBUG	
		error((char *)(ret == EAGAIN?"%s - Unsufficient resources to launch thread!":"%s - Could not launch method!"),methname); 
#endif		
		delete p; 
		return false;
	}
	else {
#ifdef MAXMSP
		sched_yield();
#endif
		return true;
	}
}

bool flext_base::PushThread()
{
	tlmutex.Lock();

	// make an entry into thread list
	thr_entry *nt = new thr_entry;
	if(thrtail) thrtail->nxt = nt; 
	else thrhead = nt;
	thrtail = nt;

	{
#ifdef NT
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
	
#ifdef MAXMSP
	clock_delay(yclk,0);
#endif
	return true;
}

void flext_base::PopThread()
{
	tlmutex.Lock();

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
#ifdef _DEBUG
		post("%s - INTERNAL ERROR: Thread not found!",thisName());
#endif
	}
	
	tlmutex.Unlock();
}

#ifdef MAXMSP
void flext_base::YTick(flext_base *th) 
{ 
	clock_delay(th->yclk,0); 
	qelem_set(th->qclk); 
	sched_yield();
}
#endif

flext_base::thrid_t flext_base::GetThreadId() 
{ 
	return pthread_self(); 
}

bool flext_base::ChangePriority(int dp,thrid_t id)
{
	sched_param parm;
	int policy;
	if(pthread_getschedparam(id,&policy,&parm) < 0) {
#ifdef _DEBUG
		post("flext - failed to get parms");
#endif
		return false;
	}
	else {
		parm.sched_priority += dp;
		if(pthread_setschedparam(id,policy,&parm) < 0) {
#ifdef _DEBUG		
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
#ifdef _DEBUG
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
#ifdef _DEBUG
		post("flext - failed to get parms");
#endif
		return false;
	}
	else {
		parm.sched_priority = p;
		if(pthread_setschedparam(id,policy,&parm) < 0) {
#ifdef _DEBUG		
			post("flext - failed to change priority");
#endif
			return false;
		}
	}
	return true;
}



class flext_base::qmsg
{
public:
	qmsg(): nxt(NULL),tp(tp_none) {}
	~qmsg();

	qmsg *nxt;

	void Clear();

	void SetBang(outlet *o) { Clear(); out = o; tp = tp_bang; }
	void SetFloat(outlet *o,float f) { Clear(); out = o; tp = tp_float; _float = f; }
	void SetInt(outlet *o,int i) { Clear(); out = o; tp = tp_int; _int = i; }
	void SetSymbol(outlet *o,const t_symbol *s) { Clear(); out = o; tp = tp_sym; _sym = s; }
	void SetList(outlet *o,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_list; _list.argc = argc,_list.argv = CopyList(argc,argv); }
	void SetAny(outlet *o,const t_symbol *s,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_any; _any.s = s,_any.argc = argc,_any.argv = CopyList(argc,argv); }

	outlet *out;
	enum { tp_none,tp_bang,tp_float,tp_int,tp_sym,tp_list,tp_any } tp;
	union {
		float _float;
		int _int;
		const t_symbol *_sym;
		struct { int argc; t_atom *argv; } _list;
		struct { const t_symbol *s; int argc; t_atom *argv; } _any;
	};

//	void Add(qmsg *o);
};

flext_base::qmsg::~qmsg() 
{ 
	Clear();
	if(nxt) delete nxt; 
}

void flext_base::qmsg::Clear() 
{ 
	if(tp == tp_list) { if(_list.argv) delete[] _list.argv; }
	else if(tp == tp_any) { if(_any.argv) delete[] _any.argv; }
	tp = tp_none;
}

/*
void flext_base::qmsg::Add(qmsg *o) 
{
	if(nxt) nxt->Add(o);
	else nxt = o;
}
*/

void flext_base::QTick(flext_base *th)
{
#ifdef DEBUG
	if(!th->IsSystemThread()) {
		error("flext - Queue tick called by wrong thread!");
		return;
	}
#endif

	th->qmutex.Lock();
	while(th->qhead) {
		qmsg *m = th->qhead;

#ifdef MAXMSP
		short state = lockout_set(1);
#endif

		switch(m->tp) {
		case qmsg::tp_bang: th->ToOutBang(m->out); break;
		case qmsg::tp_float: th->ToOutFloat(m->out,m->_float); break;
		case qmsg::tp_int: th->ToOutInt(m->out,m->_int); break;
		case qmsg::tp_sym: th->ToOutSymbol(m->out,m->_sym); break;
		case qmsg::tp_list: th->ToOutList(m->out,m->_list.argc,m->_list.argv); break;
		case qmsg::tp_any: th->ToOutAnything(m->out,m->_any.s,m->_any.argc,m->_any.argv); break;
#ifdef DEBUG
		default: ERRINTERNAL();
#endif
		}

#ifdef MAXMSP
		lockout_set(state);
#endif

		th->qhead = m->nxt;
		if(!th->qhead) th->qtail = NULL;
		m->nxt = NULL;
		delete m;
	}
	th->qmutex.Unlock();
}

void flext_base::Queue(qmsg *m)
{
	qmutex.Lock();
	if(qtail) qtail->nxt = m;
	else qhead = m;
	qtail = m;
	qmutex.Unlock();
#ifdef PD
	clock_delay(qclk,0);
#elif defined(MAXMSP)
	clock_delay(yclk,0);
#else
	#error "To implement!"
#endif
}

void flext_base::ToQueueBang(outlet *o) 
{
	qmsg *m = new qmsg(); 
	m->SetBang(o);
	Queue(m);
}

void flext_base::ToQueueFloat(outlet *o,float f)
{
	qmsg *m = new qmsg; 
	m->SetFloat(o,f);
	Queue(m);
}

void flext_base::ToQueueInt(outlet *o,int f)
{
	qmsg *m = new qmsg; 
	m->SetInt(o,f);
	Queue(m);
}

void flext_base::ToQueueSymbol(outlet *o,const t_symbol *s)
{
	qmsg *m = new qmsg; 
	m->SetSymbol(o,s);
	Queue(m);
}

void flext_base::ToQueueList(outlet *o,int argc,const t_atom *argv)
{
	qmsg *m = new qmsg; 
	m->SetList(o,argc,argv);
	Queue(m);
}

void flext_base::ToQueueAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv)
{
	qmsg *m = new qmsg; 
	m->SetAny(o,s,argc,argv);
	Queue(m);
}



flext_base::thr_params::thr_params(flext_base *c,int n): cl(c),var(new _data[n]) {}
flext_base::thr_params::~thr_params() { if(var) delete[] var; }

void flext_base::thr_params::set_any(const t_symbol *s,int argc,const t_atom *argv) { var[0]._any.args = new AtomAnything(s,argc,argv); }
void flext_base::thr_params::set_list(int argc,const t_atom *argv) { var[0]._list.args = new AtomList(argc,argv); }

//} // namespace flext

#endif // FLEXT_THREADS
