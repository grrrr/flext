DEFS=$(DEFS) /DFLEXT_SYS=2

INCPATH=$(INCPATH) -I$(PDPATH)\src

# these are both in FLEXTPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib

EXT=dll
