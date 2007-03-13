/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2007 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

#ifndef __FLSTK_H
#define __FLSTK_H

#include "flext.h"

// PI is defined in the Max/MSP SDK, but clashes with Stk.h
#ifdef PI
#undef PI
#endif

#include <Stk.h>

class FLEXT_SHARE flext_stk:
    public flext_dsp
{ 
    FLEXT_HEADER(flext_stk,flext_dsp)
 
public:
    flext_stk();

    // these have to be overridden in child classes
    virtual bool NewObjs() { return true; }
    virtual void FreeObjs() {}
    virtual void ProcessObjs(int blocksize) {}

protected:
    virtual bool Init();
    virtual void Exit();

    //! STK object for reading from inlet buffer
    class Input:
        public Stk
    {
    public:
        Input(const t_sample *b,int vecsz);

        inline MY_FLOAT lastOut() const { return (MY_FLOAT)buf[index]; }

        inline MY_FLOAT tick() 
        { 
            if(++index >= vecsz) index = 0; 
            return lastOut(); 
        }

        MY_FLOAT *tick(MY_FLOAT *vector,unsigned int vectorSize);

        inline void SetBuf(const t_sample *b) { buf = b; }

    private:
        const t_sample *buf;
        int vecsz,index;
    };

    //! STK object for writing to outlet buffer
    class Output:
        public Stk
    {
    public:
        Output(t_sample *b,int vecsz);

        inline void tick(MY_FLOAT s) 
        { 
            buf[index] = (t_sample)s; 
            if(++index >= vecsz) index = 0; 
        }

        void tick(const MY_FLOAT *vector,unsigned int vectorSize);

        inline void SetBuf(t_sample *b) { buf = b; }

    private:
        t_sample *buf;
        int vecsz,index;
    };

    Input &Inlet(int ix) { return *inobj[ix]; }
    Output &Outlet(int ix) { return *outobj[ix]; }

private:
    virtual bool CbDsp(); 
    virtual void CbSignal(); 

    void ClearObjs();

    int inobjs,outobjs;
    Input **inobj;
    Output **outobj;

    float smprt;
    int blsz;
};

#endif
