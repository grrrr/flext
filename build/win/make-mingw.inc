LDFLAGS += -shared -Wl,-x 

ifdef DEBUG
CFLAGS += -g -D_DEBUG
else
CFLAGS += $(OFLAGS) -DNDEBUG
LDFLAGS += -Wl,-S
endif

##############################################

INCPATH += -I$(FLEXTPATH)
LIBPATH += -L$(FLEXTPATH)

ifdef SHARED
# --- shared ---
DEFS += -DFLEXT_SHARED

else
ifdef THREADED
# --- static multi-threaded ---
DEFS += -DFLEXT_THREADS

else 
# --- static single-threaded ---

endif
endif

##############################################

TARGET=$(OUTPATH)/$(NAME).$(EXT)

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OUTPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OUTPATH)/%.opp,$(filter %.cpp,$(SRCS))) 


# default target
all: $(OUTPATH) $(TARGET)

#$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
#	touch $@

$(OUTPATH):
	mkdir $(OUTPATH)

$(OUTPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(OUTPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) : $(COBJS) $(CPPOBJS)
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)
	chmod 755 $@

$(INSTPATH):
	mkdir $(INSTPATH)

install:: $(INSTPATH)

install:: $(TARGET)
	$(CP) $^ $(INSTPATH)

.PHONY: clean
clean:
	$(RM) $(COBJS) $(CPPOBJS) $(TARGET)
