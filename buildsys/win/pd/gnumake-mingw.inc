DEFS += -DFLEXT_SYS=2

ifdef WINNT
CFLAGS += -D_WIN32_WINNT=0x500
endif

##############################################

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

# these are both in PDPATH
LIBS += -lpd -lpthreadVC
