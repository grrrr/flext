/* 

flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

*/

/*! \file flprefix.h
    \brief Try to find out the platform.
*/
 
#ifndef __FLEXT_PREFIX_H
#define __FLEXT_PREFIX_H

// --- definitions for FLEXT_SYS ---------------------
#define FLEXT_SYS_UNKNOWN	0

#ifndef FLEXT_SYS_MAX
	#define FLEXT_SYS_MAX	1
#else
	// already defined
	#undef FLEXT_SYS_MAX
	#define FLEXT_SYS_MAX	1
	#define FLEXT_SYS FLEXT_SYS_MAX
#endif

#ifndef FLEXT_SYS_PD
	#define FLEXT_SYS_PD	2
#else
	// already defined
	#undef FLEXT_SYS_PD
	#define FLEXT_SYS_PD	2
	#define FLEXT_SYS FLEXT_SYS_PD
#endif

#ifndef FLEXT_SYS_JMAX
	#define FLEXT_SYS_JMAX	3
#else
	// already defined
	#undef FLEXT_SYS_JMAX
	#define FLEXT_SYS_JMAX	3
	#define FLEXT_SYS FLEXT_SYS_JMAX
#endif

// --- definitions for FLEXT_OS ----------------------
#define FLEXT_OS_UNKNOWN	0
#define FLEXT_OS_WIN	1
#define FLEXT_OS_MAC	2  
#define FLEXT_OS_LINUX	3
#define FLEXT_OS_IRIX	4

// --- definitions for FLEXT_OS_API ---------------------
#define FLEXT_OSAPI_UNKNOWN	0
#define FLEXT_OSAPI_MAC_CLASSIC 1
#define FLEXT_OSAPI_MAC_CARBON 2
#define FLEXT_OSAPI_MAC_OSX 3

// --- definitions for FLEXT_CPU ---------------------
#define FLEXT_CPU_UNKNOWN	0
#define FLEXT_CPU_INTEL 1
#define FLEXT_CPU_PPC 	2
#define FLEXT_CPU_MIPS	3
#define FLEXT_CPU_ALPHA	4

// --- definitions for FLEXT_THREADS -----------------
#define FLEXT_THR_POSIX 1 // pthreads
#define FLEXT_THR_WIN32	2 // Win32 native
#define FLEXT_THR_MP	3 // MacOS MPThreads

// ---------------------------------------------------
// support old definitions

#ifndef FLEXT_SYS
	#if defined(MAXMSP)
		#define FLEXT_SYS FLEXT_SYS_MAX
	//	#undef MAXMSP
	#elif defined(PD)
		#define FLEXT_SYS FLEXT_SYS_PD
	//	#undef PD
	//	#undef NT
	#endif
#endif

#if defined(_DEBUG)
	#define FLEXT_DEBUG
#endif

// ---------------------------------------------------

// Definition of supported real-time systems
#if FLEXT_SYS == FLEXT_SYS_MAX
#elif FLEXT_SYS == FLEXT_SYS_PD
#elif FLEXT_SYS == FLEXT_SYS_JMAX
#else
	#error "System must be defined by either FLEXT_SYS_MAX, FLEXT_SYS_PD or FLEXT_SYS_JMAX"
#endif

// Definition of OS/CPU
#if defined(_MSC_VER)
	// Microsoft C++
	
	#ifndef FLEXT_CPU
		#if defined(_M_IX86)
			#define FLEXT_CPU FLEXT_CPU_INTEL
		#elif defined(_M_PPC)
			#define FLEXT_CPU FLEXT_CPU_PPC
		#elif defined(_M_MRX000)
			#define FLEXT_CPU FLEXT_CPU_MIPS
		#elif defined(_M_ALPHA)
			#define FLEXT_CPU FLEXT_CPU_ALPHA
		#else
			#define FLEXT_CPU FLEXT_CPU_UNKNOWN
		#endif
	#endif

	#ifndef FLEXT_OS
		#if defined(_WIN32)
			#define FLEXT_OS FLEXT_OS_WIN
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
	#endif

	#define FLEXT_OSAPI FLEXT_OSAPI_UNKNOWN

#elif defined(__BORLANDC__) 
	// Borland C++

	#ifndef FLEXT_CPU
		#define FLEXT_CPU FLEXT_CPU_INTEL
	#endif
	#ifndef FLEXT_OS
		#define FLEXT_OS FLEXT_OS_WIN
	#endif

	#define FLEXT_OSAPI FLEXT_OSAPI_UNKNOWN

#elif defined(__MWERKS__)
	// Metrowerks CodeWarrior

	#ifdef __MACH__
		// quick fix for OSX Mach-O
		#define TARGET_CPU_PPC 1
		#define TARGET_OS_MAC 1
		#define TARGET_API_MAC_OSX 1
	#else
		#ifndef __CONDITIONALMACROS__
		#include <ConditionalMacros.h>
		#endif
	#endif

	#ifndef FLEXT_CPU
		#if TARGET_CPU_X86
			#define FLEXT_CPU FLEXT_CPU_INTEL
		#elif TARGET_CPU_PPC
			#define FLEXT_CPU FLEXT_CPU_PPC
		#elif TARGET_CPU_MIPS
			#define FLEXT_CPU FLEXT_CPU_MIPS
		#elif TARGET_CPU_ALPHA
			#define FLEXT_CPU FLEXT_CPU_ALPHA
		#else
			#define FLEXT_CPU FLEXT_CPU_UNKNOWN
		#endif
	#endif

	#ifndef FLEXT_OS
		#if TARGET_OS_MAC
			#define FLEXT_OS FLEXT_OS_MAC
		#elif TARGET_OS_WIN32
			// assume Windows
			#define FLEXT_OS FLEXT_OS_WIN
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
	#endif
	
	#ifndef FLEXT_OSAPI
		#if TARGET_API_MAC_OSX
			// this has the precedence (OSX can also be Carbon, of course)
			#define FLEXT_OSAPI FLEXT_OSAPI_MAC_OSX
		#elif TARGET_API_MAC_CARBON
			#define FLEXT_OSAPI FLEXT_OSAPI_MAC_CARBON
		#else
			#define FLEXT_OSAPI FLEXT_OSAPI_UNKNOWN
		#endif
	#endif
	
	// This is important for method and attribute callbacks
	#pragma enumsalwaysint on
	// This is important for everything
	#pragma bool on

#elif defined(__GNUG__)
	// GNU C++

	#ifndef FLEXT_CPU
		#if defined(_X86_) || defined(__i386__) || defined(__i586__) || defined(__i686__)
			#define FLEXT_CPU FLEXT_CPU_INTEL
		#elif defined(__POWERPC__)
			#define FLEXT_CPU FLEXT_CPU_PPC
		#elif defined(__MIPS__)
			#define FLEXT_CPU FLEXT_CPU_MIPS
		#else
			#define FLEXT_CPU FLEXT_CPU_UNKNOWN
		#endif
	#endif

	#ifndef FLEXT_OS
		#if defined(linux) || defined(__linux__)
			#define FLEXT_OS FLEXT_OS_LINUX
		#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
			#define FLEXT_OS FLEXT_OS_WIN
		#elif defined(__APPLE__) && defined(__MACH__)
			#define FLEXT_OS FLEXT_OS_MAC
		// how about IRIX??
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
	#endif

	#if FLEXT_OS == FLEXT_OS_MAC
		#define FLEXT_OSAPI FLEXT_OSAPI_MAC_OSX
	#else
		#define FLEXT_OSAPI FLEXT_OSAPI_UNKNOWN
	#endif

#elif defined(__MRC__) && defined(MPW_CPLUS)
	// Apple MPW MrCpp
	#if __MRC__ < 0x500
		#error Apple MPW MrCpp v.5.0.0 or better compiler required
	#endif

	#ifndef FLEXT_CPU
		#if defined(__POWERPC__)
			#define FLEXT_CPU FLEXT_CPU_PPC
		#else
			#define FLEXT_CPU FLEXT_CPU_UNKNOWN
		#endif
	#endif

	#ifndef FLEXT_OS
		#if defined(macintosh)
			#define FLEXT_OS FLEXT_OS_MAC
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
	#endif

	#define FLEXT_OSAPI FLEXT_OSAPI_UNKNOWN

#endif

#if FLEXT_OS == FLEXT_OS_WIN
//	#pragma message("Compiling for Windows")

	#if FLEXT_SYS == FLEXT_SYS_MAX
//		#define WIN_VERSION 1
	#elif FLEXT_SYS == FLEXT_SYS_PD
//		#define PD
//		#define NT
	#endif
#elif FLEXT_OS == FLEXT_OS_LINUX
//	#pragma message("Compiling for Linux")

	#if FLEXT_SYS == FLEXT_SYS_PD
//		#define PD
	#else
		#error "Flext SYS/OS combination unknown"
	#endif
#elif FLEXT_OS == FLEXT_OS_IRIX
//	#pragma message("Compiling for Irix")

	#if FLEXT_SYS == FLEXT_SYS_PD
//		#define PD
	#else
		#error "Flext SYS/OS combination unknown"
	#endif
#elif FLEXT_OS == FLEXT_OS_MAC
//	#pragma message("Compiling for MacOS")

	#if FLEXT_SYS == FLEXT_SYS_PD
//		#define PD
	#endif
#else
	#error "Operating system could not be determined"
#endif

#if FLEXT_SYS == FLEXT_SYS_MAX
//	#pragma message("Compiling for Max/MSP")
#elif FLEXT_SYS == FLEXT_SYS_PD
//	#pragma message("Compiling for PD")
#endif

// set threading model
#ifdef FLEXT_THREADS
	#undef FLEXT_THREADS
	#if FLEXT_OS == FLEXT_OS_MAC && FLEXT_SYS == FLEXT_SYS_MAX
		// Max crashes with posix threads (but don't know why...)
		#define FLEXT_THREADS FLEXT_THR_MP		
	#else
		#define FLEXT_THREADS FLEXT_THR_POSIX
	#endif
#endif

#endif
