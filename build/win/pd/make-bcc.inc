DEFS=$(DEFS) /DFLEXT_SYS=2

INCPATH=$(INCPATH) -I$(PDPATH)\src

# these are both in FLEXTPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib

EXT=dll

#########################################

!ifdef SHARED
# --- shared ---

!else
!ifdef THREADED
# --- static multi-threaded ---

!ifdef DEBUG
LIBS=$(LIBS) flext_td-pdwin.lib 
!else
LIBS=$(LIBS) flext_t-pdwin.lib 
!endif

!else 
# --- static single-threaded ---

!ifdef DEBUG
LIBS=$(LIBS) flext_d-pdwin.lib 
!else
LIBS=$(LIBS) flext-pdwin.lib 
!endif

!endif
!endif
