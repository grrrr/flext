# build class specific settings

#LDFLAGS += -dylib_install_name $(FLEXTSHLIB)

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

$(OBJPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(OBJPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(OBJPATH) $(TARGETPATH)

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
else
	libtool -static -o $@ $(COBJS) $(CPPOBJS)
endif

##############################################

_clean_:
	rm $(COBJS) $(CPPOBJS) $(TARGET)

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
	ranlib $(FLEXTLIBINST)/$(OUTNAME).$(EXT)  # have to rerun ranlib at install dir ?!
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
