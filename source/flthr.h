/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flthr.h
    \brief Definitions for multithreading
*/

#ifndef __FLEXT_THR_H
#define __FLEXT_THR_H

// threading must be turned on
#ifdef FLEXT_THREADS

#include <pthread.h>

class flext_base::thr_params {
public:
	thr_params(flext_base *c): cl(c) {}

	void set_any(t_symbol *s,int argc,t_atom *argv) { var[0]._any.s = s; var[0]._any.argc = argc; var[0]._any.argv = argv; }
	void get_any(t_symbol *&s,int &argc,t_atom *&argv) { s = var[0]._any.s; argc = var[0]._any.argc; argv = var[0]._any.argv; }

	void set_gimme(int argc,t_atom *argv) { var[0]._gimme.argc = argc; var[0]._gimme.argv = argv; }
	void get_gimme(int &argc,t_atom *&argv) { argc = var[0]._gimme.argc; argv = var[0]._gimme.argv; }

	flext_base *cl;
	union {
		bool _bool;
		float _float;
		int _int;
		t_symptr _t_symptr;
		struct { t_symbol *s; int argc; t_atom *argv; } _any;
		struct { int argc; t_atom *argv; } _gimme;
	} var[5];
};

#endif // FLEXT_THREADS


#endif
