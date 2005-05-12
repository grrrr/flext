DEFS=$(DEFS) /DFLEXT_SYS=2

!ifdef WINNT
DEFS=$(DEFS) /D_WIN32_WINNT=0x500
!endif

##############################################

INCPATH=$(INCPATH) /I$(PDPATH)\src
LIBPATH=$(LIBPATH) /LIBPATH:$(PDPATH)\bin

# these are both in PDPATH
LIBS=$(LIBS) pd.lib pthreadVC.lib
