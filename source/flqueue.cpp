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
	qmsg(): nxt(NULL),tp(tp_none) {}
	~qmsg();

	qmsg *nxt;

	void Clear();

	void SetBang(int o) { Clear(); out = o; tp = tp_bang; }
	void SetFloat(int o,float f) { Clear(); out = o; tp = tp_float; _float = f; }
	void SetInt(int o,int i) { Clear(); out = o; tp = tp_int; _int = i; }
	void SetSymbol(int o,const t_symbol *s) { Clear(); out = o; tp = tp_sym; _sym = s; }
	void SetList(int o,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_list; _list.argc = argc,_list.argv = CopyList(argc,argv); }
	void SetAny(int o,const t_symbol *s,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_any; _any.s = s,_any.argc = argc,_any.argv = CopyList(argc,argv); }

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
void flext_base::QTick(fts_object_t *c, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
	flext_base *th = thisObject(c);
#else
void flext_base::QTick(flext_base *th)
{
#endif
//	post("qtick");
#if defined(FLEXT_THREADS) && defined(FLEXT_DEBUG) && !defined(FLEXT_PDLOCK)
	if(!th->IsSystemThread()) {
		error("flext - Queue tick called by wrong thread!");
		return;
	}
#endif

#ifdef FLEXT_THREADS
	th->qmutex.Lock();
#endif
#ifdef FLEXT_PDLOCK
    pd_lock();
#endif
	for(;;) {
		qmsg *m = th->qhead;
		if(!m) break;

        if(m->out < 0) {
            // message to self

            const int n = -1-m->out;
            t_atom tmp;

		    switch(m->tp) {
		    case qmsg::tp_bang: 
                th->m_methodmain(n,sym_bang,0,&tmp); 
                break;
		    case qmsg::tp_float: 
                SetFloat(tmp,m->_float);
                th->m_methodmain(n,sym_float,1,&tmp); 
                break;
            case qmsg::tp_int: 
                SetInt(tmp,m->_int);
#if FLEXT_SYS == FLEXT_SYS_PD
                th->m_methodmain(n,sym_float,1,&tmp); 
#elif FLEXT_SYS == FLEXT_SYS_MAX
                th->m_methodmain(n,sym_int,1,&tmp); 
#else
#error Not implemented!
#endif
		    case qmsg::tp_sym: 
                SetSymbol(tmp,m->_sym);
                th->m_methodmain(n,sym_symbol,1,&tmp); 
                break;
		    case qmsg::tp_list: 
                th->m_methodmain(n,sym_list,m->_list.argc,m->_list.argv); 
                break;
		    case qmsg::tp_any: 
                th->m_methodmain(n,m->_any.s,m->_any.argc,m->_any.argv); 
                break;
    #ifdef FLEXT_DEBUG
		    default: ERRINTERNAL();
    #endif
		    }
        }
        else {
            // message to outlet

		    switch(m->tp) {
		    case qmsg::tp_bang: th->ToOutBang(m->out); break;
		    case qmsg::tp_float: th->ToOutFloat(m->out,m->_float); break;
		    case qmsg::tp_int: th->ToOutInt(m->out,m->_int); break;
		    case qmsg::tp_sym: th->ToOutSymbol(m->out,m->_sym); break;
		    case qmsg::tp_list: th->ToOutList(m->out,m->_list.argc,m->_list.argv); break;
		    case qmsg::tp_any: th->ToOutAnything(m->out,m->_any.s,m->_any.argc,m->_any.argv); break;
    #ifdef FLEXT_DEBUG
		    default: ERRINTERNAL();
    #endif
		    }
        }

		th->qhead = m->nxt;
		if(!th->qhead) th->qtail = NULL;
		m->nxt = NULL;
		delete m;
	}
#ifdef FLEXT_PDLOCK
    pd_unlock();
#endif
#ifdef FLEXT_THREADS
	th->qmutex.Unlock();
#endif
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
    #ifdef FLEXT_PDLOCK
        // wake up a worker thread 
        // (instead of triggering the clock)
	    clock_delay(qclk,0);
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

void flext_base::ToQueueBang(int o) const 
{
	qmsg *m = new qmsg(); 
	m->SetBang(o);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueFloat(int o,float f) const
{
	qmsg *m = new qmsg; 
	m->SetFloat(o,f);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueInt(int o,int f) const
{
	qmsg *m = new qmsg; 
	m->SetInt(o,f);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueSymbol(int o,const t_symbol *s) const
{
	qmsg *m = new qmsg; 
	m->SetSymbol(o,s);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueList(int o,int argc,const t_atom *argv) const
{
	qmsg *m = new qmsg; 
	m->SetList(o,argc,argv);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueAnything(int o,const t_symbol *s,int argc,const t_atom *argv) const
{
	qmsg *m = new qmsg; 
	m->SetAny(o,s,argc,argv);
	const_cast<flext_base &>(*this).Queue(m);
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

