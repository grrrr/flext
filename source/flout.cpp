/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flout.cpp
    \brief Implementation of the flext outlet functionality.
*/

#include "flext.h"
#include "flinternal.h"

 
#ifdef MAXMSP
#define CRITON() short state = lockout_set(1)
#define CRITOFF() lockout_set(state) 
#else
#define CRITON() 
#define CRITOFF() 
#endif

#ifndef FLEXT_THREADS
void flext_base::ToOutBang(outlet *o) { CRITON(); outlet_bang((t_outlet *)o); CRITOFF(); }
void flext_base::ToOutFloat(outlet *o,float f) { CRITON(); outlet_float((t_outlet *)o,f); CRITOFF(); }
void flext_base::ToOutInt(outlet *o,int f) { CRITON(); outlet_flint((t_outlet *)o,f); CRITOFF(); }
void flext_base::ToOutSymbol(outlet *o,const t_symbol *s) { CRITON(); outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(s)); CRITOFF(); }
void flext_base::ToOutList(outlet *o,int argc,const t_atom *argv) { CRITON(); outlet_list((t_outlet *)o,gensym("list"),argc,(t_atom *)argv); CRITOFF(); }
void flext_base::ToOutAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv) { CRITON(); outlet_anything((t_outlet *)o,const_cast<t_symbol *>(s),argc,(t_atom *)argv); CRITOFF(); }
#else
void flext_base::ToOutBang(outlet *o) { if(IsSystemThread()) { CRITON(); outlet_bang((t_outlet *)o); CRITOFF(); } else ToQueueBang(o); }
void flext_base::ToOutFloat(outlet *o,float f) { if(IsSystemThread()) { CRITON(); outlet_float((t_outlet *)o,f); CRITOFF(); } else ToQueueFloat(o,f); }
void flext_base::ToOutInt(outlet *o,int f) { if(IsSystemThread()) { CRITON(); outlet_flint((t_outlet *)o,f); CRITOFF(); } else ToQueueInt(o,f); }
void flext_base::ToOutSymbol(outlet *o,const t_symbol *s) { if(IsSystemThread()) { CRITON(); outlet_symbol((t_outlet *)o,const_cast<t_symbol *>(s)); CRITOFF(); } else ToQueueSymbol(o,s); }
void flext_base::ToOutList(outlet *o,int argc,const t_atom *argv) { if(IsSystemThread()) { CRITON(); outlet_list((t_outlet *)o,gensym("list"),argc,(t_atom *)argv); CRITOFF(); } else ToQueueList(o,argc,(t_atom *)argv); }
void flext_base::ToOutAnything(outlet *o,const t_symbol *s,int argc,const t_atom *argv) { if(IsSystemThread()) { CRITON(); outlet_anything((t_outlet *)o,const_cast<t_symbol *>(s),argc,(t_atom *)argv); CRITOFF(); } else ToQueueAnything(o,s,argc,(t_atom *)argv); }
#endif


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

#ifdef FLEXT_THREADS
	th->qmutex.Lock();
#endif
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
#ifdef FLEXT_THREADS
	th->qmutex.Unlock();
#endif
}

void flext_base::Queue(qmsg *m)
{
#ifdef FLEXT_THREADS
	qmutex.Lock();
#endif
	if(qtail) qtail->nxt = m;
	else qhead = m;
	qtail = m;
#ifdef FLEXT_THREADS
	qmutex.Unlock();
#endif
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

