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

!include $(BUILDPATH)nmake.inc

!include $(BUILDPATH)nmake-$(BUILDCLASS).inc

##############################
# platform-specific make stuff

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\make-$(COMPILER).inc

!include $(BUILDPATH)$(PLATFORM)\$(RTSYS)\make-$(COMPILER)-$(BUILDCLASS).inc

##############################
# general make stuff

!include $(BUILDPATH)$(PLATFORM)\make-$(COMPILER).inc

!include $(BUILDPATH)$(PLATFORM)\make-$(COMPILER)-$(BUILDCLASS).inc
