INCPATH=$(INCPATH) /I$(PDPATH)\src
LIBPATH=$(LIBPATH) /LIBPATH:$(PDPATH)\bin

DEFS=$(DEFS) /DFLEXT_SYS=2

# these are both in PDPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib
