/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flatom.cpp
    \brief Definitions for handling the t_atom type and lists thereof.
*/

#include <flext.h>

flext_base::AtomList::AtomList(int argc,t_atom *argv):
	cnt(argc)
{
	lst = new t_atom[cnt];

	if(argv) {
		for(int i = 0; i < argc; ++i) {
			switch(lst[i].a_type = argv[i].a_type) {
			case A_FLOAT:
				lst[i].a_w.w_float = argv[i].a_w.w_float;
				break;
#ifdef MAXMSP
			case A_LONG:
				lst[i].a_w.w_int = argv[i].a_w.w_int;
				break;
#endif
			case A_SYMBOL:
				lst[i].a_w.w_symbol = argv[i].a_w.w_symbol;
				break;
#ifdef PD
			case A_POINTER:
				lst[i].a_w.w_gpointer = argv[i].a_w.w_gpointer;
				break;
#endif
			default:
				post("AtomList - atom type (%i) not supported",lst[i].a_type);
				lst[i].a_type = A_NULL;
				break;
			}
		}
	}
}

flext_base::AtomList::~AtomList()
{
	if(lst) delete[] lst;
}

