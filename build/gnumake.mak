# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin

# this should be improved
BUILDPATH=../flext/build

include $(BUILDPATH)/config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

###############################

# these are project specific

# special settings
include config-$(PLATFORM)-$(COMPILER).txt

# package specific make stuff
include makefile-$(PLATFORM)-$(COMPILER).txt

# package info
include make-files.txt

##############################

# platform-specific make stuff
include $(BUILDPATH)/make-$(PLATFORM)-$(RTSYS)-$(COMPILER).inc
# general make stuff
include $(BUILDPATH)/make-$(PLATFORM)-gen-$(COMPILER).inc
