# build class specific settings

##############################################

# default target
build: $(TARGET)

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
clean:
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

$(FLEXTLIBINST):
	if not exist $@ mkdir $@ > nul

install:: $(FLEXTINC) $(FLEXTLIBINST)

install::
	-copy $(TARGET) $(FLEXTLIBINST) > nul
	@cd $(SRCDIR)
	-for %%i in ($(HDRS)) do @copy %%i $(FLEXTINC) > nul	
