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
	class Input:
		public Stk
	{
	public:
		Input(int chns,const t_sample **b,int vecsz,float sr);

		int Chns() const { return chns; }
		void SetBufs(int c,const t_sample **b) { chns = c,buf = b; }

		t_sample tick();
		t_sample *tick(t_sample *vector,unsigned int vectorSize);
		const t_sample *tickFrame();
		t_sample *tickFrame(t_sample *frameVector,unsigned int frames);

	private:
		int chns;
		const t_sample **buf;
	};

	//! STK object for writing to outlet buffer
	class Output:
		public Stk
	{
	public:
		Output(int chns,t_sample **b,int vecsz,float sr);

		int Chns() const { return chns; }
		void SetBufs(int c,t_sample **b) { chns = c,buf = b; }

		void tick(const t_sample sample);
		void tick(const t_sample *vector,unsigned int vectorSize);
		void tickFrame(const t_sample *frameVector,unsigned int frames = 1);

	private:
		int chns;
		t_sample **buf;
	};

	void ClearObjs();

	Input *inobj;
	Output *outobj;

	int blsz;
};

#endif
