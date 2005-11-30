/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
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
#include "flcontainers.h"
#include <string.h> // for memcpy

#ifdef FLEXT_THREADS
//! Thread id of message queue thread
flext::thrid_t flext::thrmsgid;
#endif

static bool qustarted = false;

#ifdef FLEXT_SHARED
/*
    For the shared version it _should_ be possible to have only one queue for all externals.
    Yet I don't know how to do this cross-platform
*/
#define PERMANENTIDLE
#endif

static void Trigger();

class qmsg:
    public flext,
    public Fifo::Cell
{
public:
    inline qmsg &Set(flext_base *t,int o,const t_symbol *s,int ac,const t_atom *av)
    {
        th = t;
        out = o;
        msg(s,ac,av);
        return *this;
    }

    inline qmsg &Set(const t_symbol *r,const t_symbol *s,int ac,const t_atom *av)
    {
        th = NULL;
        recv = r;
        msg(s,ac,av);
        return *this;
    }

    // \note PD sys lock must already be held by caller
    inline void Send() const
    {
        if(th) {
            if(out < 0)
                // message to self
                th->CbMethodHandler(-1-out,msg.Header(),msg.Count(),msg.Atoms()); 
            else
                // message to outlet
                th->ToSysAnything(out,msg.Header(),msg.Count(),msg.Atoms());
        }
        else
            flext::Forward(recv,msg,true);
    }

private:
    flext_base *th;
    union {
        int out;
        const t_symbol *recv;
    };
    AtomAnything msg;
};



typedef PooledFifo<qmsg> QueueFifo;

class Queue:
    public flext,
    public QueueFifo
{
public:
    inline bool Empty() const { return Size() == 0; }

    inline void Push(flext_base *t,int o,const t_symbol *s,int ac,const t_atom *av)
    {
        qmsg *m = QueueFifo::New();
        FLEXT_ASSERT(m);
        m->Set(t,o,s,ac,av);
        Put(m);
        Trigger();
    }

    inline void Push(const t_symbol *r,const t_symbol *s,int ac,const t_atom *av)
    {
        qmsg *m = QueueFifo::New();
        FLEXT_ASSERT(m);
        m->Set(r,s,ac,av);
        Put(m);
        Trigger();
    }

    inline void Push(flext_base *th,int o) // bang
    { 
        Push(th,o,sym_bang,0,NULL);
    }

    inline void Push(flext_base *th,int o,float dt) 
    { 
        t_atom at; 
        SetFloat(at,dt);
        Push(th,o,sym_float,1,&at);
    }

    inline void Push(flext_base *th,int o,int dt) 
    { 
        t_atom at; 
        SetInt(at,dt);
        const t_symbol *sym;
#if FLEXT_SYS == FLEXT_SYS_PD
        sym = sym_float;
#elif FLEXT_SYS == FLEXT_SYS_MAX
        sym = sym_int;
#else
#error Not implemented!
#endif
        Push(th,o,sym,1,&at);
    }

    inline void Push(flext_base *th,int o,const t_symbol *dt) 
    { 
        t_atom at; 
        SetSymbol(at,dt);
        Push(th,o,sym_symbol,1,&at);
    }

    void Push(flext_base *th,int o,const t_atom &a) 
    { 
        const t_symbol *sym;
        if(IsSymbol(a))
            sym = sym_symbol;
        else if(IsFloat(a))
            sym = sym_float;
#if FLEXT_SYS == FLEXT_SYS_MAX
        else if(IsInt(a))
            sym = sym_int;
#endif
#if FLEXT_SYS == FLEXT_SYS_PD
        else if(IsPointer(a))
            sym = sym_pointer;
#endif
        else {
            error("atom type not supported");
            return;
        }
        Push(th,o,sym,1,&a);
    }

    inline void Push(flext_base *th,int o,int argc,const t_atom *argv) 
    {
        Push(th,o,sym_list,argc,argv);
    }
};

static Queue queue;


#if FLEXT_QMODE == 2
static flext::ThrCond qthrcond;
#elif FLEXT_QMODE == 0
//static t_qelem *qclk = NULL;
static t_clock *qclk = NULL;
#endif


#define CHUNK 10

static void QWork(bool syslock)
{
    for(;;) {
        // Since qcnt can only be increased from any other function than QWork
        // qc will be a minimum guaranteed number of present queue elements.
        // On the other hand, if new queue elements are added by the methods called
        // in the loop, these will be sent in the next tick to avoid recursion overflow.
        size_t qc = queue.Size();
        if(!qc) break;

    #if FLEXT_QMODE == 2
        if(syslock) flext::Lock();
    #endif

        qmsg *q;
        while((q = queue.Get()) != NULL) {
            q->Send();
            queue.Free(q);
        }

    #if FLEXT_QMODE == 2
        if(syslock) flext::Unlock();
    #endif

    }
}

#if FLEXT_QMODE == 0
#if FLEXT_SYS == FLEXT_SYS_JMAX
static void QTick(fts_object_t *c,int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
#else
static void QTick(flext_base *c)
{
#endif
#ifdef FLEXT_THREADS
    FLEXT_ASSERT(flext::IsSystemThread());
#endif
    QWork(false);
}

#elif FLEXT_QMODE == 1
#ifndef PERMANENTIDLE
static bool qtickactive = false;
#endif
static t_int QTick(t_int *)
{
#ifndef PERMANENTIDLE
    qtickactive = false;
#endif

    QWork(false);

#ifdef PERMANENTIDLE
    // will be run in the next idle cycle
    return 2;
#else
    // won't be run again
    // for non-shared externals assume that there's rarely a message waiting
    // so it's better to delete the callback meanwhile
    return 0; 
#endif
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

    while(!queue.Empty()) QWork(false);
}

static void Trigger()
{
#if FLEXT_SYS == FLEXT_SYS_PD
    #if FLEXT_QMODE == 2
        // wake up worker thread
        qthrcond.Signal();
    #elif FLEXT_QMODE == 1 && !defined(PERMANENTIDLE)
        if(!qtickactive) {
            sys_callback(QTick,NULL,0);
            qtickactive = true;
        }
    #elif FLEXT_QMODE == 0
        clock_delay(qclk,0);
    #endif
#elif FLEXT_SYS == FLEXT_SYS_MAX
    #if FLEXT_QMODE == 0
//        qelem_front(qclk);
        clock_delay(qclk,0);
    #endif
#elif FLEXT_SYS == FLEXT_SYS_JMAX
    #if FLEXT_QMODE == 0
        // this is dangerous because there may be other timers on this object!
        fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)thisHdr(), QTick, NULL, 0);
    #endif
#else
#error Not implemented
#endif
}

#if FLEXT_QMODE == 2
void flext_base::QWorker(thr_params *)
{
    thrmsgid = GetThreadId();
    qustarted = true;
    for(;;) {
        qthrcond.Wait();
        QWork(true);
    }
}
#endif

void flext_base::StartQueue()
{
    if(qustarted) return;
#if FLEXT_QMODE == 1
#ifdef PERMANENTIDLE
    sys_callback(QTick,NULL,0);
    qustarted = true;
#endif
#elif FLEXT_QMODE == 2
    LaunchThread(QWorker,NULL);
    while(!qustarted) Sleep(0.001);
#elif FLEXT_QMODE == 0 && (FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX)
//    qclk = (t_qelem *)(qelem_new(NULL,(t_method)QTick));
    qclk = (t_clock *)(clock_new(NULL,(t_method)QTick));
    qustarted = true;
#else
#error Not implemented!
#endif
}

void flext_base::ToQueueBang(int o) const 
{
    queue.Push(const_cast<flext_base *>(this),o);
}

void flext_base::ToQueueFloat(int o,float f) const
{
    queue.Push(const_cast<flext_base *>(this),o,f);
}

void flext_base::ToQueueInt(int o,int f) const
{
    queue.Push(const_cast<flext_base *>(this),o,f);
}

void flext_base::ToQueueSymbol(int o,const t_symbol *s) const
{
    queue.Push(const_cast<flext_base *>(this),o,s);
}

void flext_base::ToQueueAtom(int o,const t_atom &at) const
{
    queue.Push(const_cast<flext_base *>(this),o,at);
}

void flext_base::ToQueueList(int o,int argc,const t_atom *argv) const
{
    queue.Push(const_cast<flext_base *>(this),o,argc,argv);
}

void flext_base::ToQueueAnything(int o,const t_symbol *s,int argc,const t_atom *argv) const
{
    queue.Push(const_cast<flext_base *>(this),o,s,argc,argv);
}


bool flext::Forward(const t_symbol *recv,const t_symbol *s,int argc,const t_atom *argv,bool direct)
{
    if(direct || IsSystemThread()) {
        void *cl = recv->s_thing;
        if(!cl) return false;
        
#if FLEXT_SYS == FLEXT_SYS_PD
        pd_typedmess((t_class **)cl,(t_symbol *)s,argc,(t_atom *)argv);
#elif FLEXT_SYS == FLEXT_SYS_MAX
        typedmess(recv->s_thing,(t_symbol *)s,argc,(t_atom *)argv);
#else
#error Not implemented
#endif
    }
    else
        // send over queue
        queue.Push(recv,s,argc,argv);
    return true;
}
