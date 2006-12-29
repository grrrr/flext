# build class specific settings

INCPATH += -I$(FLEXTINC)
LIBPATH += -L$(FLEXTLIB)
LIBS += -l$(FLEXTNAME)

# common compilation stuff
include $(BUILDPATH)$(PLATFORM)/gnumake-gcc-targets.inc

# Attention: $@ doesn't work for paths with spaces....
$(INSTPATH):
	-mkdir -p $(INSTPATH)

_install_:: $(INSTPATH)
# copy plain file or whole bundle
	cp -R $(INSTTARGET) $(INSTPATH)
