EXT=pd_darwin

LDFLAGS += -bundle -bundle_loader $(PDBIN) 

TARGET=$(TARGETPATH)/$(TARGETNAME)
INSTTARGET=$(TARGET)
