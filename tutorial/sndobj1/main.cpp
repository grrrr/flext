// signal1~ - a flext tutorial external written by Frank Barknecht
// 
// This is a commented port of the pan~ example from the PD-Externals-Howto to
// illustrate the usage of flext. You can get the original code at
// http://iem.kug.ac.at/pd/externals-HOWTO/

#define FLEXT_ATTRIBUTES 1

#include <flsndobj.h>
 
#if !defined(FLEXT_VERSION) || (FLEXT_VERSION < 401)
#error You need at least flext version 0.4.1
#endif


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
	Pitch *obj1,*obj2;

	float sh1,sh2;

	FLEXT_ATTRVAR_F(sh1)
	FLEXT_ATTRVAR_F(sh2)
};

FLEXT_NEW_DSP("sndobj1~",sndobj1)


sndobj1::sndobj1():
	sh1(1),sh2(1)
{ 
	AddInSignal(2);  // audio ins
	AddOutSignal(2);  // audio outs

	obj1 = obj2 = NULL;

	FLEXT_ADDATTR_VAR1("shL",sh1);
	FLEXT_ADDATTR_VAR1("shR",sh2);
}

// construct needed SndObjs
void sndobj1::NewObjs()
{
	// set up objects
	obj1 = new Pitch(.1f,&InObj(0),sh1,Blocksize(),Samplerate());
	obj2 = new Pitch(.1f,&InObj(1),sh2,Blocksize(),Samplerate());
}

// destroy the SndObjs
void sndobj1::FreeObjs()
{
	if(obj1) delete obj1;
	if(obj2) delete obj2;
}

// this is called on every DSP block
void sndobj1::ProcessObjs()
{
	// set current pitch shift
	obj1->SetPitch(sh1);
	obj2->SetPitch(sh2);

	// do processing here!!
	obj1->DoProcess();
	obj2->DoProcess();

	// output
	*obj1 >> OutObj(0);
	*obj2 >> OutObj(1);
}

