/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
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
bool flext_base::indsp = false;
const t_symbol *flext_base::curtag = NULL;

flext_base::FLEXT_CLASSDEF(flext_base)()
    : incnt(0),outcnt(0)
    , insigs(0),outsigs(0)
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
    ,outlets(NULL),inlets(NULL)
#endif
#if FLEXT_SYS == FLEXT_SYS_MAX
    ,indesc(NULL),outdesc(NULL)
#endif
{
    FLEXT_LOG1("%s - flext logging is on",thisName());

    bindhead = NULL;

    if(HasAttributes()) {
        // initialize when attribute processing is enabled
        attrhead = new ItemCont;
        attrdata = new AttrDataCont;
    }
    else {
        attrhead = NULL;
        attrdata = NULL;
    }
}

flext_base::~FLEXT_CLASSDEF(flext_base)()
{
#if FLEXT_SYS == FLEXT_SYS_PD && !defined(FLEXT_NOATTREDIT)
    // attribute editor window may still be open -> close it
    gfxstub_deleteforkey(thisHdr());
#endif

#if FLEXT_SYS == FLEXT_SYS_MAX
    // according to David Z. one should do that first...
    if(insigs) dsp_free(thisHdr());
//    if(insigs) dsp_freebox(thisHdr());
#endif

#ifdef FLEXT_THREADS
    StopThreads();
#endif

    // send remaining pending messages for this object
    QFlush(this);

    // delete message lists
    if(bindhead) delete bindhead;  // ATTENTION: the object must free all memory associated to bindings itself
    if(attrhead) delete attrhead;
    if(attrdata) delete attrdata;
    
#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
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
#endif

#if FLEXT_SYS == FLEXT_SYS_MAX
    if(indesc) {
        for(int i = 0; i < incnt; ++i) if(indesc[i]) delete[] indesc[i];
        delete[] indesc;
    }
    if(outdesc) {
        for(int i = 0; i < outcnt; ++i) if(outdesc[i]) delete[] outdesc[i];
        delete[] outdesc;
    }
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
        if(HasAttributes() && m_holdaargc && m_holdaargv) {
            // initialize creation attributes
            ok = InitAttrib(m_holdaargc,m_holdaargv);
        }
    }

    return ok;
}

/*! Set up proxy classes and basic methods at class creation time
    This ensures that they are processed before the registered flext messages
*/
void flext_base::Setup(t_classid id)
{
    t_class *c = getClass(id);

#if FLEXT_SYS == FLEXT_SYS_PD || FLEXT_SYS == FLEXT_SYS_MAX
    add_loadbang(c,cb_loadbang);
#if FLEXT_SYS == FLEXT_SYS_MAX
    add_assist(c,cb_assist);
    add_dblclick(c,cb_click);
#endif
#else
    #pragma message ("no implementation of loadbang or assist") 
#endif

    if(process_attributes) {
        AddMethod(id,0,"getattributes",cb_ListAttrib);
        AddMethod(id,0,"getmethods",cb_ListMethods);

#if FLEXT_SYS == FLEXT_SYS_PD && !defined(FLEXT_NOATTREDIT)
        AddMethod(id,0,"attributedialog",cb_AttrDialog);
#endif
    }

#if FLEXT_SYS == FLEXT_SYS_PD
    SetGfx(id);
#endif

    SetProxies(c);

    StartQueue();
}

#if FLEXT_SYS != FLEXT_SYS_JMAX
void flext_base::cb_loadbang(t_class *c) { thisObject(c)->CbLoadbang(); }   
#endif

void flext_base::m_loadbang() {}
void flext_base::CbLoadbang() { m_loadbang(); }

void flext_base::CbClick() {}

#if FLEXT_SYS == FLEXT_SYS_PD
int flext_base::cb_click(t_gobj *c, struct _glist *glist,int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    if(doit && alt) {
        thisObject(c)->CbClick();
        return 1;
    }
    else
        return 0;
}
#endif

#if FLEXT_SYS == FLEXT_SYS_MAX
void flext_base::cb_click(t_class *c, Point pt, short mods)
{
    thisObject(c)->CbClick();
}

void flext_base::cb_assist(t_class *c,void * /*b*/,long msg,long arg,char *s) 
{ 
    flext_base *th = thisObject(c); 

    switch(msg) {
    case 1: //ASSIST_INLET:
        if(arg < th->incnt && th->indesc[arg]) strcpy(s,th->indesc[arg]);
        break;
    case 2: //ASSIST_OUTLET:
        if(arg < th->outcnt) {
            if(th->outdesc[arg]) strcpy(s,th->outdesc[arg]);
        }
        else
            if(arg == th->outcnt && th->HasAttributes()) strcpy(s,"Attributes");
        break;
    }
}
#endif
