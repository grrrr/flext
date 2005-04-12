EXT=pd_darwin

LDFLAGS += -bundle -bundle_loader $(PDBIN) -flat_namespace

TARGET=$(TARGETPATH)/$(TARGETNAME)
INSTTARGET=$(TARGET)
