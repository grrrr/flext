/*
flext - C++ layer for Max and Pure Data externals

Copyright (c) 2001-2020 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.
*/

/*! \file flbuf.cpp
    \brief Implementation of the buffer abstraction class.
*/
 
#ifndef __FLEXT_BUF_CPP
#define __FLEXT_BUF_CPP

#include "flext.h"
#include "flfeatures.h"
#include <set>

#include "flpushns.h"

#if FLEXT_SYS == FLEXT_SYS_PD
#define DIRTY_INTERVAL 0   // buffer dirty check in msec

FLEXT_TEMPLATE
class Buffers:
    public std::set<flext::buffer *>
{
public:
    static FLEXT_TEMPINST(Buffers) buffers;
};

FLEXT_TEMPIMPL(FLEXT_TEMPINST(Buffers) Buffers)::buffers;


FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext_obj))::cb_buffer_dsp(void *c,t_signal **sp)
{
    FLEXT_UNUSED(c);
    FLEXT_UNUSED(sp);
    for(FLEXT_TEMPINST(Buffers)::iterator it = FLEXT_TEMPINST(Buffers)::buffers.begin(); it != FLEXT_TEMPINST(Buffers)::buffers.end(); ++it)
		(*it)->Set();
} 
#endif

FLEXT_TEMPIMPL(FLEXT_CLASSDEF(flext))::buffer::buffer(const t_symbol *bn,bool delayed):
    sym(NULL),data(NULL),
    chns(0),frames(0)
{
#if FLEXT_SYS == FLEXT_SYS_PD
    arr = NULL;
    interval = DIRTY_INTERVAL;
    isdirty = false;
    ticking = false;
	
	// we probably should make this global... work on the set of registered buffers
    tick = clock_new(this,(t_method)cb_tick);
#endif

    if(bn) Set(bn,delayed);

    ClearDirty();
	
#if FLEXT_SYS == FLEXT_SYS_PD
	// register buffer
    FLEXT_ASSERT(FLEXT_TEMPINST(Buffers)::buffers.find(this) == FLEXT_TEMPINST(Buffers)::buffers.end());
	FLEXT_TEMPINST(Buffers)::buffers.insert(this);
#endif
}

FLEXT_TEMPIMPL(FLEXT_CLASSDEF(flext))::buffer::~buffer()
{
#if FLEXT_SYS == FLEXT_SYS_PD
    if(tick) clock_free(tick);
#endif

#if FLEXT_SYS == FLEXT_SYS_PD
	// unregister buffer
    FLEXT_ASSERT(FLEXT_TEMPINST(Buffers)::buffers.find(this) != FLEXT_TEMPINST(Buffers)::buffers.end());
    FLEXT_TEMPINST(Buffers)::buffers.erase(this);
#endif
}

FLEXT_TEMPIMPL(int FLEXT_CLASSDEF(flext))::buffer::Set(const t_symbol *s,bool nameonly)
{
    int ret = 0;
    bool valid = data != NULL; // valid now? (before change)

    if(s && sym != s) {
        ret = 1;
        data = NULL; 
        frames = 0;
        chns = 0; 
    }

    if(s && *GetString(s))  sym = s;

    if(!sym) {
        if(valid) ret = -1;
    }   
    else if(!nameonly) {
#if FLEXT_SYS == FLEXT_SYS_PD   
        arr = (t_garray *)pd_findbyclass(const_cast<t_symbol *>(sym), garray_class);
        if(!arr)
        {
            if (*GetString(sym)) FLEXT_LOG1("buffer: no such array '%s'",GetString(sym));
//            sym = NULL;
            if(valid) ret = -1;
        }
        else {
	        int frames1;
			FLEXT_ARRAYTYPE *data1;
	        if(!FLEXT_PD_ARRAYGRAB(arr, &frames1, &data1))
	        {
	            error("buffer: bad template '%s'",GetString(sym)); 
	            data = NULL;
	            frames = 0;
	            if(valid) ret = -1;
	        }
	        else {
	            ret = 0;
	            garray_usedindsp(arr);
	            if(frames != frames1) { frames = frames1; if(!ret) ret = 1; }
				Element *data2 = reinterpret_cast<Element *>(data1);
	            if(data != data2) { data = data2; if(!ret) ret = 1; }
	            chns = 1;
	        }
		}
#elif FLEXT_SYS == FLEXT_SYS_MAX
        if(sym->s_thing) {
            const t_buffer *p = (const t_buffer *)sym->s_thing;
            
            FLEXT_ASSERT(!NOGOOD(p));
            
            if(ob_sym(p) != sym_buffer) {
                post("buffer: object '%s' not valid (type %s)",GetString(sym),GetString(ob_sym(p))); 
                if(valid) ret = -2;
            }
            else {
#ifdef FLEXT_DEBUG
//              post("flext: buffer object '%s' - valid:%i samples:%i channels:%i frames:%i",GetString(sym),p->b_valid,p->b_frames,p->b_nchans,p->b_frames);
#endif
				Element *data1 = reinterpret_cast<Element *>(p->b_samples);
                if(data != data1) { data = data1; if(!ret) ret = 1; }
                if(chns != p->b_nchans) { chns = p->b_nchans; if(!ret) ret = 1; }
                if(frames != p->b_frames) { frames = p->b_frames; if(!ret) ret = 1; }
            }
        }
        else {
            FLEXT_LOG1("buffer: symbol '%s' not defined", GetString(sym)); 
            /*if(valid)*/ ret = -1;
        }
#else
#error not implemented
#endif
    }

    return ret;
}

FLEXT_TEMPIMPL(bool FLEXT_CLASSDEF(flext))::buffer::Update()
{
    FLEXT_ASSERT(sym);

    bool upd = false;

#if FLEXT_SYS == FLEXT_SYS_PD
    if(!arr) return data == NULL;

    int frames1;
    FLEXT_ARRAYTYPE *data1;
    if(!FLEXT_PD_ARRAYGRAB(arr, &frames1, &data1)) {
        data = NULL;
        chns = 0;
        frames = 0;
        upd = true;
    }
    else {
		Element *data2 = reinterpret_cast<Element *>(data1);
		if(data != data2 || frames != frames1) {
	        data = data2;
	        frames = frames1;
	        upd = true;
	    }
	}
#elif FLEXT_SYS == FLEXT_SYS_MAX
    const t_buffer *p = (const t_buffer *)sym->s_thing;
    if(p) {
        FLEXT_ASSERT(!NOGOOD(p) && ob_sym(p) == sym_buffer);
    
		Element *data1 = reinterpret_cast<Element *>(p->b_samples);
        if(data != data1 || chns != p->b_nchans || frames != p->b_frames) {
            data = data1;
            chns = p->b_nchans;
            frames = p->b_frames;
            upd = true;
        }
    }
    else {
        // buffer~ has e.g. been renamed
        data = NULL;
        chns = 0;
        frames = 0;
        upd = true;
    }
#else
#error not implemented
#endif
    return upd;
}

FLEXT_TEMPIMPL(FLEXT_TEMPSUB(FLEXT_CLASSDEF(flext))::buffer::lock_t FLEXT_CLASSDEF(flext))::buffer::Lock()
{
    FLEXT_ASSERT(sym);
#if FLEXT_SYS == FLEXT_SYS_PD
    FLEXT_ASSERT(arr);
#ifdef _FLEXT_HAVE_PD_GARRAYLOCKS
    garray_lock(arr);
#endif
    return false;
#elif FLEXT_SYS == FLEXT_SYS_MAX
    t_buffer *p = (t_buffer *)sym->s_thing;
    FLEXT_ASSERT(p);
#ifdef _FLEXT_HAVE_MAX_INUSEFLAG
    long old = p->b_inuse;
#ifdef ATOMIC_INCREMENT
    ATOMIC_INCREMENT(&p->b_inuse);
#else
    p->b_inuse = 1;
#endif
    return old;
#else
    return 0;
#endif
#else
#error not implemented
#endif
}

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::Unlock(flext::buffer::lock_t prv)
{
    FLEXT_UNUSED(prv); // keep compiler quiet
    FLEXT_ASSERT(sym);
#if FLEXT_SYS == FLEXT_SYS_PD
    FLEXT_ASSERT(arr);
#ifdef _FLEXT_HAVE_PD_GARRAYLOCKS
    garray_unlock(arr);
#endif
#elif FLEXT_SYS == FLEXT_SYS_MAX
    t_buffer *p = (t_buffer *)sym->s_thing;
    FLEXT_ASSERT(p);
#ifdef _FLEXT_HAVE_MAX_INUSEFLAG 
#ifdef ATOMIC_INCREMENT
    ATOMIC_DECREMENT(&p->b_inuse);
#else
    p->b_inuse = prv;
#endif
#endif
#else
#error not implemented
#endif
}

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::Frames(int fr,bool keep,bool zero)
{
    FLEXT_UNUSED(keep); // keep compiler quiet
    FLEXT_UNUSED(zero); // keep compiler quiet
    FLEXT_ASSERT(sym);
#if FLEXT_SYS == FLEXT_SYS_PD
    // is this function guaranteed to keep memory and set rest to zero?
    ::garray_resize(arr,(float)fr);
    Update();
#elif FLEXT_SYS == FLEXT_SYS_MAX
    Element *tmp = NULL;
    int sz = frames;  
    if(fr < sz) sz = fr;

    if(keep) {
        // copy buffer data to tmp storage
        tmp = (Element *)NewAligned(sz*sizeof(Element));
        FLEXT_ASSERT(tmp);
        CopySamples(tmp,data,sz);
    }
    
    t_atom msg;
    t_buffer *buf = (t_buffer *)sym->s_thing;
    // b_msr reflects buffer sample rate... is this what we want?
    // Max bug: adding half a sample to prevent roundoff errors....
    float ms = (fr+0.5)/buf->b_msr;
    
    SetFloat(msg,ms); 
    ::typedmess((object *)buf,(t_symbol *)sym_size,1,&msg);
    
    Update();

    if(tmp) {
        // copy data back
        CopySamples(data,tmp,sz);
        FreeAligned(tmp);
        if(zero && sz < fr) ZeroSamples(data+sz,fr-sz);
    }
    else
        if(zero) ZeroSamples(data,fr);
#else
#error
#endif
}


#if FLEXT_SYS == FLEXT_SYS_PD
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::SetRefrIntv(float intv)
{ 
    interval = intv; 
    if(interval == 0 && ticking) {
        clock_unset(tick);
        ticking = false;
    }
}
#elif FLEXT_SYS == FLEXT_SYS_MAX
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::SetRefrIntv(float) {}
#else
#error
#endif


FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::Dirty(bool force)
{
    FLEXT_ASSERT(sym);
#if FLEXT_SYS == FLEXT_SYS_PD
    if((!ticking) && (interval || force)) {
        ticking = true;
        cb_tick(this); // immediately redraw
    }
    else {
        if(force) clock_delay(tick,0);
        isdirty = true;
    }
#elif FLEXT_SYS == FLEXT_SYS_MAX
    t_buffer *p = (t_buffer *)sym->s_thing;
    FLEXT_ASSERT(p && !NOGOOD(p));
    // object_method function is redirected to object_method_imp on 64-bit Max 
    // which in turn doesn't exist. Use object_method_typed instead.
    t_atom rv;
    object_method_typed((t_object *)p,(t_symbol *)sym_dirty,0,NULL,&rv);
//    p->b_modtime = gettime();
#else
#error Not implemented
#endif 
}

#if FLEXT_SYS == FLEXT_SYS_PD
FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::cb_tick(buffer *b)
{
    if(b->arr) garray_redraw(b->arr);
#ifdef FLEXT_DEBUG
    else error("buffer: array is NULL");
#endif

    if(b->isdirty && b->interval) {
            b->isdirty = false;
            b->ticking = true;
            clock_delay(b->tick,b->interval);
    }
    else 
        b->ticking = false;
}
#endif

FLEXT_TEMPIMPL(void FLEXT_CLASSDEF(flext))::buffer::ClearDirty()
{
#if FLEXT_SYS == FLEXT_SYS_PD
    cleantime = clock_getlogicaltime();
#elif FLEXT_SYS == FLEXT_SYS_MAX
    cleantime = gettime();
#else
#error Not implemented
#endif
}

FLEXT_TEMPIMPL(bool FLEXT_CLASSDEF(flext))::buffer::IsDirty() const
{
    if(!sym) return false;
#if FLEXT_SYS == FLEXT_SYS_PD
    #ifdef _FLEXT_HAVE_PD_GARRAYUPDATETIME
    return arr && (isdirty || garray_updatetime(arr) > cleantime);
    #else
    // Don't know.... (no method in PD judging whether buffer has been changed from outside flext...)
    return true; 
    #endif
#elif FLEXT_SYS == FLEXT_SYS_MAX
    t_buffer *p = (t_buffer *)sym->s_thing;
    FLEXT_ASSERT(p && !NOGOOD(p));
    return p->b_modtime > cleantime;
#else
#error Not implemented
#endif
}

#include "flpopns.h"

#endif // __FLEXT_BUF_CPP


