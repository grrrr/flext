##############################################

OBJPATH=$(OUTPATH)\$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGET=$(TARGETPATH)\$(OUTNAME).$(EXT)

##############################################

CFLAGS += -pthread
LDFLAGS += -pthread -shared

##############################################

CFLAGS += $(UFLAGS)

ifdef DEBUG
CFLAGS += -g -D_DEBUG
else
CFLAGS += $(OFLAGS) -DNDEBUG
LDFLAGS += -Wl,-S
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 
