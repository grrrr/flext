/* 

Copyright (c) 2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __FLSTK_H
#define __FLSTK_H

#include "flext.h"

#include <stk.h>

class FLEXT_SHARE flext_stk:
	public flext_dsp
{
	FLEXT_HEADER(flext_stk,flext_dsp)
 
public:
	flext_stk();

	// these have to be overridden in child classes
	virtual void NewObjs() {}
	virtual void FreeObjs() {}
	virtual void ProcessObjs() {}

protected:
	virtual bool Init();
	virtual void Exit();

	virtual void m_dsp(int n,t_sample *const *in,t_sample *const *out); 
	virtual void m_signal(int n,t_sample *const *in,t_sample *const *out); 

private:
	//! STK object for reading from inlet buffer
	class Inlet:
		public Stk
	{
	public:
		Inlet(const t_sample *b,int vecsz);

		const t_sample *lastFrame() const { return buf+index; }
		t_sample lastOut() const { return buf[index]; }

		t_sample tick();
		t_sample *tick(t_sample *vector,unsigned int vectorSize);

		void SetBuf(const t_sample *b) { buf = b; }

	private:
		const t_sample *buf;
		int vecsz,index;
	};

	//! STK object for writing to outlet buffer
	class Outlet:
		public Stk
	{
	public:
		Outlet(t_sample *b,int vecsz);

		void tick(t_sample sample);
		void tick(const t_sample *vector,unsigned int vectorSize);

		void SetBuf(t_sample *b) { buf = b; }

	private:
		t_sample *buf;
		int vecsz,index;
	};

	void ClearObjs();

	int inobjs,outobjs;
	Inlet **inobj;
	Outlet **outobj;

	float smprt;
	int blsz;
};

#endif
