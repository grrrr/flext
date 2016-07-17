/*
flext - C++ layer for Max and Pure Data externals

Copyright (c) 2001-2015 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.
*/

/*! \file fldefs_attradd.h
    \brief This file contains all #defines for actual usage
    
*/

#ifndef __FLEXT_DEFS_ATTRADD_H
#define __FLEXT_DEFS_ATTRADD_H


/*! \defgroup FLEXT_D_CADDATTR Announce object attributes at class scope
    \ingroup FLEXT_D_ATTRIB
    \note These can only be used at class construction time
    @{ 
*/

//! Add handler for a gettable attribute
#define FLEXT_CADDATTR_GET(CL,NAME,GFUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable attribute
#define FLEXT_CADDATTR_SET(CL,NAME,SFUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),NULL,(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define FLEXT_CADDATTR_VAR(CL,NAME,GFUN,SFUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),(FLEXT_GET_PRE(GFUN)),(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define FLEXT_CADDATTR_VAR1(CL,NAME,FUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),(FLEXT_GET_PRE(FUN)),(FLEXT_SET_PRE(FUN)))


//! Add handler for a gettable enum attribute
#define FLEXT_CADDATTR_GET_E(CL,NAME,GFUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable enum attribute
#define FLEXT_CADDATTR_SET_E(CL,NAME,SFUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),NULL,(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define FLEXT_CADDATTR_VAR_E(CL,NAME,GFUN,SFUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define FLEXT_CADDATTR_VAR1_E(CL,NAME,FUN) \
\
flext_base::AddAttrib(CL,flext::MakeSymbol(NAME),(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(FUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(FUN)))

//! @} FLEXT_D_CADDATTR


/*! \defgroup FLEXT_D_ADDATTR Announce object attributes 
    \ingroup FLEXT_D_ATTRIB
    \note These can only be used at object construction time
    \note (in constructor or in Init() function before call to parent's Init())
    @{ 
*/

//! Add handler for a gettable attribute
#define FLEXT_ADDATTR_GET(NAME,GFUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable attribute
#define FLEXT_ADDATTR_SET(NAME,SFUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),NULL,(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define FLEXT_ADDATTR_VAR(NAME,GFUN,SFUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),(FLEXT_GET_PRE(GFUN)),(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable attribute
#define FLEXT_ADDATTR_VAR1(NAME,FUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),(FLEXT_GET_PRE(FUN)),(FLEXT_SET_PRE(FUN)))


//! Add handler for a gettable enum attribute
#define FLEXT_ADDATTR_GET_E(NAME,GFUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),NULL)

//! Add handler for a settable enum attribute
#define FLEXT_ADDATTR_SET_E(NAME,SFUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),NULL,(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define FLEXT_ADDATTR_VAR_E(NAME,GFUN,SFUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(GFUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(SFUN)))

//! Add handlers for a both get- and settable enum attribute
#define FLEXT_ADDATTR_VAR1_E(NAME,FUN) \
\
flext_base::AddAttrib(flext::MakeSymbol(NAME),(bool (*)(flext_base *,int &))(FLEXT_GET_PRE(FUN)),(bool (*)(flext_base *,int &))(FLEXT_SET_PRE(FUN)))

//! @} FLEXT_D_ADDATTR


#endif
