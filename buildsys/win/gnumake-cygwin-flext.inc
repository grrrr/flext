# build class specific settings

##############################################

# default target
build: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

$(OUTPATH):
	-mkdir -p $@

$(TARGETPATH): $(OUTPATH)
	-mkdir -p $@

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH) 

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
else
	$(AR) rc $@ $(COBJS) $(CPPOBJS)
endif

##############################################

clean:
	rm $(COBJS) $(CPPOBJS) $(TARGET)

##############################################

ifdef SHARED
FLEXTLIBINST=$(FLEXTSHLIB)
else
FLEXTLIBINST=$(FLEXTLIB)
endif

$(FLEXTINC):
	-mkdir -p $@

$(FLEXTLIBINST):
	-mkdir -p $@

install: $(FLEXTINC) $(FLEXTLIBINST)
	install $(TARGET) $(FLEXTLIBINST)
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
