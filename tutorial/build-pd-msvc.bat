@echo --- Building flext tutorial examples with MSVC++ ---

nmake /f makefile.pd-msvc NAME=simple1 DIR=simple1
nmake /f makefile.pd-msvc NAME=simple2 DIR=simple2
nmake /f makefile.pd-msvc NAME=simple3 DIR=simple3
nmake /f makefile.pd-msvc NAME=adv1 DIR=adv1
nmake /f makefile.pd-msvc NAME=attr1 DIR=attr1
nmake /f makefile.pd-msvc NAME=attr2 DIR=attr2
nmake /f makefile.pd-msvc NAME=signal1~ DIR=signal1
nmake /f makefile.pd-msvc NAME=signal2~ DIR=signal2
nmake /f makefile.pd-msvc NAME=thread1  DIR=thread1 
nmake /f makefile.pd-msvc NAME=thread2  DIR=thread2 
nmake /f makefile.pd-msvc NAME=lib1 DIR=lib1

