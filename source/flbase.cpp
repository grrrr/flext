/* 

max-pd - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all borrowed from GEM by Mark Danks
 
#include "_cpp.h"

EXT_EXTERN void *operator new(size_t, void *location, void *) {return(location);}

t_sigobj *CPPExtern::m_holder;

/////////////////////////////////////////////////////////
//
// CPPExtern
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
CPPExtern :: CPPExtern()
           : x_obj(m_holder)
{
#ifdef PD
    m_canvas = canvas_getcurrent();
#elif defined(MAXMSP)
    m_canvas = (t_patcher *)gensym("#P")->s_thing;
#endif
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
CPPExtern :: ~CPPExtern()
{ }
