DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

# these are both in PDPATH
LIBS += -lpd -lpthreadVC

EXT=dll

#########################################

ifdef SHARED
# --- shared ---

elseifdef THREADED
# --- static multi-threaded ---

ifdef DEBUG
LIBS += -lflext_td-pdwin
else
LIBS += -lflext_t-pdwin
endif

else 
# --- static single-threaded ---

ifdef DEBUG
LIBS += -lflext_d-pdwin
else
LIBS += -lflext-pdwin
endif

endif
