# general settings
!include $(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

###############################
# project-specific definitions

# package info
!include $(PKGINFO)

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

!include $(BUILDPATH)bmake.inc

!include $(BUILDPATH)bmake-$(BUILDCLASS).inc

##############################
# platform-specific make stuff

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\bmake-$(COMPILER).inc

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\bmake-$(COMPILER)-$(BUILDCLASS).inc

##############################
# general make stuff

!include $(BUILDPATH)$(PLATFORM)\bmake-$(COMPILER).inc

!include $(BUILDPATH)$(PLATFORM)\bmake-$(COMPILER)-$(BUILDCLASS).inc