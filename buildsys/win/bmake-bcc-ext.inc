# build class specific settings

INCPATH=$(INCPATH) -I$(FLEXTINC)
LIBPATH=$(LIBPATH) -L$(FLEXTLIB)
LIBS=$(LIBS) $(FLEXTNAME).lib

##############################################

# default target
build: $(TARGET) 

$(OUTPATH): 
	if not exist $@ mkdir $@ > nul

$(TARGETPATH): $(OUTPATH)
	if not exist $@ mkdir $@ > nul

SETUPFUNCTION=$(NAME)_setup

$(TARGETPATH)\$(NAME).def: $(TARGETPATH)
	@echo EXPORTS $(SETUPFUNCTION) = _$(SETUPFUNCTION) > $@
# this next line fixes a strange problem with implib - lacking underscore?!
	@echo IMPORTS _rtext_retext=PD.rtext_retext >> $@

$(TARGET) :: $(TARGETPATH)\$(NAME).def

{$(SRCDIR)}.cpp{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(TARGETPATH) $<

{$(SRCDIR)}.c{}.obj:
	bcc32 -c $(CFLAGS) $(DEFS) $(INCPATH) -n$(TARGETPATH) $<

$(TARGET) :: $(TARGETPATH)

$(TARGET) :: $(OBJS)
	@cd $(TARGETPATH)
	ilink32 $(LDFLAGS) $(LIBPATH) -L. $(OBJS) ,$(<F),,$(LIBS),$(NAME).def
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
