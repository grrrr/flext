# build class specific settings

ifdef SHARED
LDFLAGS += -install_name $(FLEXTSHLIB)/$(TARGETNAME)
# PKGVERSION must be in the 0.0.0 format
LDFLAGS += -current_version $(PKGVERSION)
# strip last number
LDFLAGS += -compatibility_version $(basename $(PKGVERSION))
LDFLAGS += -preload -seg1addr 0xd0000000
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 

# default target
_build_: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

$(OBJPATH):
	-mkdir -p $@

$(TARGETPATH):
	-mkdir -p $@

ifdef PRECOMPILE
PRECOMSRC=$(SRCDIR)/$(PRECOMPILE)
PRECOMDST=$(OBJPATH)/$(notdir $(PRECOMSRC)).gch
PRECOMINC = -include $(OBJPATH)/$(notdir $(PRECOMSRC))
endif

$(PRECOMDST): $(PRECOMSRC) $(patsubst %,$(SRCDIR)/%,$(HDRS))
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $(PRECOMSRC) -o $@

$(OBJPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(PRECOMINC) $(INCPATH) $< -o $@

$(OBJPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(OBJPATH) $(TARGETPATH)

$(TARGET) :: $(PRECOMDST) $(COBJS) $(CPPOBJS)
ifdef SHARED
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
else
	libtool -static -o $@ $(COBJS) $(CPPOBJS)
endif

##############################################

_clean_:
	-rm -r $(TARGETPATH)
	-rm -r $(OBJPATH)

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

ifneq (,$(findstring Frameworks,$(FLEXTLIBINST)))
FRAMEWORK=$(FLEXTLIBINST)/flext.framework
endif

_install_: $(FLEXTINC) $(FLEXTLIBINST)
ifdef FRAMEWORK
# --- install as framework ---
ifndef PKGVERSION
$(error Need package version)
endif
	mkdir -p $(FRAMEWORK)/Versions/$(basename $(PKGVERSION))/Headers
	ln -sfh ./$(basename $(PKGVERSION)) $(FRAMEWORK)/Versions/Current
	ln -sfh ./Versions/Current/Headers $(FRAMEWORK)/Headers
	install $(TARGET) $(FRAMEWORK)/Versions/Current
ifdef SHARED
	# make link without lib... and extension
	ln -sf ./Versions/Current/$(VERNAME) $(FRAMEWORK)/$(patsubst lib%,%,$(basename $(TARGETNAME)))
else
	ranlib $(FRAMEWORK)/Versions/Current/$(VERNAME) 
	ln -sf ./Versions/Current/$(VERNAME) $(FRAMEWORK)/$(TARGETNAME)
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FRAMEWORK)/Headers
else
# --- install as dynamic library ---
	install $(TARGET) $(FLEXTLIBINST)
ifndef SHARED
# have to rerun ranlib at install dir ?!
	ranlib $(FLEXTLIBINST)/$(VERNAME)  
endif
ifdef VERNAME
	ln -sf $(FLEXTLIBINST)/$(VERNAME) $(FLEXTLIBINST)/$(TARGETNAME)
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
endif
