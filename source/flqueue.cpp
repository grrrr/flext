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

#ifdef FLEXT_THREADS
//! Thread id of message queue thread
flext::thrid_t flext::thrmsgid = 0;
#endif

class qmsg
{
public:
    qmsg(flext_base *b): nxt(NULL),th(b),tp(tp_none) {}
    ~qmsg();

    qmsg *nxt;

    void Clear();

    void SetBang(int o) { Clear(); out = o; tp = tp_bang; }
    void SetFloat(int o,float f) { Clear(); out = o; tp = tp_float; _float = f; }
    void SetInt(int o,int i) { Clear(); out = o; tp = tp_int; _int = i; }
    void SetSymbol(int o,const t_symbol *s) { Clear(); out = o; tp = tp_sym; _sym = s; }
    void SetList(int o,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_list; _list.argc = argc,_list.argv = flext::CopyList(argc,argv); }
    void SetAny(int o,const t_symbol *s,int argc,const t_atom *argv) { Clear(); out = o; tp = tp_any; _any.s = s,_any.argc = argc,_any.argv = flext::CopyList(argc,argv); }

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

qmsg::~qmsg() 
{ 
    Clear();
    if(nxt) delete nxt; 
}

void qmsg::Clear() 
{ 
    if(tp == tp_list) { if(_list.argv) delete[] _list.argv; }
    else if(tp == tp_any) { if(_any.argv) delete[] _any.argv; }
    tp = tp_none;
}

static volatile int qcnt = 0;
static qmsg *volatile qhead = NULL,*volatile qtail = NULL;

#ifdef FLEXT_QTHR
static flext::ThrCond qthrcond;
#else
static t_qelem *qclk = NULL;
#endif

#ifdef FLEXT_THREADS
static flext::ThrMutex qmutex;
#endif

#define CHUNK 10

static void QWork(bool qlock,bool syslock)
{
    for(;;) {
        // Since qcnt can only be increased from any other function than QWork
        // qc will be a minimum guaranteed number of present queue elements.
        // On the other hand, if new queue elements are added by the methods called
        // in the loop, these will be sent in the next tick to avoid recursion overflow.
        int qc = qcnt;
        if(!qc) break;

    #ifdef FLEXT_QTHR
        if(syslock) sys_lock();
    #endif

        for(int i = 0; i < qc && qhead; ++i) {
    #ifdef FLEXT_THREADS
            if(qlock) qmutex.Lock();
    #endif
            qmsg *m = qhead;
            qcnt--;
            qhead = m->nxt;
            if(!qhead) qtail = NULL;
            m->nxt = NULL;
    #ifdef FLEXT_THREADS
            if(qlock) qmutex.Unlock();
    #endif

            if(m->out < 0) {
                // message to self

                const int n = -1-m->out;
                t_atom tmp;

                switch(m->tp) {
                case qmsg::tp_bang: 
                    m->th->m_methodmain(n,flext::sym_bang,0,&tmp); 
                    break;
                case qmsg::tp_float: 
                    flext::SetFloat(tmp,m->_float);
                    m->th->m_methodmain(n,flext::sym_float,1,&tmp); 
                    break;
                case qmsg::tp_int: 
                    flext::SetInt(tmp,m->_int);
    #if FLEXT_SYS == FLEXT_SYS_PD
                    m->th->m_methodmain(n,flext::sym_float,1,&tmp); 
    #elif FLEXT_SYS == FLEXT_SYS_MAX
                    m->th->m_methodmain(n,flext::sym_int,1,&tmp); 
    #else
    #error Not implemented!
    #endif
                case qmsg::tp_sym: 
                    flext::SetSymbol(tmp,m->_sym);
                    m->th->m_methodmain(n,flext::sym_symbol,1,&tmp); 
                    break;
                case qmsg::tp_list: 
                    m->th->m_methodmain(n,flext::sym_list,m->_list.argc,m->_list.argv); 
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
                case qmsg::tp_bang: m->th->ToSysBang(m->out); break;
                case qmsg::tp_float: m->th->ToSysFloat(m->out,m->_float); break;
                case qmsg::tp_int: m->th->ToSysInt(m->out,m->_int); break;
                case qmsg::tp_sym: m->th->ToSysSymbol(m->out,m->_sym); break;
                case qmsg::tp_list: m->th->ToSysList(m->out,m->_list.argc,m->_list.argv); break;
                case qmsg::tp_any: m->th->ToSysAnything(m->out,m->_any.s,m->_any.argc,m->_any.argv); break;
        #ifdef FLEXT_DEBUG
                default: ERRINTERNAL();
        #endif
                }
            }

            // delete processed queue element
            delete m;
        } // inner loop

    #ifdef FLEXT_QTHR
        if(syslock) sys_unlock();
    #endif

    } // for(;;)
}

#if !defined(FLEXT_QTHR)
#if FLEXT_SYS == FLEXT_SYS_JMAX
static void QTick(fts_object_t *c,int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#else
static void QTick(flext_base *c)
{
#endif
//  post("qtick");
#ifdef FLEXT_THREADS
    FLEXT_ASSERT(flext::IsSystemThread());
#endif
    QWork(true,false);
}
#endif

/*
It would be sufficient to only flush messages belonging to object th
But then the order of sent messages is not as intended
*/
void flext_base::QFlush(flext_base *th)
{
#ifdef FLEXT_THREADS
    if(!IsSystemThread()) {
        error("flext - Queue flush called by wrong thread!");
        return;
    }
#endif
#ifdef FLEXT_THREADS
    qmutex.Lock();
#endif
    while(qcnt) QWork(false,false);
#ifdef FLEXT_THREADS
    qmutex.Unlock();
#endif
}

static void Queue(qmsg *m)
{
//  post("Queue");

#ifdef FLEXT_THREADS
    qmutex.Lock();
#endif
    if(qtail) qtail->nxt = m;
    else qhead = m;
    qtail = m;
    qcnt++;
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
#error Not implemented
#endif
}

#ifdef FLEXT_QTHR
void flext_base::QWorker(thr_params *)
{
    thrmsgid = GetThreadId();
    for(;;) {
        qthrcond.Wait();
        QWork(true,true);
    }
}
#endif

void flext_base::StartQueue()
{
    static bool started = false;
    if(started) return;
    else started = true;

    // message queue ticker
    qhead = qtail = NULL;
    qcnt = 0;

#ifdef FLEXT_QTHR
    LaunchThread(QWorker,NULL);
#else
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
    qclk = (t_qelem *)(qelem_new(NULL,(t_method)QTick));
#else
#error Not implemented!
#endif
#endif
}

void flext_base::ToQueueBang(int o) const 
{
    FLEXT_ASSERT(o >= 0);
        qmsg *m = new qmsg(const_cast<flext_base *>(this)); 
    m->SetBang(o);
    Queue(m);
}

void flext_base::ToQueueFloat(int o,float f) const
{
    FLEXT_ASSERT(o >= 0);
    qmsg *m = new qmsg(const_cast<flext_base *>(this)); 
    m->SetFloat(o,f);
    Queue(m);
}

void flext_base::ToQueueInt(int o,int f) const
{
    FLEXT_ASSERT(o >= 0);
    qmsg *m = new qmsg(const_cast<flext_base *>(this)); 
    m->SetInt(o,f);
    Queue(m);
}

void flext_base::ToQueueSymbol(int o,const t_symbol *s) const
{
    FLEXT_ASSERT(o >= 0);
    qmsg *m = new qmsg(const_cast<flext_base *>(this)); 
    m->SetSymbol(o,s);
    Queue(m);
}

void flext_base::ToQueueList(int o,int argc,const t_atom *argv) const
{
    FLEXT_ASSERT(o >= 0);
    qmsg *m = new qmsg(const_cast<flext_base *>(this)); 
    m->SetList(o,argc,argv);
    Queue(m);
}

void flext_base::ToQueueAnything(int o,const t_symbol *s,int argc,const t_atom *argv) const
{
    FLEXT_ASSERT(o >= 0);
    qmsg *m = new qmsg(const_cast<flext_base *>(this)); 
    m->SetAny(o,s,argc,argv);
    Queue(m);
}


void flext_base::ToSelfBang(int n) const 
{
    FLEXT_ASSERT(n >= 0);
    ToQueueBang(-1-n);
}

void flext_base::ToSelfFloat(int n,float f) const
{
    FLEXT_ASSERT(n >= 0);
    ToQueueFloat(-1-n,f);
}

void flext_base::ToSelfInt(int n,int f) const
{
    FLEXT_ASSERT(n >= 0);
    ToQueueInt(-1-n,f);
}

void flext_base::ToSelfSymbol(int n,const t_symbol *s) const
{
    FLEXT_ASSERT(n >= 0);
    ToQueueSymbol(-1-n,s);
}

void flext_base::ToSelfList(int n,int argc,const t_atom *argv) const
{
    FLEXT_ASSERT(n >= 0);
    ToQueueList(-1-n,argc,argv);
}

void flext_base::ToSelfAnything(int n,const t_symbol *s,int argc,const t_atom *argv) const
{
    FLEXT_ASSERT(n >= 0);
    ToQueueAnything(-1-n,s,argc,argv);
}

