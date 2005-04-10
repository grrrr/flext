##############################################

FLEXTINC=$(FLEXTPREFIX)/include/flext
FLEXTLIB=$(FLEXTPREFIX)/lib
FLEXTSYS=$(FLEXTPREFIX)/lib/flext
FLEXTBIN=$(FLEXTPREFIX)/bin

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGETNAME=$(OUTNAME).$(EXT)

ifdef PKGVERSION
VERNAME=$(OUTNAME).$(PKGVERSION).$(EXT)
TARGET=$(TARGETPATH)/$(VERNAME)
else
TARGET=$(TARGETPATH)/$(TARGETNAME)
endif

INSTTARGET=$(TARGET)

##############################################

CFLAGS +=
LDFLAGS += -shared -Wl,-x 

##############################################

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
else
ifdef PROFILE
CFLAGS += -g -qp
LDFLAGS += -g -qp
else
LDFLAGS += -Wl,-S
endif
endif

##############################################

CSRCS=$(patsubst %.c,$(SRCDIR)/%.c,$(filter %.c,$(SRCS)))
CPPSRCS=$(patsubst %.cpp,$(SRCDIR)/%.cpp,$(filter %.cpp,$(SRCS)))
COBJS=$(patsubst %.c,$(OBJPATH)/%.o,$(filter %.c,$(SRCS))) 
CPPOBJS=$(patsubst %.cpp,$(OBJPATH)/%.opp,$(filter %.cpp,$(SRCS))) 
