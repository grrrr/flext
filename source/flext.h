/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __CPPEXT_H
#define __CPPEXT_H

#include <_cpp.h>


// ----------------------------

class ext_obj:
	public CPPExtern
{
	CPPEXTERN_HEADER(ext_obj,CPPExtern)
	
public:
//	ext_obj();
//	virtual ~ext_obj();
	
	virtual V m_help();
	
#ifdef MAXMSP
	virtual V m_loadbang() {}
	virtual V m_assist(L msg,L arg,C *s) = 0;
#endif	

protected:

private:
	static V cb_help(V *c);

#ifdef MAXMSP
	static V cb_loadbang(V *c);
	static V cb_assist(V *c,V *b,L msg,L arg,C *s);
#endif
};


// ----------------------------

class buffer
{
public:
	buffer(t_symbol *s = NULL);
	~buffer();
	
	BL Set(t_symbol *s = NULL);
	V Dirty();
	
	t_symbol *Symbol() { return sym; }
	const C *Name() const { return sym?sym->s_name:""; }
	F *Data() { return data; }
	I Channels() const { return chns; }
	I Frames() const { return frames; }

protected:
	const C *thisName() const { return typeid(*this).name(); }

	t_symbol *sym;
	F *data;
	I chns,frames;
#ifdef PD
	F interval;
	BL isdirty,ticking;
	t_clock *tick;
#endif

private:
	static V cb_tick(buffer *b);
};


// ----------------------------

class dsp_obj:
	public ext_obj
{
	CPPEXTERN_HEADER(dsp_obj,ext_obj)
	
public:
	dsp_obj();
	
	virtual V m_dsp(t_signal **s) = 0;
	virtual V m_enable(BL on);
	
protected:
	BL enable;

private:

	static V cb_dsp(V *c,t_signal **s);
	static V cb_enable(V *c,FI on);
};


#endif
