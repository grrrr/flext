DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src

# trick PD in a way that only the cygwin pthread headers are used
CFLAGS += -include /usr/include/pthread.h -DPTHREAD_H

LIBS += $(PDPATH)/bin/pd.dll 
#LIBS += $(PDPATH)/bin/pthreadVC.dll

EXT=dll

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
LIBS += $(FLEXTPATH)/flext_td-pdwin.a
else
LIBS += $(FLEXTPATH)/flext_t-pdwin.a
endif

else 
# --- static single-threaded ---

ifdef DEBUG
LIBS += $(FLEXTPATH)/flext_d-pdwin.a
else
LIBS += $(FLEXTPATH)/flext-pdwin.a
endif

endif
endif
