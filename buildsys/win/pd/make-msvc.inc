DEFS=$(DEFS) /DFLEXT_SYS=2

INCPATH=$(INCPATH) /I$(PDPATH)\src
LIBPATH=$(LIBPATH) /LIBPATH:$(PDPATH)\bin

# these are both in PDPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib
