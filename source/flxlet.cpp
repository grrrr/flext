/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flxlet.cpp
    \brief Implementation of the variable inlet/outlet functionality.
*/
 
#include "flext.h"
#include "flinternal.h"
#include <string.h>
#include <stdarg.h>


flext_base::xlet::xlet(type t,const char *d): 
    tp(t),nxt(NULL)
{ 
    if(d) {
        size_t ln = strlen(d);
        desc = new char[ln+1];
        memcpy(desc,d,ln);
        desc[ln] = 0;
    }
    else desc = NULL;
}

flext_base::xlet::~xlet() 
{ 
    if(desc) delete[] desc;
    if(nxt) delete nxt; 
}

void flext_base::AddXlet(xlet::type tp,int mult,const char *desc,xlet *&root)
{
    if(!root && mult) { root = new xlet(tp,desc); --mult; }
    if(mult) {
        xlet *xi = root; 
        while(xi->nxt) xi = xi->nxt;
        while(mult--) xi = xi->nxt = new xlet(tp,desc);
    }
}

void flext_base::DescXlet(int ix,const char *d,xlet *&root)
{
    xlet *xi = root; 
    for(int i = 0; xi && i < ix; xi = xi->nxt,++i) {}

    if(xi) {
        if(xi->desc) delete[] xi->desc;
        size_t ln = strlen(d);
        xi->desc = new char[ln+1];
        memcpy(xi->desc,d,ln);
        xi->desc[ln] = 0;
    }
}

unsigned long flext_base::XletCode(xlet::type tp,...)
{
    unsigned long code = 0;

    va_list marker;
    va_start(marker,tp);
    int cnt = 0;
    xlet::type arg = tp;
    for(; arg; ++cnt) {
#ifdef FLEXT_DEBUG
        if(cnt > 9) {
            error("%s - Too many in/outlets defined - truncated to 9",thisName());
            break;          
        }
#endif          

        code = code*10+(int)arg;
        arg = (xlet::type)va_arg(marker,int);
    }
    va_end(marker);

    return code;
}

void flext_base::AddInlets(unsigned long code) 
{ 
    for(; code; code /= 10) AddInlet((xlet::type)(code%10));
}

void flext_base::AddOutlets(unsigned long code) 
{ 
    for(; code; code /= 10) AddOutlet((xlet::type)(code%10));
}

