##############################################

CSRCS := $(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS := $(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS := $(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS := $(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 

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

##### precompiled header file ###################

ifdef PRECOMPILE
PRECOMSRC := $(SRCDIR)/$(PRECOMPILE)
else
PRECOMSRC := $(FLEXTINC)/flext.h
endif

ifdef PRECOMSRC
PRECOMDST := $(OBJPATH)/$(notdir $(PRECOMSRC)).gch
PRECOMINC := -I $(OBJPATH)
endif

$(PRECOMDST) : $(PRECOMSRC) $(patsubst %,$(SRCDIR)/%,$(HDRS))
	-mkdir -p $(dir $@)
	$(CXX) -c $(DEFS) $(INCPATH) $(PRECOMSRC) -o $@

##### architecture-specific compilation ###################

define ARCH_COMPILE
$(OBJPATH)/%.opp_$(1) : $(SRCDIR)/%.cpp
	-mkdir -p $(dir $$@)
	$(CXX) -c $(CFLAGS) -arch $(1) $$(CFLAGS_$(1)) $(DEFS) $(PRECOMINC) $(INCPATH) $$^ -o $$@

$(OBJPATH)/%.o_$(1) : $(SRCDIR)/%.c
	-mkdir -p $(dir $$@)
	$(CC) -c $(CFLAGS) -arch $(1) $$(CFLAGS_$(1)) $(DEFS) $(INCPATH) $$^ -o $$@
endef

# implement for all archs in ARCH
$(foreach arch,$(ARCH),$(eval $(call ARCH_COMPILE,$(arch))))

##### make fat object file ###################

$(OBJPATH)/%.opp : $(foreach arch,$(ARCH),$(OBJPATH)/%.opp_$(arch))
	lipo $^ -create -output $@

$(OBJPATH)/%.o : $(foreach arch,$(ARCH),$(OBJPATH)/%.o_$(arch))
	lipo $^ -create -output $@

##############################################

_clean_:
	-rm -r $(TARGETPATH)
	-rm -r $(OBJPATH)

##############################################
