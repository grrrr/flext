##############################################

TARGETPATH=$(OUTPATH)/$(OUTSUB)
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)

##############################################

LDFLAGS += -shared -Wl,-x

##############################################

ifdef DEBUG
CFLAGS += -g -D_DEBUG
else
CFLAGS += $(OFLAGS) -DNDEBUG
LDFLAGS += -Wl,-S
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OUTPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OUTPATH)/%.opp,$(filter %.cpp,$(SRCS))) 
