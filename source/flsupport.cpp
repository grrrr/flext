/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsupport.cpp
    \brief flext support functions and classes.
*/
 
#include "flext.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
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
const t_symbol *flext::sym_signal = NULL;

#if FLEXT_SYS != FLEXT_SYS_JMAX
const t_symbol *flext::sym_anything = NULL;

const t_symbol *sym_buffer = NULL;
const t_symbol *sym_size = NULL;
#endif

const t_symbol *sym_attributes = NULL;
const t_symbol *sym_methods = NULL;


int flext::Version() { return FLEXT_VERSION; }
const char *flext::VersionStr() { return FLEXT_VERSTR; }


void flext::Setup()
{
	static bool issetup = false;
	if(issetup) 
		return;
	else issetup = true;

#if FLEXT_SYS == FLEXT_SYS_PD
	sym__ = &s_;
	sym_anything = &s_anything;
	sym_pointer = &s_pointer;
	sym_float = &s_float;
	sym_symbol = &s_symbol;
	sym_bang = &s_bang;
	sym_list = &s_list;
	sym_signal = &s_signal;
	sym_int = gensym("int");
#elif FLEXT_SYS == FLEXT_SYS_MAX
	sym__ = gensym("");
	sym_int = gensym("int");
	sym_float = gensym("float");
	sym_symbol = gensym("symbol");
	sym_bang = gensym("bang");
	sym_list = gensym("list");
	sym_anything = gensym("anything");
	sym_signal = gensym("signal");

    sym_buffer = flext::MakeSymbol("buffer~");
    sym_size = flext::MakeSymbol("size");
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

    sym_attributes = flext::MakeSymbol("attributes");
    sym_methods = flext::MakeSymbol("methods");

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

#ifdef FLEXT_DEBUGMEM
static const size_t memtest = 0x12345678L;
#endif

void *flext_root::operator new(size_t bytes)
{
	bytes += sizeof(size_t);
#ifdef FLEXT_DEBUGMEM
    bytes += sizeof(memtest)*2;
#endif
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
#ifdef FLEXT_DEBUGMEM
    *(size_t *)(blk+sizeof(size_t)) = memtest;
    *(size_t *)(blk+bytes-sizeof(memtest)) = memtest;
	return blk+sizeof(size_t)+sizeof(memtest);
#else
	return blk+sizeof(size_t);
#endif
}

void flext_root::operator delete(void *blk)
{
	FLEXT_ASSERT(blk);
    FLEXT_ASSERT(MemCheck(blk));

#ifdef FLEXT_DEBUGMEM
	char *ori = (char *)blk-sizeof(size_t)-sizeof(memtest);
#else
    char *ori = (char *)blk-sizeof(size_t);
#endif
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

#ifdef FLEXT_DEBUGMEM
bool flext_root::MemCheck(void *blk)
{
	char *ori = (char *)blk-sizeof(size_t)-sizeof(memtest);
	size_t bytes = *(size_t *)ori;

    return 
        *(size_t *)((char *)ori+sizeof(size_t)) == memtest && 
        *(size_t *)((char *)ori+bytes-sizeof(memtest)) == memtest;
}
#endif

void *flext_root::NewAligned(size_t bytes,int bitalign)
{
	const size_t ovh = sizeof(size_t)+sizeof(char *);
	const size_t alignovh = bitalign/8-1;
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

	char *ablk = reinterpret_cast<char *>((reinterpret_cast<size_t>(blk)+ovh+alignovh) & ~alignovh);
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
void flext::GetAString(const t_atom &a,char *buf,size_t szbuf)
{ 
#if FLEXT_SYS == FLEXT_SYS_PD
	atom_string(const_cast<t_atom *>(&a),buf,(int)szbuf);
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


TableAnyMap::~TableAnyMap() { clear(); }

void TableAnyMap::clear() 
{
    if(left) { _delmap(left); left = NULL; }
    if(right) { _delmap(right); right = NULL; }
    n = 0;
}


void *TableAnyMap::_set(int tsize,size_t k,void *t)
{
    FLEXT_ASSERT(n);

    if(n < tsize) {
        // fall through
    }
    else if(k < data[0].key)
        return _toleft(tsize,k,t);
    else if(k > data[tsize-1].key)
        return _toright(tsize,k,t);

    int ix = _tryix(k);
    if(ix >= n) {
        FLEXT_ASSERT(ix == n);
        // after last entry
        data[n++](k,t);
        return NULL;
    }

    size_t dk = data[ix].key;
    if(k == dk) {
        // update data in existing slot (same key)
        void *a = data[ix].value;
        data[ix] = t;
        return a;
    }
    else {
        // insert new slot by shifting the higher ones
        FLEXT_ASSERT(k < dk);
        void *a;
        if(n == tsize)
            a = _toright(tsize,data[tsize-1]);
        else {
            ++n;
            a = NULL;
        }

        Data *tg = data+ix;
        for(Data *d = data+n-1; d > tg; d--) d[0] = d[-1];
        (*tg)(k,t);
        return a;
    }
}

void *TableAnyMap::_find(int tsize,size_t k) const
{
    FLEXT_ASSERT(n);
    if(n < tsize) {
        // fall through
    }
    else if(k < data[0].key)
        return left?left->_find(tsize,k):NULL;
    else if(k > data[n-1].key)
        return right?right->_find(tsize,k):NULL;

    const int ix = _tryix(k);
    return ix < n && data[ix].key == k?data[ix].value:NULL;
}

#ifdef FLEXT_DEBUG
void TableAnyMap::_check(int tsize)
{
    FLEXT_ASSERT(n);

    size_t k = data[0].key;
    for(int i = 1; i < n; ++i) {
        size_t k2 = data[i].key;
        FLEXT_ASSERT(k < k2);
        k = k2;
    }

    if(left || right) FLEXT_ASSERT(n == tsize);

    if(left) { 
        FLEXT_ASSERT(flext::MemCheck(left)); 
        left->_check(tsize); 
    }
    if(right) { 
        FLEXT_ASSERT(flext::MemCheck(right)); 
        right->_check(tsize); 
    }
}
#endif

void *TableAnyMap::_remove(int tsize,size_t k)
{
    FLEXT_ASSERT(n);
    if(n < tsize) {
        // fall through
    }
    else if(k < data[0].key) {
        void *r = left?left->_remove(tsize,k):NULL;
        if(r) _eraseempty(left);
        return r;
    }
    else if(k > data[n-1].key) {
        void *r = right?right->_remove(tsize,k):NULL;
        if(r) _eraseempty(right);
        return r;
    }

    const int ix = _tryix(k);
    if(ix >= n || data[ix].key != k)
        return NULL;
    else {
        // found key in this map
        void *ret = data[ix].value;

        Data dt;
        bool fnd,ins = false;
        if(n >= tsize) {
            // if this table is full get fill-in elements from branches
            if(left) {
                // try to get biggest element from left branch
                left->_getbig(dt);
                _eraseempty(left);
                fnd = true,ins = true;
            }
            else if(right) {
                // try to get smallest element from right branch
                right->_getsmall(dt);
                _eraseempty(right);
                fnd = true;
            }
            else
                fnd = false;
        }
        else fnd = false;

        if(ins) {
            // insert smaller element from left
            for(int i = ix; i; --i) data[i] = data[i-1];
            data[0] = dt;
        }
        else {
            // shift elements
            for(int i = ix+1; i < n; ++i) data[i-1] = data[i];
            // insert bigger element from right or reduce table size
            if(fnd)
                data[n-1] = dt;
            else
                --n;
        }

        return ret;
    }
}

void TableAnyMap::_getbig(Data &dt)
{
    FLEXT_ASSERT(n);

    if(right) {
        right->_getbig(dt);
        _eraseempty(right);
    }
    else {
        dt = data[n-1];
        if(left) {
            for(int i = n-1; i; --i) data[i] = data[i-1];
            left->_getbig(data[0]);
            _eraseempty(left);
        }
        else
            --n;
    }
}

void TableAnyMap::_getsmall(Data &dt)
{
    FLEXT_ASSERT(n);

    if(left) {
        left->_getsmall(dt);
        _eraseempty(left);
    }
    else {
        dt = data[0];
        for(int i = 1; i < n; ++i) data[i-1] = data[i];
        if(right) {
            right->_getsmall(data[n-1]);
            _eraseempty(right);
        }
        else
            --n;
    }
}

void TableAnyMap::iterator::forward() 
{ 
    if(map || ix >= map->n) {
        if(++ix >= map->n) {
            TableAnyMap *nmap;

            // we reached the end of the slots
            if(map->right) {
                // climb up one
                map = map->right;
                leftmost();
                ix = 0;
            }
            else {
                // fall back
                for(;;) {
                    nmap = map->parent;
                    if(!nmap) break; // no parent
                    if(nmap->left == map) {
                        // ok, we are in front of the slots now
                        ix = 0;
                        map = nmap;
                        break;
                    }
                    else {
                        FLEXT_ASSERT(nmap->right == map);
                        ix = (map = nmap)->n;
                    }
                }
            }
        }
    }
}
