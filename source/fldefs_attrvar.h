/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file fldefs_attrvar.h
    \brief This file contains all #defines for actual usage
    
*/

#ifndef __FLEXT_DEFS_ATTRVAR_H
#define __FLEXT_DEFS_ATTRVAR_H


/*! \brief Declare an implicite attribute set function
	\internal
*/
#define FLEXT_ATTRSET_(VAR,TP) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,TP &arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = arg; return true; }

/*! \brief Declare an implicite attribute get function
	\internal
*/
#define FLEXT_ATTRGET_(VAR,TP) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,TP &arg) \
{ arg = (TP)FLEXT_CAST<thisType *>(c)->VAR; return true; }



/*!	\defgroup FLEXT_DA_ATTRSET Definition of implicite attribute set handlers
	@{ 
*/

//! Declare an implicite set function for a float attribute
#define FLEXT_ATTRSET_F(VAR) \
\
FLEXT_ATTRSET_(VAR,float)

//! Declare an implicite set function for an integer attribute
#define FLEXT_ATTRSET_I(VAR) \
\
FLEXT_ATTRSET_(VAR,int)

//! Declare an implicite set function for a symbol attribute
#define FLEXT_ATTRSET_S(VAR) \
\
FLEXT_ATTRSET_(VAR,t_symptr)

//! Declare an implicite set function for a boolean attribute
#define FLEXT_ATTRSET_B(VAR) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,int &arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = arg != 0; return true; }

//! Declare an implicite set function for an enum attribute
#define FLEXT_ATTRSET_E(VAR,TP) \
\
FLEXT_ATTRSET_(VAR,TP)

//! Declare an implicite set function for a variable list attribute
#define FLEXT_ATTRSET_V(VAR) \
static bool FLEXT_SET_PRE(VAR)(flext_base *c,AtomList *&arg) \
{ FLEXT_CAST<thisType *>(c)->VAR = *arg; return true; }

//! @} FLEXT_DA_ATTRSET

/*!	\defgroup FLEXT_DA_ATTRGET Definition of implicite attribute get handlers
	@{ 
*/

//! Declare an implicite get function for a float attribute
#define FLEXT_ATTRGET_F(VAR) \
\
FLEXT_ATTRGET_(VAR,float)

//! Declare an implicite get function for an integer attribute
#define FLEXT_ATTRGET_I(VAR) \
\
FLEXT_ATTRGET_(VAR,int)

//! Declare an implicite get function for a symbol attribute
#define FLEXT_ATTRGET_S(VAR) \
\
FLEXT_ATTRGET_(VAR,t_symptr)

//! Declare an implicite get function for a boolean attribute
#define FLEXT_ATTRGET_B(VAR) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,int &arg) \
{ arg = FLEXT_CAST<thisType *>(c)->VAR?1:0; return true; }

//! Declare an implicite get function for an enum attribute
#define FLEXT_ATTRGET_E(VAR,TP) \
\
FLEXT_ATTRGET_(VAR,TP)

//! Declare an implicite get function for a variable list attribute
#define FLEXT_ATTRGET_V(VAR) \
static bool FLEXT_GET_PRE(VAR)(flext_base *c,AtomList *&arg) \
{ *arg = FLEXT_CAST<thisType *>(c)->VAR; return true; }

//! @} FLEXT_DA_ATTRGET


/*!	\defgroup FLEXT_DA_ATTRVAR Definition of implicite attribute transfer handlers (both get and set)
	@{ 
*/

//! Declare both implicite get and set functions for a float attribute
#define FLEXT_ATTRVAR_F(VAR) \
\
FLEXT_ATTRGET_F(VAR) FLEXT_ATTRSET_F(VAR) 

//! Declare both implicite get and set functions for an integer attribute
#define FLEXT_ATTRVAR_I(VAR) \
\
FLEXT_ATTRGET_I(VAR) FLEXT_ATTRSET_I(VAR) 

//! Declare both implicite get and set functions for a symbol attribute
#define FLEXT_ATTRVAR_S(VAR) \
\
FLEXT_ATTRGET_S(VAR) FLEXT_ATTRSET_S(VAR) 

//! Declare both implicite get and set functions for a boolean attribute
#define FLEXT_ATTRVAR_B(VAR) \
\
FLEXT_ATTRGET_B(VAR) FLEXT_ATTRSET_B(VAR) 

//! Declare both implicite get and set functions for an enum attribute
#define FLEXT_ATTRVAR_E(VAR,TP) \
\
FLEXT_ATTRGET_(VAR,TP) FLEXT_ATTRSET_(VAR,TP) 

//! Declare both implicite get and set functions for a variable list attribute
#define FLEXT_ATTRVAR_V(VAR) \
\
FLEXT_ATTRGET_V(VAR) FLEXT_ATTRSET_V(VAR) 


//! @} FLEXT_DA_ATTRVAR


#endif
