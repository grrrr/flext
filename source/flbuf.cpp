/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <_cppext.h>

#ifdef PD
#define DIRTY_INTERVAL 100   // buffer dirty check in msec
#endif

buffer::buffer(t_symbol *bn):
	sym(NULL),data(NULL),
	chns(0),frames(0)
{
#ifdef PD
	interval = DIRTY_INTERVAL;
	isdirty = false;
	ticking = false;
    tick = clock_new(this,(t_method)cb_tick);
#endif

	if(bn) Set(bn);
}

buffer::~buffer()
{
#ifdef PD
    clock_free(tick);
#endif
}

BL buffer::Set(t_symbol *s)
{
	if(s && *s->s_name)	sym = s;
	
	data = NULL; 
	frames = 0;
	chns = 0; 

	if(sym) {
#ifdef PD
		t_garray *a;
    
		if (!(a = (t_garray *)pd_findbyclass(sym, garray_class)))
		{
    		if (*sym->s_name)
    			error("%s: no such array '%s'",thisName(), sym->s_name);
    		sym = 0;
		}
		else if (!garray_getfloatarray(a, &frames, &data))
		{
    		error("%s: bad template '%s'", thisName(),sym->s_name);
    		data = NULL;
			frames = 0;
		}
		else {
			garray_usedindsp(a);
			chns = 1;
		}
#elif defined(MAXMSP)
		if(sym->s_thing) {
			const _buffer *p = (const _buffer *)sym->s_thing;
			
			if(NOGOOD(p)) {
				post("%s: buffer object '%s' no good",thisName(),sym->s_name);
			}
			else {
#ifdef DEBUG
				post("%s: buffer object '%s' - ???:%i samples:%i channels:%i buflen:%i",thisName(),bufname->s_name,p->b_valid,p->b_frames,p->b_nchans,p->b_size);
#endif
				data = p->b_samples;
				chns = p->b_nchans;
				frames = p->b_size;
			}
		}
		else {
    		error("%s: symbol '%s' not defined",thisName(), sym->s_name);
		}
#endif
	}

	return false;
}

V buffer::Dirty()
{
	if(sym) {
#ifdef PD
		if(!ticking) {
			ticking = true;
			clock_delay(tick,0);
		}
		else
			isdirty = true;
#elif defined(MAXMSP)
		if(sym->s_thing) {
			_buffer *p = (_buffer *)sym->s_thing;
			
			if(NOGOOD(p)) {
				post("%s: buffer object '%s' no good",thisName(),sym->s_name);
			}
			else {
				p->b_modtime = gettime();
			}
		}
		else {
    		error("%s: symbol '%s' not defined",thisName(),sym->s_name);
		}
#endif 
	}
}

#ifdef PD
V buffer::cb_tick(buffer *b)
{
	t_garray *a = (t_garray *)pd_findbyclass(b->sym, garray_class);
	if (a) garray_redraw(a);
//		else bug("tabwrite_tilde_tick");
	if(b->isdirty) {
		b->ticking = true;
		b->isdirty = false;
		clock_delay(b->tick,b->interval);
	}
	else 
		b->ticking = false;
}
#endif

