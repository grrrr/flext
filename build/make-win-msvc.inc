!ifdef $(MAX)
# Max/MSP
!include config-pd-msvc.txt
!include makefile-msvc.txt
!include ..\flext\build\make-win-max-msvc.inc

!elseifdef $(PD)
# PD
!include config-pd-msvc.txt
!include makefile-msvc.txt
!include ..\flext\build\make-win-pd-msvc.inc

!else
!error Platform not supported
!endif

# all the source files from the package
!include make-files.txt

# now build
!include ..\flext\build\make-win-gen-msvc.inc

