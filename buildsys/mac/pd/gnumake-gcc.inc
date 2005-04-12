DEFS += -DFLEXT_SYS=2

INCPATH += -I$(PDPATH)/src
LIBPATH += -L$(PDPATH)/bin

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)
TARGETPATH=$(OBJPATH)
TARGETNAME=$(OUTNAME).$(EXT)
