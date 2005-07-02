# build class specific settings

ifdef SHARED
LDFLAGS += -install_name $(FLEXTLIB)/$(TARGETNAME)
LDFLAGS += -current_version $(FLEXTMAJOR).$(FLEXTMINOR).$(FLEXTMICRO)
LDFLAGS += -compatibility_version $(FLEXTMAJOR).$(FLEXTMINOR)
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

$(FLEXTINC):
	-mkdir -p $@

$(FLEXTLIB):
	-mkdir -p $@

_install_: $(FLEXTINC) $(FLEXTLIB)
ifdef FLEXTFRAMEWORK
# --- install as framework ---
	mkdir -p $(FLEXTFRAMEWORK)/Versions/$(FLEXTMAJOR).$(FLEXTMINOR)/Headers
	ln -sfh ./$(FLEXTMAJOR).$(FLEXTMINOR) $(FLEXTFRAMEWORK)/Versions/Current
	ln -sfh ./Versions/Current/Headers $(FLEXTFRAMEWORK)/Headers
	install $(TARGET) $(FLEXTFRAMEWORK)/Versions/Current
ifdef SHARED
	# make link without lib... and extension
	ln -sf ./Versions/Current/$(VERNAME) $(FLEXTFRAMEWORK)/$(patsubst lib%,%,$(basename $(TARGETNAME)))
else
	ranlib $(FRAMEWORK)/Versions/Current/$(VERNAME) 
	ln -sf ./Versions/Current/$(VERNAME) $(FLEXTFRAMEWORK)/$(TARGETNAME)
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTFRAMEWORK)/Headers
else
# --- install as dynamic library ---
	install $(TARGET) $(FLEXTLIB)
ifndef SHARED
# have to rerun ranlib at install dir ?!
	ranlib $(FLEXTLIB)/$(VERNAME)  
endif
ifdef VERNAME
	ln -sf $(VERNAME) $(FLEXTLIB)/$(TARGETNAME)
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
endif
# transfer build system
#	cp -rf buildsys $(FLEXTSYS)
#	install build.sh $(FLEXTSYS)
#	chmod -R a+rx $(FLEXTSYS)
# make convenience script
#	echo bash $(FLEXTSYS)/build.sh $$\* > $(FLEXTBIN)/flext-build.sh
#	chmod a+rx $(FLEXTBIN)/flext-build.sh
