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

!ifndef TARGETMODE
!ifdef DEBUG
TARGETMODE=debug
!else
TARGETMODE=release
!endif
!endif

!ifndef TARGETTYPE
!ifdef SHARED
TARGETTYPE=shared
!else
!ifdef THREADED
TARGETTYPE=multi
!else
TARGETTYPE=single
!endif
!endif
!endif

##############################
# flext-specific make stuff

!include $(BUILDPATH)bmake-$(BUILDCLASS).inc

!include $(BUILDPATH)bmake.inc

##############################
# platform-specific make stuff

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\bmake-$(COMPILER)-$(BUILDCLASS).inc

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\bmake-$(COMPILER).inc

##############################
# general make stuff

!include $(BUILDPATH)$(PLATFORM)\bmake-$(COMPILER)-$(BUILDCLASS).inc

!include $(BUILDPATH)$(PLATFORM)\bmake-$(COMPILER).inc
