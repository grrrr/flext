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

// --- definitions for FLEXT_OS ----------------------
#define FLEXT_OS_UNKNOWN	0
#define FLEXT_OS_WIN	1
#define FLEXT_OS_MACOS	2  
#define FLEXT_OS_LINUX	3
#define FLEXT_OS_IRIX	4

// --- definitions for FLEXT_CPU ---------------------
#define FLEXT_CPU_UNKNOWN	0
#define FLEXT_CPU_INTEL 1
#define FLEXT_CPU_PPC 	2
#define FLEXT_CPU_MIPS	3
#define FLEXT_CPU_ALPHA	4

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
#else
	#error "System must be defined by either FLEXT_SYS_MAX or FLEXT_SYS_PD"
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

#elif defined(__BORLANDC__) 
	// Borland C++

	#ifndef FLEXT_CPU
		#define FLEXT_CPU FLEXT_CPU_INTEL
	#endif
	#ifndef FLEXT_OS
		#define FLEXT_OS FLEXT_OS_WIN
	#endif

#elif defined(__MWERKS__)
	// Metrowerks CodeWarrior

	#ifndef FLEXT_CPU
		#if defined(__INTEL__)
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
		#if defined(macintosh)
			#define FLEXT_OS FLEXT_OS_MACOS
		#elif defined(__INTEL__)
			// assume Windows
			#define FLEXT_OS FLEXT_OS_WIN
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
	#endif

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
			#define FLEXT_OS FLEXT_OS_MACOS
		// how about IRIX??
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
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
			#define FLEXT_OS FLEXT_OS_MACOS
		#else
			#define FLEXT_OS FLEXT_OS_UNKNOWN
		#endif
	#endif

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
#elif FLEXT_OS == FLEXT_OS_MACOS
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

#endif
