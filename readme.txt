flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

This package seeks to encourage the development of open source software
for the pd and Max/MSP platforms.

Donations for further development of the package are highly appreciated.

----------------------------------------------------------------------------

Package files:
- readme.txt: this one
- gpl.txt,license.txt: GPL license stuff
- flext.h: main include file
- flstdc.h: Basic definitions in classic C - some common vocabulary for the different platforms
- flbase.h,flbase.cpp: C++ interface
- flclass.h,flext.cpp: actual base classes for message (flext_base) and dsp (flext_dsp) processing
- fldsp.h,fldsp.cpp: code for signal externals
- flthr.h,flthr.cpp: code for threaded methods
- flsupport.h,flsupport.cpp: various flext support functions and classes
- flatom.cpp flatom_pr.cpp: code for functions dealing with lists of atoms
- flutil.cpp: additional utility functions
- flxlet.cpp: code for variable inlet/outlet stuff
- flattr.cpp: code for attribute stuff
- flinternals.h: internal definitions for flext library
- flbuf.cpp: buffer object handling for base classes
- fllib.cpp: code for handling external libraries in MaxMSP
- fldefs.h: definitions for internal flext use
- flcwmax.h: trivial prefix header file for Max/MSP CodeWarrior projects
- flmspbuffer.h: MaxMSP's inofficial buffer.h included here for conveniance

> for PD you need the pd source code (which is most likely part of the distribution)
> for Max/MSP you will also need the Max/MSP SDK (and for threading the Multiprocessing library)

> if you choose to compile with SndObj support you will need the respective library
> download from: http://www.may.ie/academic/music/musictec/SndObj/main.html

----------------------------------------------------------------------------

The package should at least compile (and is tested) with the following compilers:

pd - Windows:
-------------
For any of these compilers define "FLEXT_SYS=2".

o Microsoft Visual C++ 6: edit "config-pd-msvc.txt" & run "build-pd-msvc.bat" 

o Borland C++ 5.5 (free): edit "config-pd-bcc.txt" & run "build-pd-bcc.bat" 

o Cygwin: edit "config-pd-cygwin.txt" & run "sh build-pd-cygwin.sh" 

pd - linux:
-----------
Be sure to define "FLEXT_SYS=2".

o GCC: edit "config-pd-linux.txt" & run "sh build-pd-linux.sh" 

pd - MacOSX:
-----------
Be sure to define "FLEXT_SYS=2".

o GCC: edit "config-pd-darwin.txt" & run "sh build-pd-darwin.sh" 

Max/MSP - MacOS 9:
------------------
Be sure to define "FLEXT_SYS=1" - alternatively use the prefix file "flcwmax.h"

o Metrowerks CodeWarrior V6: edit & use the "flext.cw" project file

Max/MSP - MacOSX:
------------------
Be sure to define "FLEXT_SYS=1" - alternatively use the prefix file "flcwmax-x.h" or "flcwmax-x-thr.h" for threading support.

o Metrowerks CodeWarrior V6: edit & use the "flext.cw" project file

----------------------------------------------------------------------------

Goals/features of the package:

pros:
- better readability of code compared to straight C externals
- faster development, more robust coding
- sharing of common methods and data by using base classes
- any input to any inlet (with the exception of signal streams)
- transparent use of threads for methods
- libraries of externals in MaxMSP
- more than 3 typed creation arguments possible for MaxMSP

cons:
- introduces a small overhead to speed of message handling 
- overhead in object size (due to possibly unneeded code)

see flext.h, fldefs.h and flclass.h for the documented base definitions and classes

----------------------------------------------------------------------------

Version history:

0.4.1:
- full port for Max@OSX
- completely redesigned message and attribute handling: now hashed and much more efficient
- added some prerequisites for usage of flext as a shared library
- completed Max/MSPs inlet/outlet assist description functionality
- Max/MSP signal objects: fixed bug of reporting wrong number of inlets
- put overloaded new/delete into flext support class
- introduced "Finalize" virtual method, called after all "Init" methods
- fixed crash issue in flext_dsp when there are NO signal inlets or outlets defined
	(this is possibly only a problem for the signal2 tutorial example)
- added flext::GetType(t_atom &), flext::ZeroMem
- put the clock_delay for the message queue inside the thread-locked area
	Ok for the actual object, but PD has to be thread-safe here as well
- BACKWARDS-INCOMPATIBLE CHANGE: flext_base::m_methodmain and flext_base::m_method_ have got 
	const modifiers.... these virtual methods are used rarely (except for example in py/pyext)
- now MUCH cleaner platform identification... FLEXT_SYS,FLEXT_OS,FLEXT_CPU definitions
- also FLEXT_DEBUG for debug build
- SndObjs: virtual FreeObject routine is now called at destruction (by parent class), derived class doesn't need to call it!
- SndObjs: fixed typo (numbers of output slots was wrong) and init bug
- introduced a helper thread for launching flext threads in the background
- threads are now handled on class (as opposed to object) level
- threads can now be terminated separately
- put more flext functions into flext static class
- replaced ChangePriority by RefPriority
- fixed setting of priorities...  problems were caused by a compiler bug (MSVC 6)
- made a portable threading interface with support for threading libraries other than pthreads (FLEXT_THREADS defined as FLEXT_THR_*)
- implemented threading support with the MacOS MP thread library
- stripped the ThrMutex and ThrCond classes of non-portable (and irrelevant) functionality
- simplified "ToQueue*" and threaded "ToOut*" message queue mechanism for Max/MSP 
- deprecated FLEXT_ADDMETHOD_V and FLEXT_ADDMETHOD_A definitions which only lead to confusion

0.4.0:
- the use of the const keyword is enforced (e.g. the preferred type for symbols is now "const t_symbol *")
	- there _might_ be some problems with sensitive compilers
- introduced Max/Jitter-like attribute functionality ("@value" command line, "getvalue" get and "value" set functions)
- introduced a flext static class for general flext functions (to clean up the flext_base class)
- creation argument handling is now done by flext
	no more weird PD re-ordering of arguments 
- no more support for the Apple MPW compiler - MacOS9 is dying anyway...
- calling SetupInOut() has become obsolete 
	- flext creates all inlets/outlets by itself after the constructor has finished
	- this implies that CntIn(),CntOut() and the outlet pointers are not valid in the constructor
	- there is a virtual bool Init() function that may be used for such initialization
- completely redesigned FLEXT_NEW macros, usage of dynamic classes (in fllib.cpp)
- added ToQueue* functions - like ToOut* but messages or not directly sent (well suited for deadlock situations)
- introduced additional per-class methods and attributes (just like it ever was in Max and PD)
- fixed potentially dangerous typo in flext.cpp - (was: FLEXT_THREAD instead of FLEXT_THREADS)
- added OSX/darwin support (originally done by Adam T. Lindsay)
- SndObj interface now also available for cygwin and BCC 
- added prepend and append functions to AtomList class
- added IsNothing, SetNothing, CanbeBool and GetABool functions
- eliminated the remaining Get*Flint and Set*Flint functions 
- added/completed Is/Canbe/Get/Set for pointer atoms
- added print/scan functions for atoms
- fixed anything outlets for Max/MSP... for some strange reason this severe bug has not had severe consequences....

0.3.3:
- PD: fixed bug for DSP objects having no signal inlets
	this also enables floats into a non-signal leftmost inlet
- revisited priority stuff for detached threads
- Bind/unbind functions for flext classes (in MaxMSP only one object can be bound)
- made "t_symtype" another synonym for "t_symbol *"
- added forgotten dsp_free function for MaxMSP
- fixed forgotten __class__ member for MaxMSP libraries
- changed basic MaxMSP object to t_pxbox (ok, a bit more memory is used....)
- MaxMSP library can now be loaded from startup folder (but can't use aliases)
- if no handler is found for pure anything (just symbol, no args...) try list handler
- added interface for SndObj (http://www.may.ie/academic/music/musictec/SndObj/ )

0.3.2:
- Doxygen generated inline documentation / functional reference
- added some more functionality to the AtomAnything class
- bugfix for threaded methods with var list or anything arguments
- added threaded method for arbitrary (void *) data structs
- ThrMutex: added lock count functions Push() and Pop()
- eliminated potentially dangerous SetThing,GetThing and introduced DoBind,DoUnbind,GetBound instead
- fixed severe bug with the current threads list
- add path specification possibility for help symbols (in FLEXT_NEW*)
- use pthread_attr and DETACHED flag

0.3.1:
- added some more functionality to the AtomList class
- fixed forgotten Sleep implementation in pd@unix
- reorganized file structure and config/make procedures

0.3.0:
- added CYGWIN support

- added threaded methods along with a message queue for ToOut* functions (very unstable for MaxMSP!)
  to use threads compile flext with FLEXT_THREADS definition
- check/update function for buffer change (resize etc.)
- description text for inlets/outlets (e.g. for MaxMSPs assist function) - not fully implemented
- added buffer resize functions flext_base::buffer::Frames(int,bool)
- added some utility functions: Sleep, CopyAtom, CopyList
- added List manipulation classes: AtomList, AtomAnything
- Alias object names (simply specify with FLEXT_NEW*, separated by whitespace)
- float messages - int method is called if there is no float method
- MaxMSP: int messages - float method is called if there is no int method
- fixed type warning for class constructors with int arguments in PD
- fixed severe bug concerning symbol methods
- MaxMSP: use critical sections for message output (for more safety in overdrive mode)
- PD: default/manual definition of help file by flext_base::DefineHelp
- added GetThing/SetThing to access t_symbol's s_thing data member

- introduced FLEXT_NEW_DSP* and FLEXT_LIB_DSP* for FLEXT_NEW_TILDE* and FLEXT_LIB_TILDE* (the latter become deprecated)
- all variable argument defs (aka gimme) now have a V instead of G  (e.g. FLEXT_NEW_V)

0.2.3:
- restructured files and started usable inline documentation
- found that there was no method to output a bang?! Ugh! -> corrected
- finally eliminated awful t_flint type and all other schizophrenic flint functions (float and int exist now equally)
- now (finally) using type t_sample for sample values (should just be identical to float)
- added AddInBang/AddOutBang (same as Add*Symbol - unchecked)
- buffer class: added "bool Ok()" check function
- switched on error posting for unhandled messages
- added XletCode and AddInlets/AddOutlets for multiple Inlet/Outlet addition (max. 9 inlets/outlets)
- if float or int message is not handled explicitly then [list float/int( is tried

0.2.2:
- added xgimme argument type (supplies method handlers with symbol+gimme)
- more information on DSP system with flext_dsp object (block size,number of audio inputs and outputs)
- cleaner gcc makefile
- made NewAligned/FreeAligned static functions
- introduced FLEXT(_TILDE)_SETUP for class setup in a library setup function
- introduced external libraries for MaxMSP, changed library setup behavior (now via FLEXT_LIB_SETUP)
- included MaxMsp's inofficial buffer.h with consent by David Zicarelli
- changed dynamic casts in callback functions to static as gcc 3.0.4 has obvious bugs

0.2.1:
- fixed bug in message parsing code (anything messages were not correctly mapped to inlets)
- changed t_symtype to t_symptr (= t_symbol *), needed for method argument lists
- eliminated flint type (t_flint remains)

0.2.0:
- internal proxy objects for any non-leftmost inlets
- Max/MSP: all signal inlets can receive messages 
- method/argument parsing is done by flext - float/int are not distinguished, the first handler wins
- integrated more system functions into flext_base & eliminated superfluous #defines
- distribute list (into inlet 0) elements over inlets (right to left)
- added outlets for anythings
- defines for callback-to-method functions and method setup (FLEXT_CALLBACK*, FLEXT_ADD*)
- uses PD's or Max's memory allocation functions (for safety in Max's overdrive)
- no support for default arguments (A_DEFFLOAT and A_DEFSYMBOL).. use GIMME instead!
- better graphics update behavior for PD
- improved behavior for invalid/undefined buffers/arrays
- use MaxMSP internal z_disabled flag with flext_dsp for pausing/resuming dsp processing 
- included CHECK_TILDE, a test whether a tilde object (defined as FLEXT_TILDE_*) has a trailing ~. (debug mode only)
- changed notation of flext functions from to_out_float like to ToOutFloat like
- eliminated trivial shortcuts (F,I,V,...) for built-in types
- MaxMSP is only capable of 3 creation arguments... that should be sufficient - otherwise use GIMME
- Methods for aligned memory (NewAligned, FreeAligned)

0.1.1:
- documentation for flext.h
- more emancipation from GEM code
- virtually everything renamed
- abstraction for dsp processing
- makefile for BCC
- manual call of extern_setup or main unnecessary for single objects - only in pd libraries
- delayed buffer init (only name is set beforehand)
- loadbang also in PD
- introduced "compatibility mode" which denies platform-specific features 
- fixed severe dsp bug (symptom: "float method overwritten" warning in pd)
- fixed bug: wrong return code from buffer::Set

0.1.0: 
- max-pd 0.2 becomes flext 0.1.0
- heavy usage of unchanged GEM code


----------------------------------------------------------------------------

Notes:
- no support for default object arguments (A_DEFFLOAT, A_DEFSYMBOL) -> use variable argument lists instead

Platform specific:
- PD does not allow signal and message to go into the same inlet

Restrictions in compatibility mode:
- Max allows only 9 float/int inlets

Porting to new compilers/platforms:
- enums must be int-sized
- compiler must support bool type
- no need of C++ exceptions or RTTI (RTTI only for GUI objects)

----------------------------------------------------------------------------

TODO list:

general:
- documentation
- add log messages for debugging version
- feed assist function with in/outlet description
- MaxMSP: how to call separate help files for objects in a library?
- use PD's t_float type for floating point values (and what about t_int?)

bugs:
- PD: problems with timed buffer redrawing (takes a lot of cpu time)
- hard thread termination upon object destruction doesn't seem to work properly -> crash
- Max rounding bug ... buffer resize could be one sample less!
- flext_dsp: Max/MSP doesn't correctly report in/out channel counts

tests:
- PD: figure out what "pointer" messages do and where they occur
- some more mutexes needed for thread safety?
- buffer resize: flext_base::buffer::Frames(): should we use buffer or system sample rate?

- what about FLEXT_ADDMETHOD_V (for var arg lists) and FLEXT_ADDMETHOD_A (anythings)... nonsense?
	-> yes, these definitions obscure the meaning of _ as an indication for usage of a message tag

features:
- abstraction for clock functions
- manage running threads individually (stop, isrunning?, priority etc.)

premature thoughts:
- GUI objects



