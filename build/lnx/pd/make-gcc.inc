DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

LDFLAGS +=

LIBS +=

EXT=pd_linux

#########################################

ifdef SHARED
# --- shared ---

ifdef DEBUG
LIBS += -lflext_d
else
LIBS += -lflext
endif

else
ifdef THREADED
# --- static multi-threaded ---

ifdef DEBUG
LIBS += $(FLEXTPATH)/libflext_td.a
else
LIBS += $(FLEXTPATH)/libflext_t.a
endif

else 
# --- static single-threaded ---

ifdef DEBUG
LIBS += $(FLEXTPATH)/libflext_d.a
else
LIBS += $(FLEXTPATH)/libflext.a
endif

endif
endif
