DEFS=$(DEFS) /DFLEXT_SYS=1
CFLAGS=$(CFLAGS) /Zp2 

INCPATH=$(INCPATH) /I$(MAXSDKPATH)\max-includes /I$(MAXSDKPATH)\msp-includes
LIBPATH=$(LIBPATH) /LIBPATH:$(MAXSDKPATH)\max-includes /LIBPATH:$(MAXSDKPATH)\msp-includes

# these are both in MAXSDKPATH
LIBS=$(LIBS) maxapi.lib maxaudio.lib

EXT=mxe
