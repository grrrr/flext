flext - Compatibility library for Max/MSP and pd (pure data) externals

The package should at least compile (and is tested) with the following compilers:

- pd - Windows:
o Borland C++ 5.5 (free): ok - makefile is no real make but works
o Microsoft Visual C++ 6: no makefile yet

- pd - linux:
o GCC: no makefile yet

- Max/MSP - MacOS:
o Metrowerks CodeWarrior V6: works just perfectly


Package files:
- readme.txt: this one
- gpl.txt,license.txt: GPL license stuff
- flext.h: Basic definitions in classic C - Common vocabulary for the different platforms
- _cpp.h,_cpp.cpp: GEM-like C++ interface
- _cppext.h,_cppext.cpp: actual base classes for message (flext_base) and dsp (flext_dsp) processing
- buffer.cpp: buffer object handling for base classes
- cw-max.h: trivial prefix header file for Max/MSP CodeWarrior projects


TODO list:
- Documentation
- cleaner makefiles
- better function names
- abstraction for creating lists

