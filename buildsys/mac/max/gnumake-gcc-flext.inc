ifdef SHARED
EXT=dylib
else
EXT=a
endif

##############################################

OBJPATH=$(OUTPATH)/$(OUTSUB)

TARGETPATH=$(OBJPATH)
TARGET=$(TARGETPATH)/$(OUTNAME).$(EXT)
INSTTARGET=$(TARGET)

