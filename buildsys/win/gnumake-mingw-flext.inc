# build class specific settings

##############################################

# default target
_build_: $(TARGET)

#$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
#	touch $@

$(OUTPATH):
	-mkdir $(subst /,\,$@)

$(TARGETPATH): $(OUTPATH)
	-mkdir $(subst /,\,$@)

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH)

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
else
	$(AR) rc $@ $(COBJS) $(CPPOBJS)
endif

##############################################

_clean_:
	-del $(subst /,\,$(COBJS) $(CPPOBJS) $(TARGET))

##############################################

ifdef SHARED
FLEXTLIBINST=$(FLEXTSHLIB)
else
FLEXTLIBINST=$(FLEXTLIB)
endif

$(FLEXTINC):
	-mkdir $(subst /,\,$@)

$(FLEXTLIBINST):
	-mkdir $(subst /,\,$@)

_install_:: $(FLEXTINC) $(FLEXTLIBINST)

_install_::
	copy $(subst /,\,$(TARGET) $(FLEXTLIBINST))
	-for %%i in ($(HDRS)) do @copy $(SRCDIR)\%%i $(subst /,\,$(FLEXTPATH)) > nul
