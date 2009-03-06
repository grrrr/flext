# build class specific settings

!ifdef SHARED

!ifdef SNDOBJ
LIBS=$(LIBS) $(SNDOBJ)\lib\SndObj.lib
SRCS=$(SRCS) $(SRCS_SNDOBJ)
!endif

!ifdef STK
LIBS=$(LIBS) $(STK)\stk.lib
SRCS=$(SRCS) $(SRCS_STKOBJ)
!endif

!endif

##############################################

# default target
_build_: $(TARGET)

$(OUTPATH):
	if not exist $@ mkdir $@ > nul

$(TARGETPATH): $(OUTPATH)
	if not exist $@ mkdir $@ > nul

{$(SRCDIR)}.cpp{}.obj:
	cl /c $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@

{$(SRCDIR)}.c{}.obj:
	cl /c $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@


$(TARGET):: $(TARGETPATH)

$(TARGET):: $(OBJS)
	@cd $(TARGETPATH)
!ifdef SHARED
	link /DLL $(LDFLAGS) $(LIBPATH) $(OBJS) $(LIBS) /out:$(@F)
!else
	lib /OUT:$(@F) $(OBJS)
!endif
	@cd ..\..

##############################################

# remove build
_clean_:
	-del /q $(TARGETPATH)\*.* > nul
	-rmdir /q $(TARGETPATH) > nul

##############################################

!ifdef SHARED
FLEXTLIBINST=$(FLEXTSHLIB)
!else
FLEXTLIBINST=$(FLEXTLIB)
!endif

TMPFILE=$(TARGETPATH)\flext_install_filelist.tmp

_install_:
# create folders
#	-@if not exist $(FLEXTINC) mkdir $(FLEXTINC) > nul
	-@if not exist $(FLEXTLIB) mkdir $(FLEXTLIB) > nul
	-@if not exist $(FLEXTLIBINST) mkdir $(FLEXTLIBINST) > nul

	-copy $(TARGET) $(FLEXTLIBINST) > nul
!ifdef SHARED
# copy import library
	-copy $(TARGET:.dll=.lib) $(FLEXTLIB) > nul	
!endif
# copy headers

	-@del $(TMPFILE) 2>nul
	-@for %%i in ($(HDRS)) do @echo %%i >> $(TMPFILE)
	-@for /f "tokens=1,2 delims=/" %%i in ($(TMPFILE)) do @if "%%j" == "" ( copy $(SRCDIR)\%%i $(FLEXTINC) > nul ) else ( mkdir $(FLEXTINC)\%%i 2>nul & copy $(SRCDIR)\%%i\%%j $(FLEXTINC)\%%i > nul )
	-@del $(TMPFILE) 2>nul

#!ifdef STK
#	for %%i in ($(HDRS_STK:/=\)) do @copy $(SRCDIR)\%%i $(FLEXTINC) > nul
#!endif
#!ifdef SNDOBJ
#	for %%i in ($(HDRS_SNDOBJ:/=\)) do @copy $(SRCDIR)\%%i $(FLEXTINC) > nul
#!endif
