# build class specific settings

ifndef FLEXTBUILD
INCPATH += -I$(FLEXTPATH)
LIBPATH += -L$(FLEXTPATH)
LIBS += $(FLEXTPATH)/$(FLEXTNAME).a
endif

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
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@

##############################################

clean:
	rm $(COBJS) $(CPPOBJS) $(TARGET)

##############################################

$(INSTPATH):
	-mkdir -p $@

install: $(INSTPATH)
	install $(TARGET) $(INSTPATH)
