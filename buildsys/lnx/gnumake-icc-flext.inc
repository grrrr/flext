# build class specific settings

ifdef SHARED
SONAME=$(OUTNAME).$(EXT)
TARGET=$(TARGETPATH)/$(OUTNAME).$(PKGVERSION).$(EXT)
else
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)
endif

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
ifdef SHARED
	/sbin/ldconfig -l $(TARGET)
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
