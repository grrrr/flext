/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsetup.cpp
    \brief Implementation of helper functions for object setup.
*/

#include "flext.h"
#include <ctype.h>
#include <string.h>

#define ALIASDEL ','

#define ALIASSLASHES ":/\\"
#ifdef MAXMSP
	#define ALIASSLASH ':'
#elif defined(NT)
	#define ALIASSLASH '/'
#else
	#define ALIASSLASH '/'
#endif

//! C++ strdup function
char *flext::strdup(const char *t)
{
	if(!t) return NULL;
	int len = strlen(t);
	char *r = new char[len+1];
	memcpy(r,t,len+1);
	return r;
}

//! Extract space-delimited words from a string
const char *flext::extract(const char *name,int ix)
{
	static char tmp[1024];
	const char *n = name;
	
	const char *del = strchr(name,ALIASDEL);

	if(del) {
		if(ix < 0) {
			char *t = tmp;
			while(n < del && (isspace(*n) || strchr(ALIASSLASHES,*n))) ++n;
			while(n < del && !isspace(*n)) {
				char c = *(n++);
				*(t++) = strchr(ALIASSLASHES,c)?ALIASSLASH:c;
			}
			while(*t == ALIASSLASH && t > tmp) --t;
			*t = 0;

			return tmp;
		}

		n = del+1;
	}
	
	for(int i = 0; n && *n; ++i) {
		if(i == ix) {
			char *t = tmp;

			for(; *n && !isspace(*n); ++t,++n) *t = *n;
			*t = 0;
			return *tmp?tmp:NULL;
		}
		else {
			while(*n && !isspace(*n)) ++n;
			while(*n && isspace(*n)) ++n;		
		}
	}

	return NULL;
}


#ifdef _DEBUG
//! Check if object's name ends with a tilde
bool flext::chktilde(const char *objname)
{
//	int stplen = strlen(setupfun);
	bool tilde = true; //!strncmp(setupfun,"_tilde",6);

	if((objname[strlen(objname)-1] == '~'?1:0)^(tilde?1:0)) {
		if(tilde) 
			error("flext_obj::check_tilde: %s (no trailing ~) is defined as a tilde object",objname);
		else
			error("flext_obj::check_tilde: %s is no tilde object",objname);
		return true;
	} 
	else
		return false;
}
#endif




