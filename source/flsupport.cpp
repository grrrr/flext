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

#include <stdio.h>
#include <stdarg.h>
#include <new>

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
	sym_anything = gensym("anything");
	sym_pointer = gensym("pointer");
	sym_float = gensym("float");
	sym_symbol = gensym("symbol");
	sym_bang = gensym("bang");
	sym_list = gensym("list");
	sym_signal = gensym("signal");
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

#if FLEXT_SYS == FLEXT_SYS_MAX && FLEXT_OS == FLEXT_OS_MAC && FLEXT_THREADS == FLEXT_THR_POSIX
	// for Max OSX/CFM MachO functions have to be imported
    CFBundleRef bundle = NULL;
    if (bundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.Carbon")))
	{
	    //load pthread function pointers
		pthread_self 		= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_self"));
		pthread_equal 		= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_equal"));
		pthread_create 		= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_create"));
		pthread_cancel 		= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_cancel"));
		pthread_testcancel 	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_testcancel"));
		pthread_join 		= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_join"));
		pthread_exit 		= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_exit"));

		pthread_attr_init	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_attr_init"));
		pthread_attr_destroy	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_attr_destroy"));
		pthread_attr_setdetachstate	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_attr_setdetachstate"));

		pthread_attr_getschedparam	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_attr_getschedparam"));
		pthread_attr_setschedparam	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_attr_setschedparam"));

		pthread_mutex_init	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_mutex_init"));
		pthread_mutex_destroy	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_mutex_destroy"));
		pthread_mutex_lock	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_mutex_lock"));
		pthread_mutex_unlock	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_mutex_unlock"));
		pthread_mutex_trylock	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_mutex_trylock"));

		pthread_cond_init	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_cond_init"));
		pthread_cond_destroy	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_cond_destroy"));
		pthread_cond_signal	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_cond_signal"));
		pthread_cond_wait	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_cond_wait"));
		pthread_cond_timedwait	= CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_cond_timedwait"));

		sched_yield	= CFBundleGetFunctionPointerForName(bundle, CFSTR("sched_yield"));
		sched_get_priority_min	= CFBundleGetFunctionPointerForName(bundle, CFSTR("sched_get_priority_min"));
		sched_get_priority_max	= CFBundleGetFunctionPointerForName(bundle, CFSTR("sched_get_priority_max"));
    }
#endif

	thrid = GetThreadId();
    StartHelper();
#endif
}


#if FLEXT_SYS == FLEXT_SYS_PD && defined(FLEXT_THREADED) && defined(FLEXT_PDLOCK)
#error
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
#if FLEXT_SYS == FLEXT_SYS_MAX
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
#else
	    blk = (char *)::getbytes(bytes);
#endif
        SYSUNLOCK();
    }

	*(size_t *)blk = bytes;
	return blk+sizeof(size_t);
}

void flext_root::operator delete(void *blk)
{
	char *ori = (char *)blk-sizeof(size_t);
	size_t bytes = *(size_t *)ori;

    if(bytes >= LARGEALLOC) {
#if FLEXT_SYS == FLEXT_SYS_MAX
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
#if FLEXT_SYS == FLEXT_SYS_MAX
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
#else
	    blk = (char *)::getbytes(bytes);
#endif
        SYSUNLOCK();
    }

	char *ablk = reinterpret_cast<char *>((reinterpret_cast<unsigned long>(blk)+ovh+alignovh) & ~alignovh);
	*(char **)(ablk-sizeof(size_t)-sizeof(char *)) = blk;
	*(size_t *)(ablk-sizeof(size_t)) = bytes;
	return ablk;
}

void flext_root::FreeAligned(void *blk)
{
	char *ori = *(char **)((char *)blk-sizeof(size_t)-sizeof(char *));
	size_t bytes = *(size_t *)((char *)blk-sizeof(size_t));

    if(bytes >= LARGEALLOC) {
#if FLEXT_SYS == FLEXT_SYS_MAX
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
    // no checking for size here
    if(IsSymbol(a)) STD::sprintf(buf,GetString(a));
	else if(IsFloat(a)) STD::sprintf(buf,"%f",GetFloat(a));
	else if(IsInt(a)) STD::sprintf(buf,"%i",GetInt(a));
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


void flext_root::post(const char *fmt, ...)
{
	va_list ap;
    va_start(ap, fmt);

	char buf[1024]; // \TODO this is quite unsafe.....
    vsprintf(buf, fmt, ap);
	::post(buf);

    va_end(ap);
}

void flext_root::error(const char *fmt,...)
{
	va_list ap;
    va_start(ap, fmt);

	char buf[1024]; // \TODO this is quite unsafe.....
    STD::sprintf(buf,"error: ");
    vsprintf(buf+7, fmt, ap);
	::post(buf);

    va_end(ap);
}
