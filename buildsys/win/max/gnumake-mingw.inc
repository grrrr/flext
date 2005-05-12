DEFS += -DFLEXT_SYS=1
CFLAGS += -fpack-struct=2 -D_WIN32_WINNT=0x501

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/msp-includes
LIBPATH += -L$(MAXSDKPATH)/max-includes -L$(MAXSDKPATH)/msp-includes

# these are both in MAXSDKPATH
LIBS += -lmaxapi -lmaxaudio
