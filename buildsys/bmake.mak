# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing \

###############################################
# package info

USRINFO=package.txt

!include $(USRINFO)

###############################################
# check variables

!ifndef BUILDCLASS
!error BUILDCLASS must be specified in $(USRINFO)
!endif

!ifndef BUILDMODE
!error BUILDMODE must be specified in $(USRINFO)
!endif

!ifndef BUILDTYPE
!error BUILDTYPE must be specified in $(USRINFO)
!endif

##############################

!ifndef TARGETMODE
TARGETMODE=$(BUILDMODE)
!endif

!ifndef TARGETTYPE
TARGETTYPE=$(BUILDTYPE)
!endif

###############################################

SYSCONFIG=$(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt
SYSDEFAULT=$(BUILDPATH)$(PLATFORM)\$(RTSYS)\config-$(COMPILER).def

OPTIONS=-f $(BUILDPATH)bmake-sub.mak -N \
	PLATFORM=$(PLATFORM) RTSYS=$(RTSYS) COMPILER=$(COMPILER) \
	BUILDPATH=$(BUILDPATH) USRINFO=$(USRINFO)

###############################################

!ifdef BUILDDIR
USRCONFIG=config.txt
USRDEFAULT=$(BUILDDIR)\config-$(PLATFORM).def

USRMAKE=$(BUILDDIR)\makefile-$(PLATFORM)-$(COMPILER).inc

OPTIONS=$(OPTIONS) USRCONFIG=$(USRCONFIG) USRMAKE=$(USRMAKE)
!endif

###############################################
# include file describing default target dependencies

!include $(BUILDPATH)targets.inc

!include $(BUILDPATH)targets-$(BUILDCLASS).inc

###############################################

.precious: $(SYSCONFIG) $(USRCONFIG)

$(SYSCONFIG): $(SYSDEFAULT)
	@copy $** $@
	@echo -------------------------------------------------------------------------
	@echo A default system configuration file has been created.
	@echo Please edit $(SYSCONFIG) 
	@echo to match your platform and start again.
	@echo -------------------------------------------------------------------------
	@exit 1

!ifdef BUILDDIR
$(USRCONFIG): $(USRDEFAULT)
	@copy $** $@
	@echo -------------------------------------------------------------------------
	@echo A default package configuration file has been created.
	@echo Please edit $(USRCONFIG) and start again.
	@echo -------------------------------------------------------------------------
	@exit 1

$(USRDEFAULT) $(USRMAKE):
	@echo -------------------------------------------------------------------------
	@echo Your combination of platform, system and compiler is not supported yet.
	@echo Required files: 
	@echo $(USRDEFAULT)
	@echo and
	@echo $(USRMAKE)
	@echo -------------------------------------------------------------------------
	@exit 1
!endif
