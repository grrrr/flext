/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flsetup.cpp
    \brief Helper functions for object setup
    
    !Lacking Details.!
*/

#include <flext.h>
#include <ctype.h>
#include <string.h>

const char *fl_strdup(const char *t)
{
	if(!t) return NULL;
	int len = strlen(t);
	char *r = new char[len+1];
	memcpy(r,t,len+1);
	return r;
}

const char *fl_extract(const char *name,int ix)
{
	static char tmp[1024];
	const char *n = name;
	for(int i = 0; n && *n; ++i) {
		if(i == ix) {
			char *t;
			for(t = tmp; *n && !isspace(*n); ++t,++n) *t = *n;
			*t = 0;
			return *tmp?strdup(tmp):NULL;
		}
		else {
			while(*n && !isspace(*n)) ++n;
			while(*n && isspace(*n)) ++n;		
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////
// check if tilde object's name ends with a tilde
//
/////////////////////////////////////////////////////////


#ifdef _DEBUG
bool fl_chktilde(const char *objname)
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




