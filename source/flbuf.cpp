/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <flext.h>

#ifdef PD
#define DIRTY_INTERVAL 0   // buffer dirty check in msec
#endif

flext_base::buffer::buffer(t_symbol *bn,BL delayed):
	sym(NULL),data(NULL),
	chns(0),frames(0)
{
#ifdef PD
	interval = DIRTY_INTERVAL;
	isdirty = false;
	ticking = false;
    tick = clock_new(this,(t_method)cb_tick);
#endif

	if(bn) Set(bn,delayed);
}

flext_base::buffer::~buffer()
{
#ifdef PD
    clock_free(tick);
#endif
}

I flext_base::buffer::Set(t_symbol *s,BL nameonly)
{
	I ret = 0;

	if(s && sym != s) {
		ret = -1;
		data = NULL; 
		frames = 0;
		chns = 0; 
	}

	if(s && *s->s_name)	sym = s;

	if(sym && !nameonly) {
#ifdef PD
		I frames1;
		F *data1;
    
		t_garray *a = (t_garray *)pd_findbyclass(sym, garray_class);
		if(!a)
		{
    		if (*sym->s_name)
    			error("buffer: no such array '%s'",sym->s_name);
    		sym = NULL;
			ret = -1;
		}
		else if (!garray_getfloatarray(a, &frames1, &data1))
		{
    		error("buffer: bad template '%s'", sym->s_name);
    		data = NULL;
			frames = 0;
			ret = -1;
		}
		else {
			garray_usedindsp(a);
			if(frames != frames1) { frames = frames1; if(!ret) ret = 1; }
			if(data != data1) { data = data1; if(!ret) ret = 1; }
			chns = 1;
		}
#elif defined(MAXMSP)
		if(sym->s_thing) {
			const _buffer *p = (const _buffer *)sym->s_thing;
			
			if(NOGOOD(p)) {
				post("buffer: buffer object '%s' no good",sym->s_name);
				ret = -1;
			}
			else {
#ifdef DEBUG
				post("%s: buffer object '%s' - valid:%i samples:%i channels:%i frames:%i",thisName(),bufname->s_name,p->b_valid,p->b_frames,p->b_nchans,p->b_frames);
#endif
				if(data != p->b_samples) { data = p->b_samples; if(!ret) ret = 1; }
				if(chns != p->b_nchans) { chns = p->b_nchans; if(!ret) ret = 1; }
				if(frames != p->b_frames) { frames = p->b_frames; if(!ret) ret = 1; }
			}
		}
		else {
    		error("buffer: symbol '%s' not defined", sym->s_name);
			ret = -1;
		}
#endif
	}
	else ret = -1;

	return ret;
}

V flext_base::buffer::Dirty(BL force)
{
	if(sym) {
#ifdef PD
		if((!ticking) && (interval || force)) {
			ticking = true;
			clock_delay(tick,0);
		}
		else
			isdirty = true;
#elif defined(MAXMSP)
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
#endif 
	}
}

#ifdef PD
V flext_base::buffer::cb_tick(buffer *b)
{
	t_garray *a = (t_garray *)pd_findbyclass(b->sym, garray_class);
	if (a) garray_redraw(a);
//		else bug("tabwrite_tilde_tick");
	if(b->isdirty && b->interval) {
			b->isdirty = false;
			b->ticking = true;
			clock_delay(b->tick,b->interval);
	}
	else 
		b->ticking = false;
}
#endif

