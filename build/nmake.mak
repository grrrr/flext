# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin

# general settings
!include config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

###############################

# these are project specific

# special package settings
!if exist(config-$(PLATFORM)-$(COMPILER).txt)
!include config-$(PLATFORM)-$(COMPILER).txt
!endif

# package specific make stuff
!if exist(makefile-$(PLATFORM)-$(COMPILER).txt)
!include makefile-$(PLATFORM)-$(COMPILER).txt
!endif

# package info
!include make-files.txt

##############################

# platform-specific make stuff
!include make-$(PLATFORM)-$(RTSYS)-$(COMPILER).inc
# general make stuff
!include make-$(PLATFORM)-gen-$(COMPILER).inc
