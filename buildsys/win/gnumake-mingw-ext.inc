# build class specific settings

INCPATH += -I$(FLEXTINC)
LIBPATH += -L$(FLEXTLIB) -L$(FLEXTSHLIB)

ifdef SHARED
LIBS += $(FLEXTSHLIB)/lib$(FLEXTNAME).dll
else
LIBS += -l$(FLEXTNAME)
endif

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
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	-strip --strip-unneeded $@

##############################################

_clean_:
	-cmd /c "del /q $(subst /,\,$(TARGETPATH)/*.*)" > nul
	-cmd /c "rmdir /q $(subst /,\,$(TARGETPATH))" > nul

##############################################

$(INSTPATH):
	-cmd /c "mkdir $(subst /,\,$@)"

_install_: $(INSTPATH)
	cmd /c "copy $(subst /,\,$(TARGET) $(INSTPATH))"
