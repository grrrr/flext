# build class specific settings

INCPATH += -I$(FLEXTINC)
LIBPATH += -L$(FLEXTLIB)
LIBS += -l$(FLEXTNAME)

# obviously header precompilation os still buggy with gcc 3.3

ifdef PRECOMPILE
#PRECOMSRC=$(SRCDIR)/$(PRECOMPILE)
else
#PRECOMSRC=$(FLEXTINC)/flext.h
endif
ifdef PRECOMSRC
PRECOMDST=$(OBJPATH)/$(notdir $(PRECOMSRC)).gch
PRECOMINC = -include $(OBJPATH)/$(notdir $(PRECOMSRC))
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 

##############################################

# default target
_build_: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

# Attention: $@ doesn't work for paths with spaces....
$(OBJPATH):
	-mkdir -p $(OBJPATH)

# Attention: $@ doesn't work for paths with spaces....
$(TARGETPATH):
	-mkdir -p $(TARGETPATH)

$(PRECOMDST) : $(PRECOMSRC) $(patsubst %,$(SRCDIR)/%,$(HDRS))
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $(PRECOMSRC) -o $@

$(OBJPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(PRECOMINC) $(INCPATH) $< -o $@

$(OBJPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(OBJPATH) $(TARGETPATH)

$(TARGET) :: $(PRECOMDST) $(COBJS) $(CPPOBJS)
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@

##############################################

_clean_:
	-rm -r $(TARGETPATH)
	-rm -r $(OBJPATH)

##############################################

# Attention: $@ doesn't work for paths with spaces....
$(INSTPATH):
	-mkdir -p $(INSTPATH)

_install_:: $(INSTPATH)
# copy plain file or whole bundle
	cp -R $(INSTTARGET) $(INSTPATH)
