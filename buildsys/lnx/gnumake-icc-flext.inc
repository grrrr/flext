# build class specific settings

##############################################

# default target
_build_: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

$(TARGETPATH):
	-mkdir -p $@

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	icc -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	icc -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH) 

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	icc $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
ifndef DEBUG
ifndef PROFILE
	strip --strip-unneeded $@
endif
endif
else
	$(AR) rc $@ $(COBJS) $(CPPOBJS)
endif

##############################################

_clean_:
	-rm -r $(TARGETPATH)

##############################################

ifdef SHARED
FLEXTLIBINST=$(FLEXTSHLIB)
else
FLEXTLIBINST=$(FLEXTLIB)
endif

$(FLEXTINC):
	-mkdir -p $@

$(FLEXTLIBINST):
	-mkdir -p $@

_install_: $(FLEXTINC) $(FLEXTLIBINST)
	install $(TARGET) $(FLEXTLIBINST)
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
