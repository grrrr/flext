# Microsoft Developer Studio Project File - Name="flext" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=flext - Win32 Threads Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "flext.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "flext.mak" CFG="flext - Win32 Threads Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "flext - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "flext - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "flext - Win32 Threads Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "flext - Win32 Threads Release" (basierend auf  "Win32 (x86) Static Library")
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
# PROP Output_Dir "pd-msvc\s"
# PROP Intermediate_Dir "pd-msvc\s"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /O2 /I "c:\programme\audio\pd\src" /I "f:\prog\packs\sndobj\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D FLEXT_SYS=2 /YX"flext.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"pd-msvc/flext-pdwin.lib"

!ELSEIF  "$(CFG)" == "flext - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "pd-msvc\sd"
# PROP Intermediate_Dir "pd-msvc\sd"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /ZI /Od /I "c:\programme\audio\pd\src" /I "f:\prog\packs\sndobj\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D FLEXT_SYS=2 /FR /YX"flext.h" /FD /GZ /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"pd-msvc/flext_d-pdwin.lib"

!ELSEIF  "$(CFG)" == "flext - Win32 Threads Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "flext___Win32_Threads_Debug"
# PROP BASE Intermediate_Dir "flext___Win32_Threads_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "pd-msvc\td"
# PROP Intermediate_Dir "pd-msvc\td"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GR /ZI /Od /I "c:\programme\audio\pd\src" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "PD" /D "NT" /D "FLEXT_THREADS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /ZI /Od /I "c:\programme\audio\pd\src" /I "f:\prog\packs\sndobj\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "FLEXT_SYS_PD" /D "FLEXT_THREADS" /FR /YX"flext.h" /FD /GZ /c
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"msvc-debug\flext-pdwin.lib"
# ADD LIB32 /nologo /out:"pd-msvc/flext_td-pdwin.lib"

!ELSEIF  "$(CFG)" == "flext - Win32 Threads Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "flext___Win32_Threads_Release"
# PROP BASE Intermediate_Dir "flext___Win32_Threads_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "pd-msvc\t"
# PROP Intermediate_Dir "pd-msvc\t"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GR /O2 /I "c:\programme\audio\pd\src" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "PD" /D "NT" /D "FLEXT_THREADS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /O2 /I "c:\programme\audio\pd\src" /I "f:\prog\packs\sndobj\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D FLEXT_SYS=2 /D "FLEXT_THREADS" /YX"flext.h" /FD /c
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"msvc/flext-pdwin.lib"
# ADD LIB32 /nologo /out:"pd-msvc/flext_t-pdwin.lib"

!ENDIF 

# Begin Target

# Name "flext - Win32 Release"
# Name "flext - Win32 Debug"
# Name "flext - Win32 Threads Debug"
# Name "flext - Win32 Threads Release"
# Begin Group "doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\fldoxygen.h
# End Source File
# Begin Source File

SOURCE=".\make-files.txt"
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Group
# Begin Group "Atom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\flatom.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flatom_app.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flatom_part.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flatom_pr.cpp
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\flbase.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flbase.h
# End Source File
# Begin Source File

SOURCE=.\source\flclass.h
# End Source File
# Begin Source File

SOURCE=.\source\fldefs.h
# End Source File
# Begin Source File

SOURCE=.\source\flext.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flext.h
# End Source File
# Begin Source File

SOURCE=.\source\flinternal.h
# End Source File
# Begin Source File

SOURCE=.\source\fllib.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flprefix.h
# End Source File
# Begin Source File

SOURCE=.\source\flstdc.h
# End Source File
# Begin Source File

SOURCE=.\source\flthr.cpp
# End Source File
# End Group
# Begin Group "IO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\flattr.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flitem.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flmeth.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flout.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flproxy.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flxlet.cpp
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\flbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flmspbuffer.h
# End Source File
# Begin Source File

SOURCE=.\source\flsupport.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flsupport.h
# End Source File
# Begin Source File

SOURCE=.\source\flutil.cpp
# End Source File
# End Group
# Begin Group "Dsp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\fldsp.cpp
# End Source File
# Begin Source File

SOURCE=.\source\fldsp.h
# End Source File
# End Group
# Begin Group "SndObj"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\source\flsndobj.cpp
# End Source File
# Begin Source File

SOURCE=.\source\flsndobj.h
# End Source File
# End Group
# End Target
# End Project
