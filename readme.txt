flext - C++ layer for Max/MSP and pd (pure data) externals

Copyright (c) 2001-2003 Thomas Grill (xovo@gmx.net)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.  

This package seeks to encourage the development of open source software
for the pd and Max/MSP platforms.

Donations for further development of the package are highly appreciated.

----------------------------------------------------------------------------

o for PD you need the pd source code (which is most likely part of the distribution)
  download from: http://www-crca.ucsd.edu/~msp/software.html

o for Max/MSP you will also need the Max/MSP SDK for OS9 (http://www.cycling74.com/products/dlmaxmsp.html) 
  or OSX (http://www.cycling74.com/products/dlxmax.html)
  and for threading the Multiprocessing library (download at http://developer.apple.com/sdk/)

o if you choose to compile with SndObj support you will need the respective library
  download from: http://www.may.ie/academic/music/musictec/SndObj/main.html

o if you choose to compile with STK support you will need the respective package and build a library
  download from: http://ccrma-www.stanford.edu/software/stk/
  For linking it may preferable to use a library of all the STK objects.
  Under linux you can create one from the STK directory with something like
  "g++ -c -pipe -I include -D __LINUX_OSS__ src/*.cpp && ar r libstk.a *.o && rm -f *.o"

----------------------------------------------------------------------------


BUILDING & INSTALLING:
======================


pd - Windows:
-------------
o Microsoft Visual C++ 6: edit "config-pd-msvc.txt" & run "build-pd-msvc.bat" 

o Borland C++ 5.5 (free): edit "config-pd-bcc.txt" & run "build-pd-bcc.bat" 

o Cygwin: edit "config-pd-cygwin.txt" & run "sh build-pd-cygwin.sh" 
	additional settings (e.g. target processor, compiler flags) can be made in makefile.pd-cygwin

With your project using flext, be sure to define "FLEXT_SYS=2".


pd - linux:
-----------
o GCC: edit "config-pd-linux.txt" & run "sh build-pd-linux.sh" 
	additional settings (e.g. target processor, compiler flags) can be made in makefile.pd-linux

With your project using flext, be sure to define "FLEXT_SYS=2".


pd - MacOSX:
-----------
o GCC: edit "config-pd-darwin.txt" & run "sh build-pd-darwin.sh" 
	additional settings (e.g. target processor, compiler flags) can be made in makefile.pd-darwin

With your project using flext, be sure to define "FLEXT_SYS=2".


Max/MSP - MacOS 9:
------------------
o Metrowerks CodeWarrior: edit & use the "flext.cw" project file

You must have the following "Source Trees" defined:
"flext" - Pointing to the flext main directory
"Cycling74" - Pointing to the Cycling 74 SDK
"MP SDK" - Pointing to the Multiprocessing SDK (for threading support)


With your project using flext, be sure to define "FLEXT_SYS=1" 
- alternatively use the prefix file "flcwmax.h" or "flcwmax-thr.h" for threading support.


Max/MSP - MacOSX:
------------------
o Metrowerks CodeWarrior: edit & use the "flext.cw" project file

You must have the following "Source Trees" defined:
"OS X Volume" - Pointing to your OSX boot drive
"flext" - Pointing to the flext main directory
"Cycling74 OSX" - Pointing to the Cycling 74 SDK for xmax
"MP SDK" - Pointing to the Multiprocessing SDK (for threading support)


With your project using flext, be sure to define "FLEXT_SYS=1" 
- alternatively use the prefix file "flcwmax-x.h" or "flcwmax-x-thr.h" for threading support.


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

