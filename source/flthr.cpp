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
#include <sched.h>
#endif

#ifdef NT
#include <windows.h>
#endif

bool flext_base::StartThread(void *(*meth)(thr_params *p),thr_params *p,char *methname)
{
#ifdef _DEBUG
	if(!p) {
		ERRINTERNAL(); 
		return false;
	}
#endif

	pthread_t thrid; 
	if(pthread_create (&thrid,NULL,(void *(*)(void *))meth,p)) { 
		error("%s - Could not launch method!",methname); 
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
	thr_entry *nt = new thr_entry;
	if(thrtail) thrtail->nxt = nt; 
	else thrhead = nt;
	thrtail = nt;
	tlmutex.Unlock();
	
#ifdef MAXMSP
	clock_delay(yclk,0);
#endif
	return true;
}

void flext_base::PopThread()
{
	tlmutex.Lock();
	thr_entry *prv = NULL,*ti;
	for(ti = thrhead; ti; prv = ti,ti = ti->nxt)
		if(ti->Is()) break;

	if(ti) {
		if(prv) 
			prv->nxt = ti->nxt;
		else 
			thrhead = thrtail = ti->nxt;
		ti->nxt = NULL;
		delete ti;
	}
	else {
#ifdef _DEBUG
		post("%s - Am i too slow? - Thread not found!",thisName());
#endif
	}
	
	tlmutex.Unlock();
}

#ifdef MAXMSP
void flext_base::Yield() { sched_yield(); }

void flext_base::YTick(flext_base *th) { 
	clock_delay(th->yclk,0); 
	qelem_set(th->qclk); 
	sched_yield();
}
#endif


void flext_base::NormalPriority()
{
#ifdef NT
	HANDLE thr = GetCurrentThread();
	SetThreadPriority(thr,THREAD_PRIORITY_NORMAL);
#else
	int policy = 0;
	sched_param parm;
	parm.sched_priority = 0;
	if(pthread_setschedparam(pthread_self(),policy,&parm)) {
		post("%s - failed to change priority",thisName());
	}
#endif
}


void flext_base::LowerPriority()
{
#ifdef NT
	HANDLE thr = GetCurrentThread();
	SetThreadPriority(thr,THREAD_PRIORITY_BELOW_NORMAL);
#else
	int policy = 0;
	sched_param parm;
	parm.sched_priority = -1;
	if(pthread_setschedparam(pthread_self(),policy,&parm)) {
		post("%s - failed to change priority",thisName());
	}
#endif
}


void flext_base::LowestPriority()
{
#ifdef NT
	HANDLE thr = GetCurrentThread();
	SetThreadPriority(thr,THREAD_PRIORITY_LOWEST);
#else
	int policy = 0;
	sched_param parm;
	parm.sched_priority = -2;
	if(pthread_setschedparam(pthread_self(),policy,&parm)) {
		post("%s - failed to change priority",thisName());
	}
#endif
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
	void SetList(outlet *o,int argc,t_atom *argv) { Clear(); out = o; tp = tp_list; _list.argc = argc,_list.argv = flext_base::CopyList(argc,argv); }
	void SetAny(outlet *o,const t_symbol *s,int argc,t_atom *argv) { Clear(); out = o; tp = tp_any; _any.s = s,_any.argc = argc,_any.argv = flext_base::CopyList(argc,argv); }

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

void flext_base::QueueBang(outlet *o) 
{
	qmsg *m = new qmsg(); 
	m->SetBang(o);
	Queue(m);
}

void flext_base::QueueFloat(outlet *o,float f)
{
	qmsg *m = new qmsg; 
	m->SetFloat(o,f);
	Queue(m);
}

void flext_base::QueueInt(outlet *o,int f)
{
	qmsg *m = new qmsg; 
	m->SetInt(o,f);
	Queue(m);
}

void flext_base::QueueSymbol(outlet *o,const t_symbol *s)
{
	qmsg *m = new qmsg; 
	m->SetSymbol(o,s);
	Queue(m);
}

void flext_base::QueueList(outlet *o,int argc,t_atom *argv)
{
	qmsg *m = new qmsg; 
	m->SetList(o,argc,argv);
	Queue(m);
}

void flext_base::QueueAnything(outlet *o,const t_symbol *s,int argc,t_atom *argv)
{
	qmsg *m = new qmsg; 
	m->SetAny(o,s,argc,argv);
	Queue(m);
}

#endif // FLEXT_THREADS
