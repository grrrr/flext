# build class specific settings

##############################################

# default target
_build_: $(TARGET) 

$(OUTPATH):
	if not exist $@ mkdir $@ > nul

$(TARGETPATH): $(OUTPATH)
	if not exist $@ mkdir $@ > nul

{$(SRCDIR)}.cpp{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(TARGETPATH) $<

{$(SRCDIR)}.c{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(TARGETPATH) $<


$(TARGETPATH)\pd.lib : $(PDPATH)\bin\pd.dll
	implib -a $< $**

$(TARGETPATH)\pthreadVC.lib : $(PDPATH)\bin\pthreadVC.dll
	implib -a $< $**

PDTARGETS=$(TARGETPATH)\pd.lib $(TARGETPATH)\pthreadVC.lib


$(TARGET) :: $(TARGETPATH) $(PDTARGETS)

$(TARGET) :: $(OBJS)
	@cd $(TARGETPATH)
!ifdef SHARED
	ilink32 $(LDFLAGS) $(LIBPATH) -L. $(OBJS) ,$(<F),,$(LIBS)
	implib -a $(<B).lib $(<F)
!else
	-del $(<F)
	tlib "$(<F)" +$(OBJS: = +)
!endif
	@cd ..\..

##############################################

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

!if "$(FLEXTINC)" != "$(FLEXTLIB)"
$(FLEXTLIB):
	if not exist $@ mkdir $@ > nul

_install_:: $(FLEXTLIB)
!endif

!if "$(FLEXTLIBINST)" != "$(FLEXTLIB)" && "$(FLEXTLIBINST)" != "$(FLEXTINC)"
$(FLEXTLIBINST):
	if not exist $@ mkdir $@ > nul

_install_:: $(FLEXTLIBINST)
!endif

_install_:: $(FLEXTINC)
	copy $(TARGET) $(FLEXTLIBINST) > nul
!ifdef SHARED
# copy import library
	copy $(TARGET:.dll=.lib) $(FLEXTLIB) > nul	
!endif
# copy headers
	for %%i in ($(HDRS)) do @copy $(SRCDIR)\%%i $(FLEXTINC) > nul	
# copy import libraries
	for %%i in ($(PDTARGETS)) do @copy %%i $(FLEXTLIBINST) > nul	
