DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

LDFLAGS += -bundle -bundle_loader $(PDBIN)

LIBS +=

EXT=pd_darwin
