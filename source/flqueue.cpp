/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flqueue.cpp
    \brief Implementation of the flext message queuing functionality.

    \todo Let's see if queuing can be implemented for Max/MSP with defer_low

    if FLEXT_PDLOCK is defined, the new PD thread lock functions are used
*/

#include "flext.h"
#include "flinternal.h"

class flext_base::qmsg
{
public:
	qmsg(flext_base *b): th(b),nxt(NULL),tp(tp_none) {}
	~qmsg();

	qmsg *nxt;

	void Clear();

	void SetBang(int o) { Clear(); out = o; tp = tp_bang; }
	void SetFloat(int o,float f) { Clear(); out = o; tp = tp_float; _float = f; }
	void SetInt(int o,int i) { Clear(); out = o; tp = tp_int; _int = i; }
	void SetSymbol(int o,const t_symbol *s) { Clear(); out = o; tp = tp_sym; _sym = s; }
	void SetList(int o,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_list; _list.argc = argc,_list.argv = CopyList(argc,argv); }
	void SetAny(int o,const t_symbol *s,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_any; _any.s = s,_any.argc = argc,_any.argv = CopyList(argc,argv); }

	flext_base *th;
	int out;
	enum { tp_none,tp_bang,tp_float,tp_int,tp_sym,tp_list,tp_any } tp;
	union {
		float _float;
		int _int;
		const t_symbol *_sym;
		struct { int argc; t_atom *argv; } _list;
		struct { const t_symbol *s; int argc; t_atom *argv; } _any;
	};
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

#if FLEXT_SYS == FLEXT_SYS_JMAX
void flext_base::QTick(int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#else
void flext_base::QTick()
{
#endif
//	post("qtick");
#if defined(FLEXT_THREADS) && defined(FLEXT_DEBUG) && !defined(FLEXT_QTHR)
	if(!IsSystemThread()) {
		error("flext - Queue tick called by wrong thread!");
		return;
	}
#endif

#ifdef FLEXT_THREADS
	qmutex.Lock();
#endif
#ifdef FLEXT_QTHR
    pd_lock();
#endif
	for(;;) {
		qmsg *m = qhead;
		if(!m) break;

        if(m->out < 0) {
            // message to self

            const int n = -1-m->out;
            t_atom tmp;

		    switch(m->tp) {
		    case qmsg::tp_bang: 
                m->th->m_methodmain(n,sym_bang,0,&tmp); 
                break;
		    case qmsg::tp_float: 
                SetFloat(tmp,m->_float);
                m->th->m_methodmain(n,sym_float,1,&tmp); 
                break;
            case qmsg::tp_int: 
                SetInt(tmp,m->_int);
#if FLEXT_SYS == FLEXT_SYS_PD
                m->th->m_methodmain(n,sym_float,1,&tmp); 
#elif FLEXT_SYS == FLEXT_SYS_MAX
                m->th->m_methodmain(n,sym_int,1,&tmp); 
#else
#error Not implemented!
#endif
		    case qmsg::tp_sym: 
                SetSymbol(tmp,m->_sym);
                m->th->m_methodmain(n,sym_symbol,1,&tmp); 
                break;
		    case qmsg::tp_list: 
                m->th->m_methodmain(n,sym_list,m->_list.argc,m->_list.argv); 
                break;
		    case qmsg::tp_any: 
                m->th->m_methodmain(n,m->_any.s,m->_any.argc,m->_any.argv); 
                break;
    #ifdef FLEXT_DEBUG
		    default: ERRINTERNAL();
    #endif
		    }
        }
        else {
            // message to outlet

		    switch(m->tp) {
		    case qmsg::tp_bang: m->th->ToOutBang(m->out); break;
		    case qmsg::tp_float: m->th->ToOutFloat(m->out,m->_float); break;
		    case qmsg::tp_int: m->th->ToOutInt(m->out,m->_int); break;
		    case qmsg::tp_sym: m->th->ToOutSymbol(m->out,m->_sym); break;
		    case qmsg::tp_list: m->th->ToOutList(m->out,m->_list.argc,m->_list.argv); break;
		    case qmsg::tp_any: m->th->ToOutAnything(m->out,m->_any.s,m->_any.argc,m->_any.argv); break;
    #ifdef FLEXT_DEBUG
		    default: ERRINTERNAL();
    #endif
		    }
        }

		qhead = m->nxt;
		if(!qhead) qtail = NULL;
		m->nxt = NULL;
		delete m;
	}
#ifdef FLEXT_QTHR
    pd_unlock();
#endif
#ifdef FLEXT_THREADS
	qmutex.Unlock();
#endif
}

/*
It would be sufficient to only send messages belonging to object th
But then the order is not as intended
*/
void flext_base::QFlush(flext_base *th)
{
	while(qhead) QTick();
}

void flext_base::Queue(qmsg *m)
{
//	post("Queue");

#ifdef FLEXT_THREADS
	qmutex.Lock();
#endif
	if(qtail) qtail->nxt = m;
	else qhead = m;
	qtail = m;
#ifdef FLEXT_THREADS
	qmutex.Unlock();
#endif

#if FLEXT_SYS == FLEXT_SYS_PD
    #ifdef FLEXT_QTHR
        // wake up a worker thread 
        // (instead of triggering the clock)
		qthrcond.Signal();
	#else
	    clock_delay(qclk,0);
    #endif
#elif FLEXT_SYS == FLEXT_SYS_MAX
	qelem_set(qclk); 
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	// this is dangerous because there may be other timers on this object!
	fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)thisHdr(), QTick, NULL, 0);
#else
#error
#endif
}

#ifdef FLEXT_QTHR
ThrCond flext_base::qthrcond;
#else
#endif

void flext_base::StartQueue()
{
	// message queue ticker
	qhead = qtail = NULL;
#ifdef FLEXT_QTHR
	for(;;) {
		qthrcond.Wait();
		QTick();
	}
#else
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
	qclk = (t_qelem *)(qelem_new(this,(t_method)QTick));
#else
#error Not implemented!
#endif
#endif
}
#endif

void flext_base::ToQueueBang(int o) const 
{
	qmsg *m = new qmsg(this); 
	m->SetBang(o);
	Queue(m);
}

void flext_base::ToQueueFloat(int o,float f) const
{
	qmsg *m = new qmsg(this); 
	m->SetFloat(o,f);
	Queue(m);
}

void flext_base::ToQueueInt(int o,int f) const
{
	qmsg *m = new qmsg(this); 
	m->SetInt(o,f);
	Queue(m);
}

void flext_base::ToQueueSymbol(int o,const t_symbol *s) const
{
	qmsg *m = new qmsg(this); 
	m->SetSymbol(o,s);
	Queue(m);
}

void flext_base::ToQueueList(int o,int argc,const t_atom *argv) const
{
	qmsg *m = new qmsg(this); 
	m->SetList(o,argc,argv);
	Queue(m);
}

void flext_base::ToQueueAnything(int o,const t_symbol *s,int argc,const t_atom *argv) const
{
	qmsg *m = new qmsg(this); 
	m->SetAny(o,s,argc,argv);
	Queue(m);
}


void flext_base::ToSelfBang(int n) const 
{
    ToQueueBang(-1-n);
}

void flext_base::ToSelfFloat(int n,float f) const
{
    ToQueueFloat(-1-n,f);
}

void flext_base::ToSelfInt(int n,int f) const
{
    ToQueueInt(-1-n,f);
}

void flext_base::ToSelfSymbol(int n,const t_symbol *s) const
{
    ToQueueSymbol(-1-n,s);
}

void flext_base::ToSelfList(int n,int argc,const t_atom *argv) const
{
    ToQueueList(-1-n,argc,argv);
}

void flext_base::ToSelfAnything(int n,const t_symbol *s,int argc,const t_atom *argv) const
{
    ToQueueAnything(-1-n,s,argc,argv);
}

