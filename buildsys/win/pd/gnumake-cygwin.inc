DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src

LIBS += $(PDPATH)/bin/pthreadVC.dll
SYSLIBS += $(PDPATH)/bin/pd.dll 
