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
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH) 

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ -Wl,-soname,$(SONAME) $(COBJS) $(CPPOBJS) $(LIBS)
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
	ldconfig -l $(TARGET)
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
