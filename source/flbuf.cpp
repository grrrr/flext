/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <_cppext.h>

#ifdef PD
#define DIRTY_INTERVAL 0   // buffer dirty check in msec
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

I buffer::Set(t_symbol *s)
{
	I ret = 0;

	if(s && sym != s) {
		ret = -1;
		data = NULL; 
		frames = 0;
		chns = 0; 
	}

	if(s && *s->s_name)	sym = s;

	if(sym) {
#ifdef PD
		I frames1;
		F *data1;
    
		t_garray *a = (t_garray *)pd_findbyclass(sym, garray_class);
		if(!a)
		{
    		if (*sym->s_name)
    			error("%s: no such array '%s'",thisName(), sym->s_name);
    		sym = NULL;
			ret = -1;
		}
		else if (!garray_getfloatarray(a, &frames1, &data1))
		{
    		error("%s: bad template '%s'", thisName(),sym->s_name);
    		data = NULL;
			frames = 0;
			ret = -1;
		}
		else {
			garray_usedindsp(a);
			if(frames != frames1) { frames = frames1; ret = 1; }
			if(data != data1) { data = data1; ret = 1; }
			chns = 1;
		}
#elif defined(MAXMSP)
		if(sym->s_thing) {
			const _buffer *p = (const _buffer *)sym->s_thing;
			
			if(NOGOOD(p)) {
				post("%s: buffer object '%s' no good",thisName(),sym->s_name);
				ret = -1;
			}
			else {
#ifdef DEBUG
				post("%s: buffer object '%s' - valid:%i samples:%i channels:%i buflen:%i",thisName(),bufname->s_name,p->b_valid,p->b_frames,p->b_nchans,p->b_size);
#endif
				if(data != p->b_samples) { data = p->b_samples; ret = 1; }
				if(chns != p->b_nchans) { chns = p->b_nchans; ret = 1; }
				if(frames != p->b_size) { frames = p->b_size; ret = 1; }
			}
		}
		else {
    		error("%s: symbol '%s' not defined",thisName(), sym->s_name);
			ret = -1;
		}
#endif
	}
	else ret = -1;

	return ret;
}

V buffer::Dirty(BL force)
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
	if(b->isdirty && b->interval) {
			b->isdirty = false;
			b->ticking = true;
			clock_delay(b->tick,b->interval);
	}
	else 
		b->ticking = false;
}
#endif

