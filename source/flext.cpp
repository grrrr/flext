/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#include <_cppext.h>

V ext_obj::cb_setup(t_class *c)
{
	add_method0(c,cb_help,"help");
	
#ifdef MAXMSP
	add_loadbang(c,cb_loadbang);
	add_assist(c,cb_assist);
#endif
}

V ext_obj::cb_help(V *c) { thisClass(c)->m_help(); }	

#ifdef MAXMSP
V ext_obj::cb_loadbang(V *c) { thisClass(c)->m_loadbang(); }	
V ext_obj::cb_assist(V *c,V *b,L msg,L arg,C *s) { thisClass(c)->m_assist(msg,arg,s); }
#endif

V ext_obj::m_help()
{
	post("Loaded object '%s' - compiled on %s %s",thisName(),__DATE__,__TIME__);
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

