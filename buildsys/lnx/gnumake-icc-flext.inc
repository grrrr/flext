# build class specific settings

##############################################

# default target
_build_: $(TARGET)

$(CSRCS) $(CPPSRCS): $(patsubst %,$(SRCDIR)/%,$(HDRS))
	touch $@

$(TARGETPATH):
	-mkdir -p $@

$(TARGETPATH)/%.opp : $(SRCDIR)/%.cpp
	icc -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGETPATH)/%.o : $(SRCDIR)/%.c
	icc -c $(CFLAGS) $(DEFS) $(INCPATH) $< -o $@

$(TARGET) :: $(TARGETPATH) 

$(TARGET) :: $(COBJS) $(CPPOBJS)
ifdef SHARED
	icc $(LDFLAGS) $(LIBPATH) -o $@ $(COBJS) $(CPPOBJS) $(LIBS)
	chmod 755 $@
ifndef DEBUG
ifndef PROFILE
	strip --strip-unneeded $@
endif
endif
else
	$(AR) rc $@ $(COBJS) $(CPPOBJS)
endif

##############################################

_clean_:
	-rm -r $(TARGETPATH)

##############################################

$(FLEXTINC) $(FLEXTLIB) $(FLEXTSYS) $(FLEXTBIN):
	-mkdir -p $@

_install_: $(FLEXTINC) $(FLEXTLIB) $(FLEXTSYS) $(FLEXTBIN)
	install $(TARGET) $(FLEXTLIB)
ifdef VERNAME
ifdef SHARED
	/sbin/ldconfig -l $(FLEXTLIB)/$(VERNAME)
else
	-ln -sf $(FLEXTLIB)/$(VERNAME) $(FLEXTLIB)/$(TARGETNAME)
endif
endif
	install $(patsubst %,$(SRCDIR)/%,$(HDRS)) $(FLEXTINC)
# transfer build system
	cp -rf buildsys $(FLEXTSYS)
	install build.sh $(FLEXTSYS)
	chmod -R a+rx $(FLEXTSYS)
# make convenience script
	echo bash $(FLEXTSYS)/build.sh $$\* > $(FLEXTBIN)/flext-build.sh
	chmod a+rx $(FLEXTBIN)/flext-build.sh
	
