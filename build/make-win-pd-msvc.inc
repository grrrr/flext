DEFS=$(DEFS) /DFLEXT_SYS=2

INCPATH=$(INCPATH) /I$(PDPATH)\src
LIBPATH=$(LIBPATH) /LIBPATH:$(PDPATH)\bin

# these are both in PDPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib

EXT=dll

#########################################

!ifdef FLEXT_SHARED
# --- shared ---

!elseifdef FLEXT_THREADED
# --- static multi-threaded ---

!ifdef _DEBUG
LIBS=$(LIBS) flext_td-pdwin.lib 
!else
LIBS=$(LIBS) flext_t-pdwin.lib 
!endif

!else 
# --- static single-threaded ---

!ifdef _DEBUG
LIBS=$(LIBS) flext_d-pdwin.lib 
!else
LIBS=$(LIBS) flext-pdwin.lib 
!endif

!endif
