@echo off

if "%1"=="" goto syntax
if "%2"=="" goto syntax
if "%3"=="" goto syntax

rem first check configuration
call ..\build.bat %1 %2 %3 config "PKGINFO="
if errorlevel 1 goto end

for /D %%i in (*) do (
	pushd %%i
	if exist package.txt (
		call ..\..\build.bat %1 %2 %3
	)
	popd
)

goto end

rem -----------------------------------------
:syntax

echo .
echo SYNTAX: build [platform] [system] [compiler]
echo platform ... win / lnx / mac
echo system   ... pd / max
echo compiler ... msvc / gcc / mingw / cygwin / bcc / icc
echo .
echo Please make sure that your make program and compiler can be accessed with the
echo system path and that all relevant environment variables are properly set.
echo .

:end
