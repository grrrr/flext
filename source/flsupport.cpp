/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsupport.cpp
    \brief flext support functions and classes.
*/
 
#include "flext.h"

const t_symbol *flext::sym_float = NULL;
const t_symbol *flext::sym_symbol = NULL;
const t_symbol *flext::sym_bang = NULL;
const t_symbol *flext::sym_list = NULL;
const t_symbol *flext::sym_pointer = NULL;
const t_symbol *flext::sym_int = NULL;

#if FLEXT_SYS != FLEXT_SYS_JMAX
const t_symbol *flext::sym_anything = NULL;
#endif

#if FLEXT_SYS == FLEXT_SYS_PD
const t_symbol *flext::sym_signal = NULL;
#endif

int flext::Version() { return FLEXT_VERSION; }
const char *flext::VersionStr() { return FLEXT_VERSTR; }

void flext::FLEXT_SETUPFUNC()
{
	static bool issetup = false;
	if(issetup) 
		return;
	else issetup = true;

#if FLEXT_SYS == FLEXT_SYS_PD
	sym_anything = &s_anything;
	sym_pointer = &s_pointer;
	sym_float = &s_float;
	sym_symbol = &s_symbol;
	sym_bang = &s_bang;
	sym_list = &s_list;
	sym_signal = &s_signal;
#elif FLEXT_SYS == FLEXT_SYS_MAX
	sym_int = gensym("int");
	sym_float = gensym("float");
	sym_symbol = gensym("symbol");
	sym_bang = gensym("bang");
	sym_list = gensym("list");
	sym_anything = gensym("anything");
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	sym_int = fts_s_int;
	sym_float = fts_s_float;
	sym_symbol = fts_s_symbol;
	sym_bang = fts_s_bang;
	sym_list = fts_s_list;
	sym_pointer = fts_s_pointer;
#else
#endif

#ifdef FLEXT_THREADS
	thrid = GetThreadId();
    StartHelper();
#endif
}


/////////////////////////////////////////////////////////
// overloaded new/delete memory allocation methods
//
/////////////////////////////////////////////////////////

void *flext::operator new(size_t bytes)
{
	bytes += sizeof(size_t);
#if FLEXT_SYS == FLEXT_SYS_JMAX
	char *blk = (char *)::fts_malloc(bytes);
#else
	char *blk = (char *)::getbytes(bytes);
#endif
	*(size_t *)blk = bytes;
	return blk+sizeof(size_t);
}

void flext::operator delete(void *blk)
{
	char *ori = (char *)blk-sizeof(size_t);
#if FLEXT_SYS == FLEXT_SYS_JMAX
	fts_free(ori);
#else
	size_t bytes = *(size_t *)ori;
	::freebytes(ori,bytes);
#endif
}

void *flext::NewAligned(size_t bytes,int bitalign)
{
	const size_t ovh = sizeof(size_t)+sizeof(char *);
	const unsigned long alignovh = bitalign/8-1;
	bytes += ovh+alignovh;
#if FLEXT_SYS == FLEXT_SYS_JMAX
	char *blk = (char *)::fts_malloc(bytes);
#else
	char *blk = (char *)::getbytes(bytes);
#endif
	char *ablk = reinterpret_cast<char *>((reinterpret_cast<unsigned long>(blk)+ovh+alignovh) & ~alignovh);
	*(char **)(ablk-sizeof(size_t)-sizeof(char *)) = blk;
	*(size_t *)(ablk-sizeof(size_t)) = bytes;
	return ablk;
}

void flext::FreeAligned(void *blk)
{
	char *ori = *(char **)((char *)blk-sizeof(size_t)-sizeof(char *));

#if FLEXT_SYS == FLEXT_SYS_JMAX
	fts_free(ori);
#else
	size_t bytes = *(size_t *)((char *)blk-sizeof(size_t));
	::freebytes(ori,bytes);
#endif
}

// ------------------------------------------

/*! \todo there is probably also a shortcut for Max and jMax
    \todo size checking
*/
void flext::GetAString(const t_atom &a,char *buf,int szbuf)
{ 
#if FLEXT_SYS == FLEXT_SYS_PD
	atom_string(const_cast<t_atom *>(&a),buf,szbuf);
#else
    // no checking for size here
    if(IsSymbol(a)) sprintf(buf,GetString(a));
	else if(IsFloat(a)) sprintf(buf,"%f",GetFloat(a));
	else if(IsInt(a)) sprintf(buf,"%i",GetInt(a));
    else *buf = 0;
#endif
}  

unsigned long flext::AtomHash(const t_atom &a)
{
#if FLEXT_SYS == FLEXT_SYS_MAX || FLEXT_SYS == FLEXT_SYS_PD
	return ((unsigned long)a.a_type<<28)^*(unsigned long *)&a.a_w;
#else
#error Not implemented
#endif
}

unsigned int flext::FoldBits(unsigned long h,int bits)
{
	if(!bits) return 0;
	const int hmax = (1<<bits)-1;
	unsigned int ret = 0;
	for(unsigned int i = 0; i < sizeof(h)*8; i += bits)
		ret ^= (h>>i)&hmax;
	return ret;
}

int flext::Int2Bits(unsigned long n)
{
	int b;
	for(b = 0; n; ++b) n >>= 1;
	return b;
}

