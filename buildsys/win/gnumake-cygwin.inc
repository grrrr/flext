##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)

##############################################

CFLAGS += 
LDFLAGS += -shared

##############################################

ifdef DEBUG
CFLAGS += -g
else
ifdef PROFILE
CFLAGS += -g -pg
LDFLAGS += -pg
else
LDFLAGS += -Wl,-S
endif
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 
