/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2004 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flbase.cpp
    \brief Implementation of the internal flext base classes.

    \remark This is all derived from GEM by Mark Danks
*/
 
#include "flext.h"
#include "flinternal.h"
#include <string.h>
#include <ctype.h>

#if FLEXT_SYS == FLEXT_SYS_PD
#ifdef _MSC_VER
    #pragma warning (push)
    #pragma warning (disable:4091)
#endif
#include <g_canvas.h>
#ifdef _MSC_VER
    #pragma warning (pop)
#endif
#endif


/////////////////////////////////////////////////////////
//
// flext_obj
//
/////////////////////////////////////////////////////////

flext_hdr *flext_obj::m_holder = NULL;
const t_symbol *flext_obj::m_holdname = NULL;
bool flext_obj::m_holdattr = false;
int flext_obj::m_holdaargc = 0;
const t_atom *flext_obj::m_holdaargv = NULL;
bool flext_obj::process_attributes = false;

bool flext_obj::initing = false;
bool flext_obj::exiting = false;

void flext_obj::ProcessAttributes(bool attr) { process_attributes = attr; }

/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
flext_obj :: FLEXT_CLASSDEF(flext_obj)()
    : x_obj(m_holder)
    , procattr(m_holdattr)
    , init_ok(true)
    , m_name(m_holdname)
{
#if FLEXT_SYS == FLEXT_SYS_PD
    m_canvas = canvas_getcurrent();
#elif FLEXT_SYS == FLEXT_SYS_MAX
    m_canvas = (t_patcher *)gensym("#P")->s_thing;
    x_obj->curinlet = 0;
#endif
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
flext_obj :: ~FLEXT_CLASSDEF(flext_obj)() {
    x_obj = NULL;
}


bool flext_obj::Init() { return true; }
bool flext_obj::Finalize() { return true; }
void flext_obj::Exit() {}

void flext_obj::DefineHelp(t_classid c,const char *ref,const char *dir,bool addtilde)
{
#if FLEXT_SYS == FLEXT_SYS_PD
    char tmp[256];
    if(dir) { 
        strcpy(tmp,dir); 
        strcat(tmp,"/"); 
        strcat(tmp,ref); 
    }
    else 
        strcpy(tmp,ref);
    if(addtilde) strcat(tmp,"~"); 

    ::class_sethelpsymbol(getClass(c),gensym(const_cast<char *>(tmp)));
#else
    // no solution for Max/MSP yet
#endif
}

bool flext_obj::GetParamSym(t_atom &dst,const t_symbol *sym,t_canvas *c)
{
#if FLEXT_SYS == FLEXT_SYS_PD && defined(PD_MINOR_VERSION) && PD_MINOR_VERSION >= 37
    if(!c) c = canvas_getcurrent();

    const char *s = GetString(sym);
    if((s[0] == '$' || s[0] == '#') && isdigit(s[1])) {
        const t_symbol *res;
        // patcher parameter detected... get value!
        if(s[0] != '$') {
            char tmp[MAXPDSTRING];
            strcpy(tmp,s);
            tmp[0] = '$';
            res = canvas_realizedollar(c,const_cast<t_symbol *>(MakeSymbol(tmp)));
        }
        else
            res = canvas_realizedollar(c,const_cast<t_symbol *>(sym));

        // check for number
        const char *c = GetString(res);
        while(*c && (isdigit(*c) || *c == '.')) ++c;

        if(!*c) 
            SetFloat(dst,(float)atof(GetString(res)));
        else
            SetSymbol(dst,res);
        return true;
    }
    else
#else
    #pragma message("Not implemented")
#endif
    SetSymbol(dst,sym);
    return true;
}


#if FLEXT_SYS == FLEXT_SYS_PD
// this declaration is missing in m_pd.h (0.37-0 and -1)
// but it is there in 0.37-2 (but how to tell which micro-version?)
extern "C" 
#ifdef _MSC_VER
__declspec(dllimport)
#endif
void canvas_getargs(int *argcp, t_atom **argvp);
#endif


void flext_obj::CanvasArgs(AtomList &args) const
{
#if FLEXT_SYS == FLEXT_SYS_PD
    int argc;
    t_atom *argv;
    canvas_getargs(&argc,&argv);
    args(argc);
    for(int i = 0; i < argc; ++i) args[i] = argv[i];
#else
    #pragma message("Not implemented")
    args(0);
#endif
}
