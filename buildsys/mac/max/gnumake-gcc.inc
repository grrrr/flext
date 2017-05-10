DEFS += -DFLEXT_SYS=1

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/jit-includes -I$(MAXSDKPATH)/msp-includes

LDFLAGS += -F$(MAXSDKPATH)/max-includes -framework MaxAPI
LDFLAGS += -F$(MAXSDKPATH)/msp-includes -framework MaxAudioAPI

