# system settings
include $(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt

###############################
# project specific definitions

# package info
include $(PKGINFO)

# special settings
ifdef USRCONFIG
	include $(USRCONFIG)
endif

# package specific make stuff
ifdef USRMAKE
	include $(USRMAKE)
endif

##############################

ifndef TARGETMODE
ifdef DEBUG
	TARGETMODE=debug
else
	TARGETMODE=release
endif
endif

ifndef TARGETTYPE
ifdef SHARED
	TARGETTYPE=shared
else
ifdef THREADED
	TARGETTYPE=multi
else
	TARGETTYPE=single
endif
endif
endif

##############################
# flext-specific definitions

include $(BUILDPATH)gnumake.inc

include $(BUILDPATH)gnumake-$(BUILDCLASS).inc

##############################
# platform-specific make stuff

include $(BUILDPATH)$(PLATFORM)/$(RTSYS)/gnumake-$(COMPILER).inc

include $(BUILDPATH)$(PLATFORM)/$(RTSYS)/gnumake-$(COMPILER)-$(BUILDCLASS).inc

##############################
# general make stuff

include $(BUILDPATH)$(PLATFORM)/gnumake-$(COMPILER).inc

include $(BUILDPATH)$(PLATFORM)/gnumake-$(COMPILER)-$(BUILDCLASS).inc
