#include <_cppext.h>

V ext_obj::cb_setup(t_class *c)
{
	add_method0(c,cb_help,"help");
	
#ifdef MAX
	add_loadbang(c,cb_loadbang);
	add_assist(c,cb_assist);
#endif
}

V ext_obj::cb_help(V *c) { thisClass(c)->m_help(); }	

#ifdef MAX
V ext_obj::cb_loadbang(V *c) { thisClass(c)->m_loadbang(); }	
V ext_obj::cb_assist(V *c,V *b,L msg,L arg,C *s) { thisClass(c)->m_assist(msg,arg,s); }
#endif

V ext_obj::m_help()
{
	post("Loaded object '%s' - compiled on %s %s",thisName(),__DATE__,__TIME__);
}

// -----------------------------

V buf_obj::cb_setup(t_class *c)
{
}


buf_obj::buf_obj()
{
	bufname = NULL;
	buf = NULL; 
	bufchns = 0;
	buflen = 0; 
}

V buf_obj::setbuf(t_symbol *s)
{
	if(s && *s->s_name)	bufname = s;
	
	buf = NULL; 
	bufchns = 0;
	buflen = 0; 

	if(bufname) {
#ifdef PD 
		t_garray *a;
    
		if (!(a = (t_garray *)pd_findbyclass(bufname, garray_class)))
		{
    		if (*bufname->s_name)
    			error("%s: no such array '%s'",thisName(), bufname->s_name);
    		bufname = 0;
		}
		else if (!garray_getfloatarray(a, &buflen, &buf))
		{
    		error("%s: bad template '%s'", thisName(),bufname->s_name);
    		buf = 0;
			buflen = 0;
		}
		else {
			garray_usedindsp(a);
			bufchns = 1;
		}
#elif defined(MAX)
		if(bufname->s_thing) {
			const _buffer *p = (const _buffer *)bufname->s_thing;
			
			if(NOGOOD(p)) {
				post("%s: buffer object '%s' no good",thisName(),bufname->s_name);
			}
			else {
#ifdef DEBUG
				post("%s: buffer object '%s' - ???:%i samples:%i channels:%i buflen:%i",thisName(),bufname->s_name,p->b_valid,p->b_frames,p->b_nchans,p->b_size);
#endif
				buf = p->b_samples;
				bufchns = p->b_nchans;
				buflen = p->b_size;
			}
		}
		else {
    		error("%s: symbol '%s' not defined",thisName(), bufname->s_name);
		}
#endif
	}
}

V buf_obj::setdirty()
{
	if(bufname) {
#ifdef PD
		t_garray *a;
    
		if (!(a = (t_garray *)pd_findbyclass(bufname, garray_class))) {
    		if (*bufname->s_name)
    			error("%s: no such array '%s'",thisName(),bufname->s_name);
		}
		else if (!garray_getfloatarray(a, &buflen, &buf)) {
    		error("%s: bad template '%s'",thisName(),bufname->s_name);
		}
		else garray_redraw(a);
#elif defined(MAX)
		if(bufname->s_thing) {
			_buffer *p = (_buffer *)bufname->s_thing;
			
			if(NOGOOD(p)) {
				post("%s: buffer object '%s' no good",thisName(),bufname->s_name);
			}
			else {
				p->b_modtime = gettime();
			}
		}
		else {
    		error("%s: symbol '%s' not defined",thisName(),bufname->s_name);
		}
#endif 
	}
}



// ----------------------------

V dsp_obj::cb_setup(t_class *c)
{
	enable_signal(c);
	
	add_dsp(c,cb_dsp);
	add_method1(c,cb_enable,"enable",A_FLINT);
}

dsp_obj::dsp_obj(): enable(true) {}

V dsp_obj::cb_dsp(V *c,t_signal **s) { thisClass(c)->m_dsp(s); }
V dsp_obj::cb_enable(V *c,FI on) { thisClass(c)->m_enable(on != 0); }

V dsp_obj::m_enable(BL en) { enable = en; }

// ----------------------------

V bufdsp_obj::cb_setup(t_class *c)
{
	enable_signal(c);
	
	add_dsp(c,cb_dsp);
	add_method1(c,cb_enable,"enable",A_FLINT);
}

bufdsp_obj::bufdsp_obj(): enable(true) {}

V bufdsp_obj::cb_dsp(V *c,t_signal **s) { thisClass(c)->m_dsp(s); }
V bufdsp_obj::cb_enable(V *c,FI on) { thisClass(c)->m_enable(on != 0); }

V bufdsp_obj::m_enable(BL en) { enable = en; }





