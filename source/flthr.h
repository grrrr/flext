/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flthr.h
    \brief Definitions for multithreading
	\internal
*/

#ifndef __FLEXT_THR_H
#define __FLEXT_THR_H

// threading must be turned on
#ifdef FLEXT_THREADS

#include <pthread.h>
#include <sched.h>

class flext_base::thr_params 
{
public:
	thr_params(flext_base *c,int n = 1);
	~thr_params();

	void set_any(const t_symbol *s,int argc,const t_atom *argv);
	void set_list(int argc,const t_atom *argv);

	flext_base *cl;
	union _data {
		bool _bool;
		float _float;
		int _int;
		t_symptr _t_symptr;
		struct { AtomAnything *args; } _any;
		struct { AtomList *args; } _list;
		struct { void *data; } _ext;
	} *var;
};

class flext_base::thr_entry 
{
public:
	thr_entry(pthread_t id = pthread_self()): thrid(id),nxt(NULL) {}

	bool Is(pthread_t id = pthread_self()) const { return pthread_equal(thrid,id) != 0; }

	pthread_t thrid;
	thr_entry *nxt;
};

#endif // FLEXT_THREADS


#endif
