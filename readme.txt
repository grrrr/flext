flext - C++ layer for Max/MSP and pd (pure data) externals
version 0.1.2

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

This package wants to encourage the development of open source software
for the pd and Max/MSP platforms.

Donations for further development of the package are highly appreciated.

----------------------------------------------------------------------------

Package files:
- readme.txt: this one
- gpl.txt,license.txt: GPL license stuff
- flstdc.h: Basic definitions in classic C - Common vocabulary for the different platforms
- flbase.h,flbase.cpp: GEM-like C++ interface
- flext.h,flext.cpp: actual base classes for message (flext_base) and dsp (flext_dsp) processing
- flbuf.cpp: buffer object handling for base classes
- flcwmax.h: trivial prefix header file for Max/MSP CodeWarrior projects


----------------------------------------------------------------------------

The package should at least compile (and is tested) with the following compilers:

- pd - Windows:
o Borland C++ 5.5 (free): makefile.bcc - makefile is no real make but works (you have to edit the pd path!)
o Microsoft Visual C++ 6: flext.dsp - project file (you have to edit the pd path in the project properties/C-C++/preprocessor tab!)

- pd - linux:
o GCC: makefile.pd-linux - you have to edit the pd path!

- Max/MSP - MacOS:
o Metrowerks CodeWarrior V6: flext project - CodeWarrior project file



for Max/MSP you will also need the Max/MSP SDK
for PD you need the pd source code (which is most likely part of the distribution)

----------------------------------------------------------------------------

Goals/features of the package:

pros:
- better readability of code compared to straight C externals
- faster development, more robust coding
- sharing of common methods and data by using base classes

cons:
- introduces a small overhead to speed (esp. to messages)


see flext.h for the documented base classes

----------------------------------------------------------------------------

Version history:

0.1.2:
- defines for callback-to-method functions and method setup (FLEXT_CALLBACK*, FLEXT_ADD*)
- deprecated support for default arguments (A_DEFFLOAT and A_DEFSYMBOL).. use GIMME instead!
- flext now uses PD's or Max's memory allocation functions (for safety in Max's overdrive)
- added outlets for anythings
- better graphics update behavior for PD
- proxy object for any non-leftmost inlets (still lacking consistency!)
- changed "enable" message for DSP objects to "dspon" (enable is reserved in Max/MSP)

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
- max-pd 0.2 became flext 0.1.0
- heavy usage of unchanged GEM code


----------------------------------------------------------------------------

Notes:
- no support for default object arguments (A_DEFFLOAT, A_DEFSYMBOL) -> use GIMME instead

Platform specific:
- Max does not allow external libraries -> only one FLEXT_NEW* or FLEXT_TILDE_NEW* in a project 
- In PD loadbang is not triggered upon object creation

Restrictions in compatibility mode:
- Max allows only 9 float/int inlets
- Max allows no additional symbol inlets



----------------------------------------------------------------------------

TODO list:

general:
- documentation
- add log messages for debugging version
- add #defines to exclude unwanted features
- exchange more preprocessor definitions for C++ base class code (esp. pd and max calls)
- cleaner makefile for BCC
- makefile for MS VC++ 6
- should we use a namespace?
- Apple MPW project
- where to put flext source/lib in linux: /usr/local/lib,/usr/local/include ?

bugs:
none known

tests:
- test loadbang upon object creation in Max/MSP

features:
- use anything inlets from system - do method parsing in flext_base?
- abstraction for parsing argument lists
- abstraction for creating lists and anythings
- simulate loadbang on object creation for PD
- abstraction for clock functions
- message queue? (for multithreaded operation)
- sending messages to own inlet (passing computation to other patch objects - message queue?)

premature thoughts:
- explore jMax interface style
- interface for supercollider, csound?
- interface for VST, DX, ...?
- interface for scripting language modules? (python?)

