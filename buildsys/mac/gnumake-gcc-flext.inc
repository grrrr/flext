# build class specific settings

ifdef SHARED
#LDFLAGS += -dylib_install_name $(FLEXTSHLIB)/$(OUTNAME).$(EXT)
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

_install_: $(FLEXTINC) $(FLEXTLIBINST)
	install $(TARGET) $(FLEXTLIBINST)
ifndef SHARED
# have to rerun ranlib at install dir ?!
	ranlib $(FLEXTLIBINST)/$(notdir $(TARGET))  
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
