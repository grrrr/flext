DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

LDFLAGS += -bundle -bundle_loader $(PDBIN)

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)

TARGETPATH=$(OBJPATH)/$(OUTNAME).$(EXT)/Contents/MacOS
TARGET=$(TARGETPATH)/$(OUTNAME)
