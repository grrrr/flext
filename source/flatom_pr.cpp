/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flatom_pr.cpp
    \brief Definitions for printing and scanning the t_atom type.
*/
 
#include "flext.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __MWERKS__
#define STD std
#else
#define STD
#endif

void flext::PrintAtom(const t_atom &a,char *buf)
{
	switch(a.a_type) {
	case A_NULL:
		break;
	case A_FLOAT:
#ifdef PD
		if(a.a_w.w_float == (int)a.a_w.w_float)
			STD::sprintf(buf,"%i",(int)GetFloat(a));
		else
#endif
		STD::sprintf(buf,"%f",GetFloat(a));
		break;
#ifdef MAXMSP
	case A_LONG:
		STD::sprintf(buf,"%i",GetInt(a));
		break;
#endif
#ifdef PD
	case A_POINTER:
		STD::sprintf(buf,"%x",GetPointer(a));
		break;
#endif
	case A_SYMBOL:
		strcpy(buf,GetString(a));
		break;
#ifdef _DEBUG
	default:
		ERRINTERNAL();
#endif
	}
}

bool flext::ScanAtom(t_atom &a,const char *buf)
{
	// skip whitespace
	while(*buf && isspace(*buf)) ++buf;
	if(!*buf) return false;

	char tmp[1024];
	strcpy(tmp,buf);
	char *c = tmp;

	// check for word type (s = 0,1,2 ... int,float,symbol)
	int s = 0;
	for(; *c && !isspace(*c); ++c) {
		if(!isdigit(*c)) 
			s = (*c != '.' || s == 1)?2:1;
	}

	switch(s) {
	case 0: // integer
#ifdef MAXMSP
		SetInt(a,atol(tmp));
		break;
#endif
	case 1: // float
		SetFloat(a,(float)atof(tmp));
		break;
	default: { // anything else is a symbol
		char t = *c; *c = 0;
		SetString(a,tmp);
		*c = t;
		break;
	}
	}

	return true;
}


