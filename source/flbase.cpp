/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
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

void flext_obj::ProcessAttributes(bool attr) { process_attributes = attr; }

/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
flext_obj :: flext_obj()
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
flext_obj :: ~flext_obj() {
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
		if(addtilde) strcat(tmp,"~"); 
	}
	else 
		strcpy(tmp,ref);
    ::class_sethelpsymbol(getClass(c),gensym(const_cast<char *>(tmp)));
#else
	// no solution for Max/MSP yet
#endif
}





