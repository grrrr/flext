flext - Compatibility library for Max/MSP and pd (pure data) externals
version 0.1.1

Copyright (c) 2001,2002 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  


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
o Borland C++ 5.5 (free): ok - makefile is no real make but works
o Microsoft Visual C++ 6: no makefile yet

- pd - linux:
o GCC: no makefile yet

- Max/MSP - MacOS:
o Metrowerks CodeWarrior V6: ok



for Max/MSP you will also need the Max/MSP SDK
for PD you need the source code (which is most likely part of the distribution)

----------------------------------------------------------------------------

Features:
- better readability of code compared to straight C externals
- faster development, more robust coding
- sharing of common methods, data by using base classes


----------------------------------------------------------------------------

Version history:

0.1.1:
- more emancipation from GEM code
- virtually everything renamed
- abstraction for dsp processing
- makefile for BCC
- manual call of extern_setup or main unnecessary for single objects - only in pd libraries


0.1.0: 
- max-pd 0.2 became flext 0.1.0
- heavy usage of unchanged GEM code


----------------------------------------------------------------------------

Platform restrictions:
- Max does not allow external libraries -> only one FLEXT_NEW* or FLEXT_TILDE_NEW* in a project 

Restrictions in compatibility mode:
- Max allows only 9 float/int inlets
- Max allows no additional symbol inlets



----------------------------------------------------------------------------

TODO list:
- documentation
- add warning messages for debugging version
- exchange more preprocessor definitions for C++ base class code
- cleaner makefiles
- makefile and project for MS VC++ 6
- flext project and example template for CodeWarrior
- better function names

- prevent buffer warning message at max patcher load (wait for loadbang)
- abstraction for parsing GIMME args
- abstraction for creating lists

