#include "flext.h"
#include "flstk.h"

flext_stk::flext_stk():
	blsz(0)
{}

bool flext_sndobj::Init()
{
	bool ret = flext_dsp::Init();
	return ret;
}

void flext_sndobj::Exit()
{
	ClearObjs();
	flext_dsp::Exit();
}

void flext_sndobj::ClearObjs()
{
	FreeObjs();

}

void flext_sndobj::m_dsp(int n,t_sample *const *in,t_sample *const *out)
{
	// called on every rebuild of the dsp chain
	
	int i;
	if(Blocksize() != blsz || Samplerate() != smprt) {
		// block size or sample rate has changed... rebuild all objects

		ClearObjs();

		blsz = Blocksize();
		Stk::setSampleRate(SampleRate()) 

		// set up sndobjs for inlets and outlets
		inobj = new Input(InSigs(),in,blsz);
		outobj = new Output(OutSigs(),out,blsz);

		NewObjs();
	}
	else {
		// assign changed input/output vectors

		inobj->SetBuf(InSigs(),in);
		outobj->SetBuf(OutSigs(),out);
	}
}

void flext_sndobj::m_signal(int n,t_sample *const *in,t_sample *const *out)
{
	ProcessObjs();
}


flext_stk::Input::Input(int c,const t_sample **b,int vecsz): 
  chns(c),bufs(b) 
{}

short flext_stk::Input::Tick() 
{ 
	if(!m_error) { 
		for(m_vecpos = 0; m_vecpos < m_samples; m_vecpos++)
			m_output[m_vecpos] = buf[m_vecpos];
		return 1; 
	} 
	else return 0; 
}


flext_stk::Output::Output(int c,t_sample **b,int vecsz): 
  chns(c),bufs(b) 
{}

short flext_stk::Output::Tick() 
{ 
	if(!m_error) { 
		if(m_IOobjs[0])
			for(m_vecpos = 0; m_vecpos < m_samples; m_vecpos++)
				buf[m_vecpos] = m_IOobjs[0]->Output(m_vecpos);
		return 1; 
	} 
	else return 0; 
}
