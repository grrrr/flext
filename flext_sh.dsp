# Microsoft Developer Studio Project File - Name="flext_sh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=flext_sh - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "flext_sh.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "flext_sh.mak" CFG="flext_sh - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "flext_sh - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "flext_sh - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "flext_sh"
# PROP Scc_LocalPath "..\flext"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "flext_sh - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "pd-msvc/dr"
# PROP Intermediate_Dir "pd-msvc/dr"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FLEXT_SH_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "c:\programme\audio\pd\src" /I "f:\prog\audio\sndobj\include" /I "f:\prog\audio\stk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "FLEXT_DLL" /D FLEXT_SYS=2 /D "FLEXT_SHARED" /D "FLEXT_THREADS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "NDEBUG"
# ADD RSC /l 0xc07 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 pd.lib pthreadVC.lib kernel32.lib user32.lib pd.lib sndobj.lib stk.lib /nologo /dll /machine:I386 /out:"../flext/pd-msvc/flext.dll" /libpath:"f:\prog\audio\sndobj\lib" /libpath:"f:\prog\audio\stk\lib"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "flext_sh - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "pd-msvc/dd"
# PROP Intermediate_Dir "pd-msvc/dd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FLEXT_SH_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /I "c:\programme\audio\pd\src" /I "f:\prog\audio\sndobj\include" /I "f:\prog\audio\stk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FLEXT_DLL" /D "FLEXT_SHARED" /D FLEXT_SYS=2 /D "FLEXT_THREADS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc07 /d "_DEBUG"
# ADD RSC /l 0xc07 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib pd.lib pthreadVC.lib sndobj.lib stk_d.lib /nologo /dll /debug /machine:I386 /out:"../flext/pd-msvc/flext_d.dll" /pdbtype:sept /libpath:"f:\prog\audio\sndobj\lib" /libpath:"f:\prog\audio\stk\lib"

!ENDIF 

# Begin Target

# Name "flext_sh - Win32 Release"
# Name "flext_sh - Win32 Debug"
# Begin Group "doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\changes.txt
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldoxygen.h
# End Source File
# Begin Source File

SOURCE="..\flext\make-files.txt"
# End Source File
# Begin Source File

SOURCE=..\flext\readme.txt
# End Source File
# End Group
# Begin Group "Atom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\flatom.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flatom_app.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flatom_part.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flatom_pr.cpp
# End Source File
# End Group
# Begin Group "Base"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\flbase.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flbase.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\flclass.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\flext.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flext.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\flinternal.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fllib.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flprefix.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\flstdc.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\flthr.cpp
# End Source File
# End Group
# Begin Group "IO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\flattr.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flbind.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flitem.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flmeth.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flmsg.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flout.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flproxy.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flqueue.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flxlet.cpp
# End Source File
# End Group
# Begin Group "Util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\flbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flmspbuffer.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\flsimd.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flsupport.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flsupport.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fltimer.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\flutil.cpp
# End Source File
# End Group
# Begin Group "Dsp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\fldsp.cpp
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldsp.h
# End Source File
# End Group
# Begin Group "SndObj"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\flsndobj.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\flext\source\flsndobj.h
# End Source File
# End Group
# Begin Group "STK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\flstk.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\flext\source\flstk.h
# End Source File
# End Group
# Begin Group "Defs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\flext\source\fldefs.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_attradd.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_attrcb.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_attrvar.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_hdr.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_methadd.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_methbind.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_methcall.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_methcb.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_meththr.h
# End Source File
# Begin Source File

SOURCE=..\flext\source\fldefs_setup.h
# End Source File
# End Group
# End Target
# End Project
