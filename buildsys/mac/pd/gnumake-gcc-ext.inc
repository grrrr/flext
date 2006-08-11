EXT=pd_darwin

#LDFLAGS += -bundle -bundle_loader $(PDBIN) 
LDFLAGS += -dylib -undefined dynamic_lookup 

TARGET=$(TARGETPATH)/$(TARGETNAME)
INSTTARGET=$(TARGET)
