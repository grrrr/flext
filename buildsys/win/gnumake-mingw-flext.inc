# build class specific settings

##############################################

# default target
_build_: $(TARGET)

#$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
#	touch $@

$(OUTPATH):
	-cmd /c "mkdir $(subst /,\,$@)"

$(TARGETPATH): $(OUTPATH)
	-cmd /c "mkdir $(subst /,\,$@)"

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH)

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	-strip --strip-unneeded $@
else
	$(AR) rc $@ $(COBJS) $(CPPOBJS)
endif

##############################################

_clean_:
	-cmd /c "del /q $(subst /,\,$(TARGETPATH)/*.*)" > nul
	-cmd /c "rmdir /q $(subst /,\,$(TARGETPATH))" > nul

##############################################

ifdef SHARED
FLEXTLIBINST=$(FLEXTSHLIB)
else
FLEXTLIBINST=$(FLEXTLIB)
endif

$(FLEXTINC):
	-cmd /c "mkdir $(subst /,\,$@)"

$(FLEXTLIBINST):
	-cmd /c "mkdir $(subst /,\,$@)"

_install_:: $(FLEXTINC) $(FLEXTLIBINST)

_install_::
	cmd /c "copy $(subst /,\,$(TARGET) $(FLEXTLIBINST))"
	-cmd /c "for %i in ($(HDRS)) do @cmd /c copy $(SRCDIR)\%i $(subst /,\,$(FLEXTPATH))" > nul
