@set flext=%~dp0

@rem Arguments:
@rem %1 - platform (win/lnx/mac)
@rem %2 - system (pd/max)
@rem %3 - compiler (msvc/gcc/mingw/cygwin/bcc/icc)
@rem %4 - target (build/clean/install)

@set platform=%1
@set rtsys=%2
@set compiler=%3
@set target=%4

@rem --- The subbatch knowns which make utility to use ---
@set subbatch=%flext%buildsys\build-%compiler%.bat

@if "%platform%"=="" goto syntax
@if "%rtsys%"=="" goto syntax
@if "%compiler%"=="" goto syntax

@if not exist %subbatch% goto syntax

@call %subpatch% %subbatch% %platform% %rtsys% %target% %5 %6 %7 %8 %9

@goto end

@rem -----------------------------------------
:syntax

@echo .
@echo SYNTAX: build [platform] [system] [compiler] {target}
@echo platform ... win / lnx / mac
@echo system   ... pd / max
@echo compiler ... msvc / gcc / mingw / cygwin / bcc / icc
@echo target   ... all (default) / clean / install
@echo .
@echo Please make sure that your make program and compiler can be accessed with the
@echo system path and that all relevant environment variables are properly set.

:end
