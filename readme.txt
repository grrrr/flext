flext - C++ layer for Max/MSP and Pd (Pure Data) externals

Copyright (c) 2001-2023 Thomas Grill (gr@grrrr.org)
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "license.txt," in this distribution.

This package seeks to encourage the development of open source software
for the pd and Max/MSP platforms.

Donations for further development of the package are highly appreciated.
https://www.paypal.com/xclick/business=gr%40grrrr.org&item_name=flext&no_note=1&tax=0&currency_code=EUR

----------------------------------------------------------------------------

Abstract:

flext seeks to represent a uniform programming interface for extending the most common
modular real-time audio systems Max/MSP and Pure Data (Pd) with external modules, or
short externals. These modules provide a way to tailor such a system for oneís
special needs and supply additional functionality.

Source code based on flext is able to exploit most common features of the
respective real-time framework while staying completely independent of the
actual host system and platform (hardware and operating system).

flext currently supports development for Pd under Linux, Windows and OSX as well as
Max/MSP under OS9, OSX and Windows with various programming environments.

----------------------------------------------------------------------------

Goals/features of the package:

pros:
- better readability of code compared to straight C externals
- faster development, more robust coding
- sharing of common methods and data by using base classes
- any input to any inlet (with the exception of signal streams)
- transparent use of threads for methods
- libraries of externals in Max/MSP
- more than 3 typed creation arguments possible for Max/MSP

cons:
- introduces a small overhead to speed of message and signal handling
- larger memory footprint

----------------------------------------------------------------------------

Prerequisites:

--- Pd ---
    You need the pd source code which is most likely part of the distribution.
    Otherwise download from: http://msp.ucsd.edu/software.html

--- Max/MSP ---
    You will need the latest Max/MSP SDK: https://cycling74.com/downloads/sdk

--- SndObj ---
    If you choose to compile with SndObj support you will need the respective library
    download from: http://www.may.ie/academic/music/musictec/SndObj/main.html

--- STK ---
    If you choose to compile with STK support you will need the respective package
    and build a library.
    download from: https://ccrma.stanford.edu/software/stk/
    For linking it may preferable to use a library of all the STK objects.

    Under linux you can create one from the STK directory with something like
    > g++ -c -pipe -I include -D __LINUX_OSS__ src/*.cpp && ar r libstk.a *.o && rm -f *.o

    Under Windows you can build a static STK library with the following commands:
    > cl src/*.c* /MT /D__OS_WINDOWS__ /EHsc /Ox /Iinclude /I../pthreads/include /c
    > lib *.obj /out:stk.lib
    Please note, that you have to have pthreads installed (../pthreads points to it in the command)
    Also, the resulting stk.lib will be a multithreaded build, linked to a static C library.
    Consequently you should also use multithreaded flext for your flext-based external.

----------------------------------------------------------------------------

Building and installing of flext and flext-based externals:

    See the build.txt document

----------------------------------------------------------------------------

Various notes / limitations / bug list:

    Read the notes.txt document

----------------------------------------------------------------------------

History of changes:

    Read the changes.txt document
