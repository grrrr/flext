/* 

max-pd - compatibility library for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

// This is all borrowed from GEM by Mark Danks
 
#include "_cpp.h"

EXT_EXTERN void *operator new(size_t, void *location, void *) {return(location);}

#ifdef PD
t_object *CPPExtern::m_holder;
#elif defined(MAX)
t_pxobject *CPPExtern::m_holder;
#endif

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
#endif
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
CPPExtern :: ~CPPExtern()
{ }
