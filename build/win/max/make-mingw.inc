DEFS += -DFLEXT_SYS=1
CFLAGS += -fpack-struct=2

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/msp-includes
LIBPATH += -L$(MAXSDKPATH)/max-includes -L$(MAXSDKPATH)/msp-includes

# these are both in PDPATH
LIBS += -lmaxapi -lmaxaudio

EXT=mxe

#########################################

ifdef SHARED
# --- shared ---

elseifdef THREADED
# --- static multi-threaded ---

ifdef DEBUG
LIBS += -lflext_td-maxwin
else
LIBS += -lflext_t-maxwin
endif

else 
# --- static single-threaded ---

ifdef DEBUG
LIBS += -lflext_d-maxwin
else
LIBS += -lflext-maxwin
endif

endif
