DEFS += -DFLEXT_SYS=1

INCPATH += -I$(MAXSDKPATH)/max-includes -I$(MAXSDKPATH)/msp-includes

LDFLAGS += -framework MaxAPI -framework MaxAudioAPI

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)

TARGETPATH=$(OBJPATH)/$(OUTNAME).$(EXT)/Contents/MacOS
TARGET=$(TARGETPATH)/$(OUTNAME)
