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
	-del /q $(TARGET) > nul
	-cd $(TARGETPATH)
	-del /q $(OBJS) > nul

##############################################

!ifdef SHARED
FLEXTLIBINST=$(FLEXTSHLIB)
!else
FLEXTLIBINST=$(FLEXTLIB)
!endif

$(FLEXTINC):
	if not exist $@ mkdir $@ > nul

$(FLEXTLIB):
	if not exist $@ mkdir $@ > nul

$(FLEXTLIBINST):
	if not exist $@ mkdir $@ > nul

_install_:: $(FLEXTINC) $(FLEXTLIB) $(FLEXTLIBINST)

_install_::
	copy $(TARGET) $(FLEXTLIBINST) > nul
!ifdef SHARED
# copy import library
	copy $(TARGET:.dll=.lib) $(FLEXTLIB) > nul	
!endif
# copy headers
	for %%i in ($(HDRS)) do @copy $(SRCDIR)\%%i $(FLEXTINC) > nul	
