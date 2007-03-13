/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __FLSNDOBJ_H
#define __FLSNDOBJ_H

#include "flext.h"

// PI is defined in the Max/MSP SDK, but clashes with SndObj.h
#ifdef PI
#undef PI
#endif

#ifndef FLEXT_THREADS
#define NOPTHREAD
#endif
#include <SndObj.h>
#include <SndIO.h>
#undef NOPTHREAD


class FLEXT_SHARE flext_sndobj:
    public flext_dsp
{
    FLEXT_HEADER(flext_sndobj,flext_dsp)
 
public:
    flext_sndobj();

    // these have to be overridden in child classes
    virtual bool NewObjs() { return true; }
    virtual void FreeObjs() {}
    virtual void ProcessObjs() {}

    // inputs and outputs
    SndObj &InObj(int i) { return *tmpobj[i]; }
    SndIO &OutObj(int i) { return *outobj[i]; }

protected:
    virtual bool Init();
    virtual void Exit();

private:
    //! SndObj for reading from inlet buffer
    class Inlet:
        public SndIO
    {
    public:
        Inlet(const t_sample *b,int vecsz,float sr);
        virtual short Read();
        virtual short Write();

        void SetBuf(const t_sample *b) { buf = b; }

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

        void SetBuf(t_sample *b) { buf = b; }

    private:
        t_sample *buf;
    };

    virtual bool CbDsp(); 
    virtual void CbSignal(); 

    void ClearObjs();

    int inobjs,outobjs;
    SndObj **tmpobj;
    Inlet **inobj;
    Outlet **outobj;

    float smprt;
    int blsz;
};

#endif
