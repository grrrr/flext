# Microsoft Developer Studio Project File - Name="flext" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=flext - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "flext.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "flext.mak" CFG="flext - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "flext - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "flext - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "max/flext"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "flext - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "msvc"
# PROP Intermediate_Dir "msvc"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "c:\programme\audio\pd\src" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "PD" /D "NT" /YX /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"msvc/flext-pdwin.lib"

!ELSEIF  "$(CFG)" == "flext - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "msvc-debug"
# PROP Intermediate_Dir "msvc-debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "c:\programme\audio\pd\src" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "PD" /D "NT" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"msvc-debug\flext-pdwin.lib"

!ENDIF 

# Begin Target

# Name "flext - Win32 Release"
# Name "flext - Win32 Debug"
# Begin Group "doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\flbase.cpp
# End Source File
# Begin Source File

SOURCE=.\flbase.h
# End Source File
# Begin Source File

SOURCE=.\flbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\flcwmax.h
# End Source File
# Begin Source File

SOURCE=.\flext.cpp
# End Source File
# Begin Source File

SOURCE=.\flext.h
# End Source File
# Begin Source File

SOURCE=.\flstdc.c
# End Source File
# Begin Source File

SOURCE=.\flstdc.h
# End Source File
# End Target
# End Project
