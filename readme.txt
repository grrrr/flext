flext - C++ layer for Max/MSP and pd (pure data) externals
version 0.2.3

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
- flstdc.h: Basic definitions in classic C - some common vocabulary for the different platforms
- flbase.h,flbase.cpp: GEM-like C++ interface
- flext.h,flext.cpp: actual base classes for message (flext_base) and dsp (flext_dsp) processing
- flbuf.cpp: buffer object handling for base classes
- fllib.cpp: code for handling external libraries in MaxMSP
- fldefs.h: definitions for internal flext use
- flcwmax.h: trivial prefix header file for Max/MSP CodeWarrior projects
- flmspbuffer.h: MaxMSP's inofficial buffer.h included here for conveniance

> for PD you need the pd source code (which is most likely part of the distribution)
> for Max/MSP you will also need the Max/MSP SDK and http://www.sirius.com/~cycling/objects/index.sit 

----------------------------------------------------------------------------

The package should at least compile (and is tested) with the following compilers:

pd - Windows:
-------------
o Borland C++ 5.5 (free): edit & run "make -f makefile.bcc" 

o Microsoft Visual C++ 6: use the "flext.dsp" project file  
> you have to edit the pd path in the project properties/C-C++/preprocessor tab!

pd - linux:
-----------
o GCC: edit & run "make -f makefile.pd-linux" and as root "make -f makefile.pd-linux install"

Max/MSP - MacOS:
----------------
o Metrowerks CodeWarrior V6: edit & use the "flext.cw" project file

o Apple MPW-PR: edit & use the "flext.mpw" makefile

----------------------------------------------------------------------------

Goals/features of the package:

pros:
- better readability of code compared to straight C externals
- faster development, more robust coding
- sharing of common methods and data by using base classes
- any input to any inlet (with the exception of signal streams)
- libraries of externals in MaxMSP

cons:
- introduces a small overhead to speed of message handling 
- overhead in object size (due to possibly unneeded code)


see flext.h for the documented base classes

----------------------------------------------------------------------------

Version history:

0.2.3:
- restructured files and started usable inline documentation
- found that there was no means to output a bang?! Ugh! -> corrected

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
- no support for default object arguments (A_DEFFLOAT, A_DEFSYMBOL) -> use GIMME instead

Platform specific:
- PD does not allow signal and message to go into the same inlet

Restrictions in compatibility mode:
- Max allows only 9 float/int inlets
- Max allows only 3 typed creation arguments -> use GIMME for more

Porting to new platforms:
- enums must be int-sized
- compiler must support RTTI and bool type

----------------------------------------------------------------------------

TODO list:

general:
- documentation
- add log messages for debugging version
- exchange more preprocessor definitions for C++ base class code (esp. pd and max calls)
- should we use a namespace?
- where to put flext source/lib in linux: /usr/local/lib,/usr/local/include ?
- clean up headers (eliminate flstdc.h)

bugs:
- PD: problems with timed buffer redrawing (takes a lot of cpu time)
- No knowledge about external resizing of a buffer! (how to handle that?)

tests:
- PD: figure out what "pointer" messages do and where they occur

features:
- abstraction for parsing argument lists
- abstraction for creating lists and anythings
- abstraction for clock functions
- multithreaded operation: detached method handling, message queue
- sending messages to own inlet (passing computation to other patch objects - message queue?)
- adding/removing methods anytime

premature thoughts:
- explore jMax interface style
- interface for supercollider, csound?
- interface for VST, DX, ...?
- interface for scripting language modules? (python?)





