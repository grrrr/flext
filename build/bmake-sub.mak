!include $(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

###############################

# these are project specific

# special package settings
!include config.txt

# package specific make stuff
!include build\makefile-$(PLATFORM)-$(COMPILER).inc

# package info
!include build\package.txt

##############################

# platform-specific make stuff
!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\make-$(COMPILER).inc
# general make stuff
!include $(BUILDPATH)$(PLATFORM)\make-$(COMPILER).inc
