@set build=%~dp0

nmake -f %build%nmake.mak PLATFORM=%1 RTSYS=%2 COMPILER=msvc BUILDPATH=%build% %3 %4 %5 %6 %7 %8 %9
