##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)
TARGETPATH=$(OBJPATH)

ifdef SHARED
SONAME=$(TARGETPATH)/$(OUTNAME).$(EXT)
TARGET=$(TARGETPATH)/$(OUTNAME)-$(PKGVERSION).$(EXT)
else
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)
endif

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
