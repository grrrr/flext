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

const char *extractname(const char *name,int ix)
{
	static char tmp[1024];
	const char *n = name;
	for(int i = 0; n && *n; ++i) {
		if(i == ix) {
			char *t;
			for(t = tmp; *n && !isspace(*n); ++t,++n) *t = *n;
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

