DEFS += -DFLEXT_SYS=1

# fpack-struct should align to 2 bytes... can't specify that...
CFLAGS += -fpack-struct -D_WIN32_WINNT=0x501

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/msp-includes
LIBPATH += -L$(MAXSDKPATH)/max-includes -L$(MAXSDKPATH)/msp-includes

# these are both in MAXSDKPATH
LIBS += -lmaxapi -lmaxaudio
