DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

# these are both in PDPATH
LIBS += -lpd -lpthreadVC

EXT=dll
