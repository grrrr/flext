# build class specific settings

INCPATH += -I$(FLEXTINC)
LIBPATH += -L$(FLEXTLIB) -L$(FLEXTSHLIB)
LIBS += -l$(FLEXTNAME)

##############################################

# default target
_build_: $(TARGET)

#$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
#	touch $@

$(OUTPATH):
	-mkdir $(subst /,\,$@)

$(TARGETPATH): $(OUTPATH)
	-mkdir $(subst /,\,$@)

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	$(CXX) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH)

$(TARGET) :: $(COBJS) $(CPPOBJS)
	$(CXX) $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)

##############################################

_clean_:
	-del $(subst /,\,$(COBJS) $(CPPOBJS) $(TARGET))

##############################################

$(INSTPATH):
	-mkdir $(subst /,\,$@)

_install_: $(INSTPATH)
	copy $(subst /,\,$(TARGET) $(INSTPATH))
