/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
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

// \TODO take bufsz into account!
bool flext::PrintAtom(const t_atom &a,char *buf,int bufsz)
{
	bool ok = true;
	if(IsFloat(a)) {
		STD::sprintf(buf,"%g",GetFloat(a));
	}
	else if(IsInt(a)) {
		STD::sprintf(buf,"%i",GetInt(a));
	}
	else if(IsSymbol(a)) {
		STD::strcpy(buf,GetString(a));
	}
	else if(IsPointer(a)) {
		STD::sprintf(buf,"%p",GetPointer(a));
	}
	else if(a.a_type == A_DOLLAR) {
		STD::sprintf(buf,"$%d",a.a_w.w_index);
	}
	else if(a.a_type == A_DOLLSYM) {
		STD::sprintf(buf,"$%s",GetString(a));
	}
	else {
		ERRINTERNAL();
		ok = false;
	}
	return ok;
}

bool flext::PrintList(int argc,const t_atom *argv,char *buf,int bufsz)
{
	bool ok = true;
    for(int i = 0; ok && i < argc && bufsz > 0; ++i) {
		if(i) { *(buf++) = ' '; --bufsz; } // prepend space

		if(PrintAtom(argv[i],buf,bufsz)) {
			int len = strlen(buf);
			buf += len,bufsz -= len;
		}
		else
			ok = false;
    }
	*buf = 0;
    return ok;
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
#if FLEXT_SYS == FLEXT_SYS_MAX
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


