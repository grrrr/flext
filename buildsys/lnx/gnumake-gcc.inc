##############################################

ifndef FLEXTINC
FLEXTINC=$(FLEXTPREFIX)/include/flext
endif
ifndef FLEXTLIB
FLEXTLIB=$(FLEXTPREFIX)/lib
endif
ifndef FLEXTSYS
FLEXTSYS=$(FLEXTPREFIX)/lib/flext
endif
ifndef FLEXTBIN
FLEXTBIN=$(FLEXTPREFIX)/bin
endif

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGETNAME=$(OUTNAME).$(EXT)

##############################################

CFLAGS += -pthread -fPIC
LDFLAGS += -pthread -shared -Wl,-x 

##############################################

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
else
ifdef PROFILE
CFLAGS += -g -pg
LDFLAGS += -g -pg
else
LDFLAGS += -Wl,-S
endif
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 
