/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flprefix.h
    \brief Try to find out the platform.
*/
 
#ifndef __FLEXT_PREFIX_H
#define __FLEXT_PREFIX_H

// definitions for FLEXT_SYS
#define FLEXT_SYS_UNKNOWN	0
#define FLEXT_SYS_MAX	1
#define FLEXT_SYS_PD	2

// definitions for FLEXT_OS
#define FLEXT_OS_UNKNOWN	0
#define FLEXT_OS_WIN	1
#define FLEXT_OS_MACOS9	2
#define FLEXT_OS_MACOSX	3
#define FLEXT_OS_LINUX	4
#define FLEXT_OS_IRIX	5

// definitions for FLEXT_CPU
#define FLEXT_CPU_UNKNOWN	0
#define FLEXT_CPU_INTEL 1
#define FLEXT_CPU_PPC 	2
#define FLEXT_CPU_MIPS	3


// Old definitions
#if defined(MAXMSP)
	#define FLEXT_SYS FLEXT_SYS_MAX
	#undef MAXMSP
#elif defined(PD)
	#define FLEXT_SYS FLEXT_SYS_PD
	#undef PD
	#undef NT
#endif

// Definition of supported real-time systems
#if FLEXT_SYS == FLEXT_SYS_MAX
#elif FLEXT_SYS == FLEXT_SYS_PD
	#define PD
#else
	#error "System must be defined by either FLEXT_SYS_MAX or FLEXT_SYS_PD"
#endif

// Definition of OS/CPU
#if defined(_MSC_VER)
	// Microsoft C++
	
#elif defined(__MWERKS__)
	// Metrowerks CodeWarrior

	#if defined(__INTEL__)
		#define FLEXT_CPU FLEXT_CPU_INTEL
	#elif defined(__POWERPC__)
		#define FLEXT_CPU FLEXT_CPU_PPC
	#elif defined(__MIPS__)
		#define FLEXT_CPU FLEXT_CPU_MIPS
	#elif defined(__POWERPC__)
		#define FLEXT_CPU FLEXT_CPU_UNKNOWN
	#endif

#elif defined(__GNUG__)
	// GNU C++

#endif


#if FLEXT_OS == FLEXT_OS_WIN
	#if FLEXT_SYS == FLEXT_SYS_MAX
		#define WIN_VERSION 1
	#elif FLEXT_SYS == FLEXT_SYS_PD
		#define NT
	#endif
#elif FLEXT_OS == FLEXT_OS_LINUX
	#if FLEXT_SYS == FLEXT_SYS_MAX
		#error "Flext SYS/OS combination unknown"
	#endif
#elif FLEXT_OS == FLEXT_OS_IRIX
	#if FLEXT_SYS == FLEXT_SYS_MAX
		#error "Flext SYS/OS combination unknown"
	#endif
#elif FLEXT_OS == FLEXT_OS_MACOS9
	#if FLEXT_SYS == FLEXT_SYS_PD
		#error "Flext SYS/OS combination unknown"
	#endif
#elif FLEXT_OS == FLEXT_OS_MACOSX
#else
	#error "Operating system could not be determined"
#endif


#endif
