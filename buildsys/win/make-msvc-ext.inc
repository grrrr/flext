# build class specific settings

INCPATH=$(INCPATH) /I$(FLEXTINC)
LIBPATH=$(LIBPATH) /LIBPATH:$(FLEXTLIB)
LIBS=$(LIBS) $(FLEXTNAME).lib

##############################################

# default target
build: $(TARGET)

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
	link /DLL $(LDFLAGS) $(LIBPATH) $(OBJS) $(LIBS) /out:$(@F)
	@cd ..\..

##############################################
# remove build

clean:
	-del /q $(TARGET) > nul
	-cd $(TARGETPATH)
	-del /q $(OBJS) > nul

##############################################

$(INSTPATH):
	if not exist $@ mkdir $@ > nul

install: $(INSTPATH)
	copy $(TARGET) $(INSTPATH) > nul
