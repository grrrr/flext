/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flbase.cpp
    \brief Implementation of the internal flext base classes.

	\remark This is all derived from GEM by Mark Danks
*/
 
#include "flbase.h"
#include "flinternal.h"

/////////////////////////////////////////////////////////
//
// flext_obj
//
/////////////////////////////////////////////////////////

flext_hdr *flext_obj::m_holder;
const char *flext_obj::m_holdname;


/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
flext_obj :: flext_obj()
           : x_obj(m_holder)
		   , m_name(flext::strdup(m_holdname))
		   , init_ok(true)
{
#ifdef PD
    m_canvas = canvas_getcurrent();
#elif defined(MAXMSP)
    m_canvas = (t_patcher *)gensym("#P")->s_thing;
    x_obj->curinlet = 0;
#endif
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
flext_obj :: ~flext_obj()
{ 
	if(m_name) delete[] const_cast<char *>(m_name);
}




/////////////////////////////////////////////////////////
// overloaded new/delete memory allocation methods
//
/////////////////////////////////////////////////////////

void *flext_obj::operator new(size_t bytes)
{
	bytes += sizeof(size_t);
	char *blk = (char *)getbytes(bytes);
	*(size_t *)blk = bytes;
	return blk+sizeof(size_t);
}

void flext_obj::operator delete(void *blk)
{
	char *ori = (char *)blk-sizeof(size_t);
	size_t bytes = *(size_t *)ori;
	freebytes(ori,bytes);
}

void *flext_obj::NewAligned(size_t bytes,int bitalign)
{
	const size_t ovh = sizeof(size_t)+sizeof(char *);
	const unsigned long alignovh = bitalign/8-1;
	bytes += ovh+alignovh;
	char *blk = (char *)getbytes(bytes);
	char *ablk = reinterpret_cast<char *>((reinterpret_cast<unsigned long>(blk)+ovh+alignovh) & ~alignovh);
	*(char **)(ablk-sizeof(size_t)-sizeof(char *)) = blk;
	*(size_t *)(ablk-sizeof(size_t)) = bytes;
	return ablk;
}

void flext_obj::FreeAligned(void *blk)
{
	char *ori = *(char **)((char *)blk-sizeof(size_t)-sizeof(char *));
	size_t bytes = *(size_t *)((char *)blk-sizeof(size_t));
	freebytes(ori,bytes);
}





