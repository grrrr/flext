# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing /

# package info
include build/package.txt


ifeq ($(PLATFORM),win)
	# substitute eventual \ by /
	UBUILDPATH=$(subst \,/,$(BUILDPATH))
else
	UBUILDPATH=$(BUILDPATH)
endif


SYSCONFIG=$(UBUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt
SYSDEFAULT=$(UBUILDPATH)$(PLATFORM)/$(RTSYS)/config-$(COMPILER).def


OPTIONS=-f $(UBUILDPATH)gnumake-sub.mak \
	PLATFORM=$(PLATFORM) RTSYS=$(RTSYS) COMPILER=$(COMPILER) \
	BUILDPATH=$(UBUILDPATH)


ifdef HAVECONFIG
USRCONFIG=config.txt
USRDEFAULT=build/config-$(PLATFORM).def
OPTIONS+=USRCONFIG=$(USRCONFIG)
endif

ifdef HAVEMAKE
USRMAKE=build/makefile-$(PLATFORM)-$(COMPILER).inc
OPTIONS+=USRMAKE=$(USRMAKE)
endif


all: config
	$(MAKE) $(OPTIONS) all

all-debug: config
	$(MAKE) $(OPTIONS) DEBUG=1 $@

all-shared: config
	$(MAKE) $(OPTIONS) SHARED=1 $@

all-shared-debug: config
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 $@

clean install:
	$(MAKE) $(OPTIONS) $@


config: $(USRMAKE) $(SYSCONFIG) $(USRCONFIG) 


.precious: $(SYSCONFIG) $(USRCONFIG)

$(SYSCONFIG): $(SYSDEFAULT)
	@cp $< $@
	@echo -------------------------------------------------------------------------
	@echo A default system configuration file has been created.
	@echo Please edit $(SYSCONFIG) 
	@echo to match your platform and start again.
	@echo -------------------------------------------------------------------------
	@false

ifdef HAVECONFIG	
$(USRCONFIG): $(USRDEFAULT)
	@cp $< $@
	@echo -------------------------------------------------------------------------
	@echo A default package configuration file has been created.
	@echo Please edit $(USRCONFIG) and start again.
	@echo -------------------------------------------------------------------------
	@false
endif

ifdef HAVEMAKE
$(USRMAKE):
	@echo -------------------------------------------------------------------------
	@echo Your combination of platform, system and compiler is not supported yet.
	@echo Required file: $(USRMAKE)
	@echo -------------------------------------------------------------------------
	@false
endif

