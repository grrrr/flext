/* 

max-pd - compatibility library for Max/MSP and pd (pure data)

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
	
#ifdef MAX
	virtual V m_loadbang() {}
	virtual V m_assist(L msg,L arg,C *s) = 0;
#endif	

protected:

private:
	static V cb_help(V *c);

#ifdef MAX
	static V cb_loadbang(V *c);
	static V cb_assist(V *c,V *b,L msg,L arg,C *s);
#endif
};


// ----------------------------

class buf_obj:
	public ext_obj
{
	CPPEXTERN_HEADER(buf_obj,ext_obj)
	
public:
	buf_obj();
	
protected:
	virtual V setbuf(t_symbol *s = NULL);
	virtual V setdirty();

	t_symbol *bufname;
	F *buf;
	I bufchns,buflen;
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


// ----------------------------

class bufdsp_obj:
	public buf_obj
{
	CPPEXTERN_HEADER(bufdsp_obj,buf_obj)
	
public:
	bufdsp_obj();
	
	virtual V m_dsp(t_signal **s) = 0;
	virtual V m_enable(BL on);
	
protected:
	BL enable;

private:

	static V cb_dsp(V *c,t_signal **s);
	static V cb_enable(V *c,FI on);
};


#endif

