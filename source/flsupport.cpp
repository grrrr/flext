/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
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
const t_symbol *flext::sym_anything = NULL;
const t_symbol *flext::sym_pointer = NULL;
const t_symbol *flext::sym_int = NULL;

#if FLEXT_SYS == FLEXT_SYS_PD
const t_symbol *flext::sym_signal = NULL;
#endif

void flext::Setup()
{
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
#else
#error
#endif
}

void flext::GetAString(const t_atom &a,char *buf,int szbuf)
{ 
#if FLEXT_SYS == FLEXT_SYS_PD
	atom_string(const_cast<t_atom *>(&a),buf,szbuf);
#else
	if(IsSymbol(a)) sprintf(buf,GetString(a));
	else if(IsFloat(a)) sprintf(buf,"%f",GetFloat(a));
	else if(IsInt(a)) sprintf(buf,"%i",GetInt(a));
#endif
}  

int flext::FoldBits(unsigned long h,int bits)
{
	if(!bits) return 0;
	const int hmax = (1<<bits)-1;
	int ret = 0;
	for(int i = 0; i < sizeof(h)*8; i += bits)
		ret = ret^((h>>i)&hmax);
	return ret;
}

int flext::Int2Bits(unsigned long n)
{
	int b;
	for(b = 0; n; ++b) n >>= 1;
	return b;
}

