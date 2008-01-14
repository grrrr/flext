DEFS += -DFLEXT_SYS=2 -DPD

INCPATH += -I$(PDPATH)/src

ifdef THREADED
LIBS += $(PDPATH)/bin/pthreadVC.dll
endif

SYSLIBS += $(PDPATH)/bin/pd.dll 
