# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing /

include $(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

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
include $(BUILDPATH)$(PLATFORM)/$(RTSYS)/make-$(COMPILER).inc
# general make stuff
include $(BUILDPATH)$(PLATFORM)/make-$(COMPILER).inc
