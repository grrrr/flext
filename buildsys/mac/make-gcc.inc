ifndef FLEXTBUILD
INCPATH += -I$(FLEXTPATH)
LIBPATH += -L$(FLEXTPATH)
LIBS+=$(FLEXTPATH)/$(FLEXTNAME).a
endif

LDFLAGS += -Wl,-x -framework ApplicationServices -framework vecLib -dynamiclib -dynamic

##############################################

ifndef DEBUG
CFLAGS += -g -D_DEBUG
else
CFLAGS += $(OFLAGS) -DNDEBUG
LDFLAGS += -Wl,-S
endif

##############################################

ifdef FLEXTBUILD
OUTNAME=$(FLEXTNAME)

ifdef SHARED
EXT=dylib
else
EXT=a
BUILDLIB=1
endif

else
OUTNAME=$(NAME)
endif

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)

ifdef FLEXTBUILD
TARGETPATH=$(OBJPATH)
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)
else
TARGETPATH=$(OBJPATH)/$(OUTNAME).$(EXT)/Contents/MacOS
TARGET=$(TARGETPATH)/$(OUTNAME)
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 

# default target
_all_: $(TARGET)

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
ifdef BUILDLIB
	libtool -static -o $@ $(COBJS) $(CPPOBJS)
else
#	$(CXX) $(LDFLAGS) $(LIBPATH) -dylib_install_name $(INSTPATH)/$(notdir $@) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
endif

##############################################

_clean_:
	rm $(COBJS) $(CPPOBJS) $(TARGET)

##############################################

$(INSTPATH):
	-mkdir -p $@

$(FLEXTPATH):
	-mkdir -p $@

_install_:: $(INSTPATH) $(FLEXTPATH)

_install_:: $(TARGET)
ifdef FLEXTBUILD
	cp $(TARGET) $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTPATH)
else
	install $(TARGET) $(INSTPATH)
endif
