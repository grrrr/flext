// signal1~ - a flext tutorial external written by Frank Barknecht
// 
// This is a commented port of the pan~ example from the PD-Externals-Howto to
// illustrate the usage of flext. You can get the original code at
// http://iem.kug.ac.at/pd/externals-HOWTO/

#include <flsndobj.h>
 
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 401)
#error You need at least flext version 0.4.1
#endif


#define OBJS 10

class sndobj1:
	public flext_sndobj
{
	FLEXT_HEADER(sndobj1,flext_sndobj)

public:
	sndobj1();

protected:

	// these are obligatory!
	virtual void NewObjs();
	virtual void FreeObjs();
	virtual void ProcessObjs();

	// space for a few sndobjs
	SndObj *obj1,*obj2;
};

FLEXT_NEW_DSP("sndobj1~",sndobj1)


sndobj1::sndobj1()
{ 
	AddInSignal(2);  // audio ins
	AddOutSignal(2);  // audio outs

	obj1 = obj2 = NULL;
}

void sndobj1::NewObjs()
{
	// set up objects
	obj1 = new Pitch(.1f,&InObj(0),1.3f,Blocksize(),Samplerate());
	obj2 = new Pitch(.1f,&InObj(1),0.8f,Blocksize(),Samplerate());
}

void sndobj1::FreeObjs()
{
	if(obj1) delete obj1;
	if(obj2) delete obj2;
}

void sndobj1::ProcessObjs()
{
	// do processing here!!
	obj1->DoProcess();
	obj2->DoProcess();

	// output
	*obj1 >> OutObj(0);
	*obj2 >> OutObj(1);
}

