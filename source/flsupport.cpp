/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2004 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsupport.cpp
    \brief flext support functions and classes.
*/
 
#include "flext.h"

#include <stdio.h>
#include <stdarg.h>
#include <new>

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

const t_symbol *flext::sym__ = NULL;
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


void flext::Setup()
{
	static bool issetup = false;
	if(issetup) 
		return;
	else issetup = true;

#if FLEXT_SYS == FLEXT_SYS_PD
	sym__ = gensym("");
	sym_anything = gensym("anything");
	sym_pointer = gensym("pointer");
	sym_float = gensym("float");
	sym_symbol = gensym("symbol");
	sym_bang = gensym("bang");
	sym_list = gensym("list");
	sym_signal = gensym("signal");
	sym_int = gensym("int");
#elif FLEXT_SYS == FLEXT_SYS_MAX
	sym__ = gensym("");
	sym_int = gensym("int");
	sym_float = gensym("float");
	sym_symbol = gensym("symbol");
	sym_bang = gensym("bang");
	sym_list = gensym("list");
	sym_anything = gensym("anything");
#elif FLEXT_SYS == FLEXT_SYS_JMAX
	sym__ = fts_new_symbol("");; // is there a static symbol for that?
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


#if FLEXT_SYS == FLEXT_SYS_PD && defined(FLEXT_THREADED) && defined(FLEXT_PDLOCK)
#define SYSLOCK() sys_lock()
#define SYSUNLOCK() sys_unlock()
#else
#define SYSLOCK() (void)0
#define SYSUNLOCK() (void)0
#endif


/////////////////////////////////////////////////////////
// overloaded new/delete memory allocation methods
//
/////////////////////////////////////////////////////////

#define LARGEALLOC 32000

void *flext_root::operator new(size_t bytes)
{
	bytes += sizeof(size_t);

    char *blk;
    if(bytes >= LARGEALLOC) {
#if FLEXT_SYS == FLEXT_SYS_MAX && defined(_SYSMEM_H_)
        blk = (char *)::sysmem_newptr(bytes);
#else
        // use C library function for large memory blocks
        blk = (char *)::malloc(bytes);
#endif
    }
    else {
	//! We need system locking here for secondary threads!
        SYSLOCK();

#if FLEXT_SYS == FLEXT_SYS_JMAX
    	blk = (char *)::fts_malloc(bytes);
#elif defined(FLEXT_USECMEM)
	    blk = (char *)::malloc(bytes);
#else
	    blk = (char *)::getbytes(bytes);
#endif
        SYSUNLOCK();
    }

	FLEXT_ASSERT(blk);

	*(size_t *)blk = bytes;
	return blk+sizeof(size_t);
}

void flext_root::operator delete(void *blk)
{
	FLEXT_ASSERT(blk);

	char *ori = (char *)blk-sizeof(size_t);
	size_t bytes = *(size_t *)ori;

    if(bytes >= LARGEALLOC) {
#if FLEXT_SYS == FLEXT_SYS_MAX && defined(_SYSMEM_H_)
        ::sysmem_freeptr(ori);
#else
        // use C library function for large memory blocks
        ::free(ori);
#endif
    }
    else {
	//! We need system locking here for secondary threads!
        SYSLOCK();

#if FLEXT_SYS == FLEXT_SYS_JMAX
        ::fts_free(ori);
#elif defined(FLEXT_USECMEM)
	    ::free(ori);
#else
	    ::freebytes(ori,bytes);
#endif
        SYSUNLOCK();
    }
}

void *flext_root::NewAligned(size_t bytes,int bitalign)
{
	const size_t ovh = sizeof(size_t)+sizeof(char *);
	const unsigned long alignovh = bitalign/8-1;
	bytes += ovh+alignovh;

    char *blk;
    if(bytes >= LARGEALLOC) {
#if FLEXT_SYS == FLEXT_SYS_MAX && defined(_SYSMEM_H_)
        blk = (char *)::sysmem_newptr(bytes);
#else
        // use C library function for large memory blocks
        blk = (char *)::malloc(bytes);
#endif
    }
    else {
	//! We need system locking here for secondary threads!
        SYSLOCK();

#if FLEXT_SYS == FLEXT_SYS_JMAX
    	blk = (char *)::fts_malloc(bytes);
#elif defined(FLEXT_USECMEM)
	    blk = (char *)::malloc(bytes);
#else
	    blk = (char *)::getbytes(bytes);
#endif
        SYSUNLOCK();
    }
	FLEXT_ASSERT(blk);

	char *ablk = reinterpret_cast<char *>((reinterpret_cast<unsigned long>(blk)+ovh+alignovh) & ~alignovh);
	*(char **)(ablk-sizeof(size_t)-sizeof(char *)) = blk;
	*(size_t *)(ablk-sizeof(size_t)) = bytes;
	return ablk;
}

void flext_root::FreeAligned(void *blk)
{
	FLEXT_ASSERT(blk);

	char *ori = *(char **)((char *)blk-sizeof(size_t)-sizeof(char *));
	size_t bytes = *(size_t *)((char *)blk-sizeof(size_t));

    if(bytes >= LARGEALLOC) {
#if FLEXT_SYS == FLEXT_SYS_MAX && defined(_SYSMEM_H_)
        ::sysmem_freeptr(ori);
#else
        // use C library function for large memory blocks
        ::free(ori);
#endif
    }
    else {
	//! We need system locking here for secondary threads!
        SYSLOCK();

#if FLEXT_SYS == FLEXT_SYS_JMAX
        ::fts_free(ori);
#elif defined(FLEXT_USECMEM)
	    ::free(ori);
#else
	    ::freebytes(ori,bytes);
#endif
        SYSUNLOCK();
    }
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
    if(IsSymbol(a)) STD::strncpy(buf,GetString(a),szbuf);
	else if(IsFloat(a)) STD::snprintf(buf,szbuf,"%f",GetFloat(a));
	else if(IsInt(a)) STD::snprintf(buf,szbuf,"%i",GetInt(a));
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

void flext_root::post(const char *fmt, ...)
{
	va_list ap;
    va_start(ap, fmt);

	char buf[1024];
    vsnprintf(buf,sizeof buf,fmt, ap);
	buf[sizeof buf-1] = 0; // in case of full buffer
	::post(buf);

    va_end(ap);
}

void flext_root::error(const char *fmt,...)
{
	va_list ap;
    va_start(ap, fmt);

	char buf[1024];
    STD::strcpy(buf,"error: ");
    vsnprintf(buf+7,sizeof buf-7,fmt, ap);
	buf[sizeof buf-1] = 0; // in case of full buffer
	::post(buf);

    va_end(ap);
}


AnyMap::AnyMap() {}
AnyMap::~AnyMap() {}
AnyMap::iterator AnyMap::find(unsigned int k) { return Parent::find(k); }
unsigned int &AnyMap::operator [](unsigned int k) { return Parent::operator [](k); }
