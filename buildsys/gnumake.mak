# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing /

###############################################
# package info

USRINFO=package.txt

include $(USRINFO)

###############################################
# check variables

ifndef BUILDCLASS
	error BUILDCLASS must be specified in $(USRINFO)
endif

ifndef BUILDMODE
	error BUILDMODE must be specified in $(USRINFO)
endif

ifndef BUILDTYPE
	error BUILDTYPE must be specified in $(USRINFO)
endif

##############################

ifndef TARGETMODE
	TARGETMODE=$(BUILDMODE)
endif

ifndef TARGETTYPE
	TARGETTYPE=$(BUILDTYPE)
endif

###############################################

ifeq ($(PLATFORM),win)
	# substitute eventual \ by /
	UBUILDPATH=$(subst \,/,$(BUILDPATH))
else
	UBUILDPATH=$(BUILDPATH)
endif

###############################################

SYSCONFIG=$(UBUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt
SYSDEFAULT=$(UBUILDPATH)$(PLATFORM)/$(RTSYS)/config-$(COMPILER).def

OPTIONS=-f $(UBUILDPATH)gnumake-sub.mak \
	PLATFORM=$(PLATFORM) RTSYS=$(RTSYS) COMPILER=$(COMPILER) \
	BUILDPATH=$(UBUILDPATH) USRINFO=$(USRINFO)

###############################################

ifdef BUILDDIR
USRCONFIG=config.txt
USRDEFAULT=$(BUILDDIR)/config-$(PLATFORM).def

USRMAKE=$(BUILDDIR)/makefile-$(PLATFORM)-$(COMPILER).inc

OPTIONS+=USRCONFIG=$(USRCONFIG) USRMAKE=$(USRMAKE)
endif

###############################################
# include file describing default target dependencies

include $(BUILDPATH)targets.inc

include $(BUILDPATH)targets-$(BUILDCLASS).inc

###############################################

.precious: $(SYSCONFIG) $(USRCONFIG)

$(SYSCONFIG): $(SYSDEFAULT)
	@cp $< $@
	@echo -------------------------------------------------------------------------
	@echo A default system configuration file has been created.
	@echo Please edit $(SYSCONFIG) 
	@echo to match your platform, then start again.
	@echo -------------------------------------------------------------------------
	@false

ifdef BUILDDIR	
$(USRCONFIG): $(USRDEFAULT)
	@cp $< $@
	@echo -------------------------------------------------------------------------
	@echo A default package configuration file has been created.
	@echo Please edit $(USRCONFIG), then start again.
	@echo -------------------------------------------------------------------------
	@false

$(USRDEFAULT) $(USRMAKE):
	@echo -------------------------------------------------------------------------
	@echo Your combination of platform, system and compiler is not supported yet.
	@echo Required files: 
	@echo $(USRDEFAULT)
	@echo and
	@echo $(USRMAKE)
	@echo -------------------------------------------------------------------------
	@false
endif
