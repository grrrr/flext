/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsupport.cpp
    \brief flext support functions and classes.
*/
 
#include "flsupport.h"

const t_symbol *flext::sym_float = NULL;
const t_symbol *flext::sym_symbol = NULL;
const t_symbol *flext::sym_bang = NULL;
const t_symbol *flext::sym_list = NULL;
const t_symbol *flext::sym_anything = NULL;
const t_symbol *flext::sym_pointer = NULL;
const t_symbol *flext::sym_int = NULL;

#ifdef PD
const t_symbol *flext::sym_signal = NULL;
#endif


void flext::GetAString(const t_atom &a,char *buf,int szbuf)
{ 
#ifdef PD
	atom_string(const_cast<t_atom *>(&a),buf,szbuf);
#else
	if(IsSymbol(a)) sprintf(buf,GetString(a));
	else if(IsFloat(a)) sprintf(buf,"%f",GetFloat(a));
	else if(IsInt(a)) sprintf(buf,"%i",GetInt(a));
#endif
}  
