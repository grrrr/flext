/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flbuf.cpp
    \brief Implementation of the buffer abstraction class.
*/
 
#include "flext.h"

#if FLEXT_SYS != FLEXT_SYS_JMAX

#if FLEXT_SYS == FLEXT_SYS_MAX
#include "flmspbuffer.h" // include inofficial buffer.h
#endif

#if FLEXT_SYS == FLEXT_SYS_PD
#define DIRTY_INTERVAL 0   // buffer dirty check in msec
#endif



flext::buffer::buffer(const t_symbol *bn,bool delayed):
	sym(NULL),data(NULL),
	chns(0),frames(0)
{
#if FLEXT_SYS == FLEXT_SYS_PD
	arr = NULL;
	interval = DIRTY_INTERVAL;
	isdirty = false;
	ticking = false;
    tick = clock_new(this,(t_method)cb_tick);
#endif

	if(bn) Set(bn,delayed);
}

flext::buffer::~buffer()
{
#if FLEXT_SYS == FLEXT_SYS_PD
    clock_free(tick);
#endif
}

int flext::buffer::Set(const t_symbol *s,bool nameonly)
{
	int ret = 0;
	bool valid = data != NULL; // valid now? (before change)

	if(s && sym != s) {
		ret = -1;
		data = NULL; 
		frames = 0;
		chns = 0; 
	}

	if(s && *GetString(s))	sym = s;

	if(!sym) {
		if(valid) ret = -1;
	}	
	else if(!nameonly) {
#if FLEXT_SYS == FLEXT_SYS_PD
		int frames1;
		t_sample *data1;
    
		arr = (t_garray *)pd_findbyclass(const_cast<t_symbol *>(sym), garray_class);
		if(!arr)
		{
    		if (*GetString(sym)) error("buffer: no such array '%s'",GetString(sym));
    		sym = NULL;
			if(valid) ret = -1;
		}
		else if(!garray_getfloatarray(arr, &frames1, &data1))
		{
    		error("buffer: bad template '%s'",GetString(sym)); 
    		data = NULL;
			frames = 0;
			if(valid) ret = -1;
		}
		else {
			garray_usedindsp(arr);
			if(frames != frames1) { frames = frames1; if(!ret) ret = 1; }
			if(data != data1) { data = data1; if(!ret) ret = 1; }
			chns = 1;
		}
#elif FLEXT_SYS == FLEXT_SYS_MAX
		if(sym->s_thing) {
			const _buffer *p = (const _buffer *)sym->s_thing;
			
			if(NOGOOD(p)) {
				post("buffer: buffer object '%s' no good",GetString(sym)); 
				if(valid) ret = -1;
			}
			else {
#ifdef FLEXT_DEBUG
				post("flext: buffer object '%s' - valid:%i samples:%i channels:%i frames:%i",GetString(sym),p->b_valid,p->b_frames,p->b_nchans,p->b_frames);
#endif
				if(data != p->b_samples) { data = p->b_samples; if(!ret) ret = 1; }
				if(chns != p->b_nchans) { chns = p->b_nchans; if(!ret) ret = 1; }
				if(frames != p->b_frames) { frames = p->b_frames; if(!ret) ret = 1; }
			}
		}
		else {
    		error("buffer: symbol '%s' not defined", GetString(sym)); 
    		if(valid) ret = -1;
		}
#else
#error not implemented
#endif
	}

	return ret;
}

bool flext::buffer::Valid() const
{
	if(sym) {
#if FLEXT_SYS == FLEXT_SYS_PD
		int frames1;
		t_sample *data1;
		return garray_getfloatarray(arr, &frames1, &data1) != 0;
#elif FLEXT_SYS == FLEXT_SYS_MAX
		const _buffer *p = (const _buffer *)sym->s_thing;
		return p && p->b_valid;
#else
#error
#endif 
	}
	else return false;
}


bool flext::buffer::Update()
{
	if(!Ok()) return false;

#if FLEXT_SYS == FLEXT_SYS_PD
	int frames1;
	t_sample *data1;
	if(!garray_getfloatarray(arr, &frames1, &data1)) {
		frames = 0;
		data = NULL;
		chns = 0;
		return true;
	}
	else if(data != data1 || frames != frames1) {
		frames = frames1;
		data = data1;
		return true;
	}
	else
		return false;
#elif FLEXT_SYS == FLEXT_SYS_MAX
	if(!sym->s_thing) 
		return false;
	else {
		const _buffer *p = (const _buffer *)sym->s_thing;
		if(data != p->b_samples || chns != p->b_nchans || frames != p->b_frames) {
			data = p->b_samples;
			chns = p->b_nchans;
			frames = p->b_frames;
			return true;
		}
		else
			return false;
	}
#else
#error not implemented
#endif
}

void flext::buffer::Frames(int fr,bool keep)
{
#if FLEXT_SYS == FLEXT_SYS_PD
	::garray_resize(arr,(float)fr);
	Update();
#elif FLEXT_SYS == FLEXT_SYS_MAX
	t_sample *tmp = NULL;
	int sz = frames;
	if(fr < sz) sz = fr;

	if(keep) {
		// copy buffer data to tmp storage
		tmp = new t_sample[sz];
		if(tmp)
			CopySamples(data,tmp,sz);
		else
			error("flext::buffer - not enough memory for keeping buffer~ contents");
	}
	
	t_atom msg;
	_buffer *buf = (_buffer *)sym->s_thing;
	// b_msr reflects buffer sample rate... is this what we want?
	// Max bug: adding small value 0.001 to get right sample count
	float ms = fr/buf->b_msr+0.001;
	
	SetFloat(msg,ms); 
	::typedmess((object *)buf,gensym("size"),1,&msg);
	
	Update();

	if(tmp) {
		// copy data back
		CopySamples(tmp,data,sz);
		delete[] tmp;
	}
#else
#error
#endif
}


#if FLEXT_SYS == FLEXT_SYS_PD
void flext::buffer::SetRefrIntv(float intv) 
{ 
	interval = intv; 
	if(interval == 0 && ticking) {
		clock_unset(tick);
		ticking = false;
	}
}
#elif FLEXT_SYS == FLEXT_SYS_MAX
void flext::buffer::SetRefrIntv(float) {}
#else
#error
#endif


void flext::buffer::Dirty(bool force)
{
	if(sym) {
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
		if(sym->s_thing) {
			_buffer *p = (_buffer *)sym->s_thing;
			
			if(NOGOOD(p)) {
				post("buffer: buffer object '%s' no good",sym->s_name);
			}
			else {
				p->b_modtime = gettime();
			}
		}
		else {
    		error("buffer: symbol '%s' not defined",sym->s_name);
		}
#else
#error
#endif 
	}
}

#if FLEXT_SYS == FLEXT_SYS_PD
void flext::buffer::cb_tick(buffer *b)
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


#endif // Jmax
