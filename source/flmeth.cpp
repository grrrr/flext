/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flmeth.cpp
    \brief Method processing of flext base class.
*/
 
#include "flext.h"
#include <string.h>
#include <stdarg.h>

flext_base::methitem::methitem(int in,const t_symbol *tg,attritem *conn): 
	item(tg,in,conn),
	argc(0),args(NULL)
	,fun(NULL)
{}

flext_base::methitem::~methitem() 
{ 
	if(args) delete[] args; 
}

void flext_base::methitem::SetArgs(methfun _fun,int _argc,metharg *_args)
{
	fun = _fun;
	if(args) delete[] args;
	argc = _argc,args = _args;
}

void flext_base::AddMethodDef(int inlet,const char *tag)
{
	methhead->Add(new methitem(inlet,tag?MakeSymbol(tag):NULL));
}

/*! \brief Add a method to the queue
*/
void flext_base::AddMethod(itemarr *ma,int inlet,const char *tag,methfun fun,metharg tp,...)
{
	va_list marker; 

	// at first just count the arg type list (in argc)
	int argc = 0;
	va_start(marker,tp);
	metharg *args = NULL,arg = tp;
	for(; arg != a_null; ++argc) arg = (metharg)va_arg(marker,int); //metharg);
	va_end(marker);
	
	if(argc > 0) {
		if(argc > FLEXT_MAXMETHARGS) {
			error("flext - method %s: only %i arguments are type-checkable: use variable argument list for more",tag?tag:"?",FLEXT_MAXMETHARGS);
			argc = FLEXT_MAXMETHARGS;
		}

		args = new metharg[argc];

		va_start(marker,tp);
		metharg a = tp;
		for(int ix = 0; ix < argc; ++ix) {
#ifdef FLEXT_DEBUG
			if(a == a_list && ix > 0) {
				ERRINTERNAL();
			}
#endif
#if FLEXT_SYS == FLEXT_SYS_PD
			if(a == a_pointer && flext_base::compatibility) {
				post("Pointer arguments are not allowed in compatibility mode"); 
			}
#endif
			args[ix] = a;
			a = (metharg)va_arg(marker,int); //metharg);
		}
		va_end(marker);
	}
	
	methitem *mi = new methitem(inlet,MakeSymbol(tag));

	mi->SetArgs(fun,argc,args);

	ma->Add(mi);
}

