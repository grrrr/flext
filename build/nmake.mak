# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing \

SYSCONFIG=$(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt
SYSDEFAULT=$(BUILDPATH)$(PLATFORM)\$(RTSYS)\config-$(COMPILER).def
USRCONFIG=config.txt
USRDEFAULT=build\config-$(PLATFORM).def
USRMAKE=build/makefile-$(PLATFORM)-$(COMPILER).inc

EMPTYMSG=^# Nothing to adjust!


OPTIONS=-f $(BUILDPATH)nmake-sub.mak \
	PLATFORM=$(PLATFORM) RTSYS=$(RTSYS) COMPILER=$(COMPILER) \
	BUILDPATH=$(BUILDPATH)


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
	@copy $** $@
	@echo -------------------------------------------------------------------------
	@echo A default system configuration file has been created.
	@echo Please edit $(SYSCONFIG) 
	@echo to match your platform and start again.
	@echo -------------------------------------------------------------------------
	@exit 1
	
$(USRCONFIG):
	@if exist $(USRDEFAULT) ( copy $(USRDEFAULT) $@ ) else ( echo $(EMPTYMSG) > $@ )
	@echo -------------------------------------------------------------------------
	@echo A default package configuration file has been created.
	@echo Please edit $(USRCONFIG) and start again.
	@echo -------------------------------------------------------------------------
	@exit 1

$(USRMAKE):
	@echo -------------------------------------------------------------------------
	@echo Your combination of platform, system and compiler is not supported yet.
	@echo Required file: $(USRMAKE)
	@echo -------------------------------------------------------------------------
	@exit 1
