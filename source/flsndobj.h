#ifndef FLEXT_SNDOBJ
#define FLEXT_SNDOBJ

#include <flext.h>

#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 300)
#error You need at least flext version 0.3.0 
#endif

#include <SndObj/AudioDefs.h>

class flext_sndobj:
	public flext_dsp
{
	FLEXT_HEADER(flext_sndobj,flext_dsp)
 
public:
	flext_sndobj();
	virtual ~flext_sndobj();

	virtual void NewObjs() {}
	virtual void FreeObjs() {}
	virtual void ProcessObjs() {}

	SndObj &InObj(int i) { return *tmpobj[i]; }
	SndIO &OutObj(int i) { return *outobj[i]; }

protected:
	virtual void m_dsp(int n,t_sample *const *in,t_sample *const *out); 
	virtual void m_signal(int n,t_sample *const *in,t_sample *const *out); 

private:
	//! SndObj for reading from inlet buffer
	class Inlet:
		public SndIO
	{
	public:
		Inlet(const t_sample *b,int vecsz,float sr);
		virtual short Read();
		virtual short Write();

	private:
		const t_sample *buf;
	};

	//! SndObj for writing to outlet buffer
	class Outlet:
		public SndIO
	{
	public:
		Outlet(t_sample *b,int vecsz,float sr);
		virtual short Read();
		virtual short Write();

	private:
		t_sample *buf;
	};

	void ClearObjs();

	int inobjs,outobjs;
	SndObj **tmpobj;
	Inlet **inobj;
	Outlet **outobj;
};

#endif
