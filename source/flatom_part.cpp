/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flatom_part.cpp
    \brief Definitions for handling the t_atom type and lists thereof.
*/
 
#include "flext.h"

int flext::AtomList::Get(t_atom *argv,int mxsz) const
{
	int argc = Count();
	if(mxsz >= 0 && argc > mxsz) argc = mxsz;

	for(int i = 0; i < argc; ++i) SetAtom(argv[i],lst[i]);

	return argc;
}


flext::AtomList flext::AtomList::GetPart(int offs,int len) const
{
	if(offs+len > Count()) {
		len = Count()-offs;
		if(len < 0) len = 0;
	}

	return AtomList(len,Atoms()+offs);
}
