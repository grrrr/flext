/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flext.cpp
    \brief Implementation of the flext base class.
*/
 
#include "flext.h"
#include "flinternal.h"
#include <string.h>

// === flext_base ============================================

bool flext_base::compatibility = true;

flext_base::flext_base():
	inlist(NULL),outlist(NULL),
	curtag(NULL),
	incnt(0),outcnt(0),
	insigs(0),outsigs(0),
	outlets(NULL),outattr(NULL),
	distmsgs(false),
	methhead(new itemarr),attrhead(new itemarr), //attrcnt(0),
	clmethhead(ClMeths(thisClass())),clattrhead(ClAttrs(thisClass())), 
	inlets(NULL)
#if FLEXT_SYS == FLEXT_SYS_MAX
	,indesc(NULL),outdesc(NULL)
#endif
{
	LOG1("%s - flext logging is on",thisName());

	// message queue ticker
	qhead = qtail = NULL;
	qclk = (t_qelem *)(qelem_new(this,(t_method)QTick));
}

flext_base::~flext_base()
{
#ifdef FLEXT_THREADS
	StopThreads();
#endif

	// send remaining pending messages and destroy queue ticker
	while(qhead) QTick(this);
	qelem_free((t_qelem *)qclk);

	// delete message lists
	if(methhead) delete methhead;
	if(attrhead) delete attrhead;
	
	// destroy inlets and outlets and their proxy objects
	if(inlist) delete inlist;
	if(outlist) delete outlist;
	if(outlets) delete[] outlets;

	if(inlets) {
		for(int ix = 0; ix < incnt; ++ix)
			if(inlets[ix]) {
				// release proxy object
#if FLEXT_SYS == FLEXT_SYS_PD
				pd_free(&inlets[ix]->obj.ob_pd);
#elif FLEXT_SYS == FLEXT_SYS_MAX
				freeobject((object *)inlets[ix]);
#endif
			}
		delete[] inlets;
	}

#if FLEXT_SYS == FLEXT_SYS_MAX
	if(indesc) {
		for(int i = 0; i < incnt; ++i) if(indesc[i]) delete[] indesc[i];
		delete[] indesc;
	}
	if(outdesc) {
		for(int i = 0; i < outcnt; ++i) if(outdesc[i]) delete[] outdesc[i];
		delete[] outdesc;
	}

//	if(insigs) dsp_free(thisHdr());
	if(insigs) dsp_freebox(thisHdr());
#endif
}

/*! This virtual function is called after the object has been created, that is, 
	after the constructor has been processed. 
	It creates the inlets and outlets and the message and attribute lists.
	\note You can override it in your own class, but be sure to call it, 
	\note otherwise no inlets/outlets will be created
	\note All inlet, outlets, method and attribute declarations must be made before a call to Init!
	\remark Creation of inlets/outlets can't be done upon declaration, as Max/MSP needs creation
	\remark in reverse.
*/
bool flext_base::Init()
{
	bool ok = flext_obj::Init();

	if(ok) ok = InitInlets() && InitOutlets();

	if(ok) {
		// initialize method lists
		if(methhead) methhead->Finalize();
		if(clmethhead) clmethhead->Finalize();
		
		if(procattr) {
			// initialize attribute lists
			if(attrhead) attrhead->Finalize();
			if(clattrhead) clattrhead->Finalize();

			// initialize creation attributes
			if(m_holdaargc && m_holdaargv)
				ok = InitAttrib(m_holdaargc,m_holdaargv);
		}
	}

	return ok;
}

/*! Set up proxy classes and basic methods at class creation time
	This ensures that they are processed before the registered flext messages
*/
void flext_base::Setup(t_class *c)
{
	add_method(c,cb_help,"help");
	add_loadbang(c,cb_loadbang);
#if FLEXT_SYS == FLEXT_SYS_MAX
	add_assist(c,cb_assist);
#endif

	if(process_attributes) 
		AddMethod(c,0,"getattributes",(methfun)cb_ListAttrib);

	SetProxies(c);

#ifdef FLEXT_THREADS
	thrid = GetThreadId();
	StartHelper();
#endif
}

void flext_base::cb_help(t_class *c) { thisObject(c)->m_help(); }	

void flext_base::cb_loadbang(t_class *c) { thisObject(c)->m_loadbang(); }	

void flext_base::m_help()
{
	// This should better be overloaded
	post("%s (using flext " FLEXT_VERSTR ") - compiled on %s %s",thisName(),__DATE__,__TIME__);
}

void flext_base::m_loadbang() {}

#if FLEXT_SYS == FLEXT_SYS_MAX
void flext_base::cb_assist(t_class *c,void * /*b*/,long msg,long arg,char *s) { thisObject(c)->m_assist(msg,arg,s); }
#endif

void flext_base::m_assist(long msg,long arg,char *s) 
{
#if FLEXT_SYS == FLEXT_SYS_MAX
	switch(msg) {
	case 1: //ASSIST_INLET:
		strcpy(s,arg < incnt && indesc[arg]?indesc[arg]:""); 
		break;
	case 2: //ASSIST_OUTLET:
		strcpy(s,arg < outcnt && outdesc[arg]?outdesc[arg]:""); 
		break;
	}
#endif
}
