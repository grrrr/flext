/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flqueue.cpp
    \brief Implementation of the flext message queuing functionality.
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

void flext_base::QTick(flext_base *th)
{
//	post("qtick");
#if defined(FLEXT_THREADS) && defined(FLEXT_DEBUG)
	if(!th->IsSystemThread()) {
		error("flext - Queue tick called by wrong thread!");
		return;
	}
#endif

#ifdef FLEXT_THREADS
	th->qmutex.Lock();
#endif
	for(;;) {
		qmsg *m = th->qhead;
		if(!m) break;

		CRITON();

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

		CRITOFF();

		th->qhead = m->nxt;
		if(!th->qhead) th->qtail = NULL;
		m->nxt = NULL;
		delete m;
	}
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
	clock_delay(qclk,0);
#elif FLEXT_SYS == FLEXT_SYS_MAX
	qelem_set(qclk); 
#else
#error 
#endif

}

void flext_base::ToQueueBang(outlet *o) const 
{
	qmsg *m = new qmsg(); 
	m->SetBang(o);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueFloat(outlet *o,float f) const
{
	qmsg *m = new qmsg; 
	m->SetFloat(o,f);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueInt(outlet *o,int f) const
{
	qmsg *m = new qmsg; 
	m->SetInt(o,f);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueSymbol(outlet *o,const t_symbol *s) const
{
	qmsg *m = new qmsg; 
	m->SetSymbol(o,s);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueList(outlet *o,int argc,const t_atom *argv) const
{
	qmsg *m = new qmsg; 
	m->SetList(o,argc,argv);
	const_cast<flext_base &>(*this).Queue(m);
}

void flext_base::ToQueueAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv) const
{
	qmsg *m = new qmsg; 
	m->SetAny(o,s,argc,argv);
	const_cast<flext_base &>(*this).Queue(m);
}

