# general settings
!include $(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

##############################

# package info
!include $(USRINFO)

# special package settings
!ifdef USRCONFIG
!include $(USRCONFIG)
!endif

# package specific make stuff
!ifdef USRMAKE
!include $(USRMAKE)
!endif

##############################

# flext-specific make stuff
!include $(BUILDPATH)nmake-flext.inc

# platform-specific make stuff
!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\make-$(COMPILER).inc
# general make stuff
!include $(BUILDPATH)$(PLATFORM)\make-$(COMPILER).inc
