ifndef FLEXTBUILD
INCPATH += -I$(FLEXTPATH)
LIBPATH += -L$(FLEXTPATH)
LIBS+=$(FLEXTPATH)/$(FLEXTNAME).a
endif

LDFLAGS += -shared -Wl,-x

##############################################

ifdef DEBUG
CFLAGS += -g -D_DEBUG
else
CFLAGS += $(OFLAGS) -DNDEBUG
LDFLAGS += -Wl,-S
endif

##############################################

ifdef FLEXTBUILD
OUTNAME=$(FLEXTNAME)

ifdef SHARED
EXT=dll
else
EXT=a
BUILDLIB=1
endif

else
OUTNAME=$(NAME)
endif

##############################################

TARGETPATH=$(OUTPATH)/$(OUTSUB)
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(TARGETPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(TARGETPATH)/%.opp,$(filter %.cpp,$(SRCS))) 

# default target
_all_: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

$(TARGETPATH):
	-mkdir $(OUTPATH)
	-mkdir $@

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH) 

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef BUILDLIB
	$(AR) rc $@ $(COBJS) $(CPPOBJS)
else
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
endif

##############################################

_clean_:
	rm $(COBJS) $(CPPOBJS) $(TARGET)

##############################################

$(INSTPATH):
	-mkdir $@

$(FLEXTPATH):
	-mkdir $@

_install_:: $(INSTPATH) $(FLEXTPATH)

_install_:: $(TARGET)
ifdef FLEXTBUILD
	cp $(TARGET) $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTPATH)
else
	install $(TARGET) $(INSTPATH)
endif
