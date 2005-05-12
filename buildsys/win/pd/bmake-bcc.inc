DEFS=$(DEFS) /DFLEXT_SYS=2

!ifdef WINNT
DEFS=$(DEFS) /D_WIN32_WINNT=0x500
!endif

##############################################

INCPATH=$(INCPATH) -I$(PDPATH)\src

# these are both in FLEXTPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib
