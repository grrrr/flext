# build class specific settings

INCPATH += -I$(FLEXTINC)
LIBPATH += -L$(FLEXTLIB)
LIBS += -l$(FLEXTNAME)

##############################################

# default target
_build_: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

$(TARGETPATH):
	mkdir -p $@

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	icc -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	icc -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET):: $(TARGETPATH)

$(TARGET):: $(COBJS) $(CPPOBJS)
	icc $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
ifndef DEBUG
ifndef PROFILE
	strip --strip-unneeded $@
endif
endif

##############################################

_clean_:
	-rm -r $(TARGETPATH)

##############################################

$(INSTPATH):
	-mkdir -p $@

_install_: $(INSTPATH)
	install $(TARGET) $(INSTPATH)
