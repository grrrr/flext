# general settings
!include $(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

##############################
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
# flext-specific definitions

!include $(BUILDPATH)nmake-$(BUILDCLASS).inc

!include $(BUILDPATH)nmake.inc

##############################
# platform-specific make stuff

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\nmake-$(COMPILER)-$(BUILDCLASS).inc

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\nmake-$(COMPILER).inc

##############################
# general make stuff

!include $(BUILDPATH)$(PLATFORM)\nmake-$(COMPILER)-$(BUILDCLASS).inc

!include $(BUILDPATH)$(PLATFORM)\nmake-$(COMPILER).inc
