flext - Compatibility library for Max/MSP and pd (pure data) externals

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

----------------------------------------------------------------------------

TODO list:
- Documentation
- Trade preprocessor definitions for C++ base class code
- cleaner makefiles
- better function names
- abstraction for parsing GIMME args
- abstraction for creating lists
+ clarify: what is Max/MSP behavior for floats into signal inlets? -> SEEMS TO ACT LIKE PD

