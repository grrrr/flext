# build class specific settings

##############################################

# default target
_build_: $(TARGET)

$(OUTPATH):
	if not exist $@ mkdir $@ > nul

$(TARGETPATH): $(OUTPATH)
	if not exist $@ mkdir $@ > nul

{$(SRCDIR)}.cpp{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@

{$(SRCDIR)}.c{}.obj:
	cl /c /LD $(CFLAGS) $(DEFS) $(INCPATH) $** /Fo$(TARGETPATH)\$@


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

_install_:
# create folders
	@if not exist $(FLEXTINC) mkdir $(FLEXTINC) > nul
	@if not exist $(FLEXTLIB) mkdir $(FLEXTLIB) > nul
	@if not exist $(FLEXTLIBINST) mkdir $(FLEXTLIBINST) > nul

	copy $(TARGET) $(FLEXTLIBINST) > nul
!ifdef SHARED
# copy import library
	copy $(TARGET:.dll=.lib) $(FLEXTLIB) > nul	
!endif
# copy headers
	for %%i in ($(HDRS)) do @copy $(SRCDIR)\%%i $(FLEXTINC) > nul
