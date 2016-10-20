DEFS += -DFLEXT_SYS=1

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/jit-includes -I$(MAXSDKPATH)/msp-includes

LDFLAGS += -framework MaxAPI -F$(MAXSDKPATH)/max-includes
LDFLAGS += -framework MaxAudioAPI -F$(MAXSDKPATH)/msp-includes

