DEFS += -DFLEXT_SYS=1

# fpack-struct should align to 2 bytes... can't specify that...
CFLAGS += -DWINVER=0x0501 -D_WIN32_WINNT=0x501

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/jit-includes -I$(MAXSDKPATH)/msp-includes
LIBPATH += -L$(MAXSDKPATH)/max-includes -L$(MAXSDKPATH)/jit-includes -L$(MAXSDKPATH)/msp-includes

# these are both in MAXSDKPATH
SYSLIBS += -lMaxAPI -lMaxAudio
