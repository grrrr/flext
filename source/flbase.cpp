/* 

flext - C++ compatibility layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all borrowed from GEM by Mark Danks
 
#include "flbase.h"

FLEXT_EXT void *operator new(size_t, void *location, void *) {return(location);}

t_sigobj *flext_obj::m_holder;

/////////////////////////////////////////////////////////
//
// flext_obj
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
flext_obj :: flext_obj()
           : x_obj(m_holder)
		   , m_name("#object in construction#")
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
flext_obj :: ~flext_obj()
{ }
