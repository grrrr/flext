# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing /


ifeq ($(PLATFORM),win)
	# substitute eventual \ by /
	UBUILDPATH=$(subst \,/,$(BUILDPATH))
else
	UBUILDPATH=$(BUILDPATH)
endif


SYSCONFIG=$(UBUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt
SYSDEFAULT=$(UBUILDPATH)$(PLATFORM)/$(RTSYS)/config-$(COMPILER).def
USRCONFIG=config.txt
USRDEFAULT=build/config-$(PLATFORM).def
USRMAKE=build/makefile-$(PLATFORM)-$(COMPILER).inc

EMPTYMSG=\\\# no settings to adjust \!


OPTIONS=-f $(UBUILDPATH)gnumake-sub.mak \
	PLATFORM=$(PLATFORM) RTSYS=$(RTSYS) COMPILER=$(COMPILER) \
	BUILDPATH=$(UBUILDPATH)


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


config: $(SYSCONFIG) $(USRCONFIG) $(USRMAKE)


.precious: $(SYSCONFIG) $(USRCONFIG)

$(SYSCONFIG): $(SYSDEFAULT)
	@cp $> $@
	@echo -------------------------------------------------------------------------
	@echo A default system configuration file has been created.
	@echo Please edit $(SYSCONFIG) 
	@echo to match your platform and start again.
	@echo -------------------------------------------------------------------------
	@false
	
$(USRCONFIG):
	@if [ -f $(USRDEFAULT) ]; then cp $(USRDEFAULT) $@; else echo ${EMPTYMSG} > $@; fi
	@echo -------------------------------------------------------------------------
	@echo A default package configuration file has been created.
	@echo Please edit $(USRCONFIG) and start again.
	@echo -------------------------------------------------------------------------
	@false

$(USRMAKE):
	@echo -------------------------------------------------------------------------
	@echo Your combination of platform, system and compiler is not supported yet.
	@echo Required file: $(USRMAKE)
	@echo -------------------------------------------------------------------------
	@false
