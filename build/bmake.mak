# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing \

SYSCONFIG=$(BUILDPATH)config-$(PLATFORM)-$(RTSYS)-$(COMPILER).txt
USRCONFIG=config.txt

.precious: $(SYSCONFIG) $(USRCONFIG)


OPTIONS=-f $(BUILDPATH)bmake-sub.mak -N \
	PLATFORM=$(PLATFORM) RTSYS=$(RTSYS) COMPILER=$(COMPILER) \
	BUILDPATH=$(BUILDPATH)


all: $(SYSCONFIG) $(USRCONFIG)
	$(MAKE) $(OPTIONS) all

all-debug: $(SYSCONFIG) $(USRCONFIG)
	$(MAKE) $(OPTIONS) DEBUG=1 $@

all-shared: $(SYSCONFIG) $(USRCONFIG)
	$(MAKE) $(OPTIONS) SHARED=1 $@

all-shared-debug: $(SYSCONFIG) $(USRCONFIG)
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 $@

clean install:
	$(MAKE) $(OPTIONS) $@


$(SYSCONFIG): $(BUILDPATH)$(PLATFORM)\$(RTSYS)\config-$(COMPILER).def
	copy $** $@
	@echo -------------------------------------------------------------------------
	@echo A default system configuration file has been created.
	@echo Please edit $(SYSCONFIG) 
	@echo to match your platform and start again.
	@echo -------------------------------------------------------------------------
	@exit 1
	
$(USRCONFIG): build\config-$(PLATFORM).def
	copy $** $@
	@echo -------------------------------------------------------------------------
	@echo A default package configuration file has been created.
	@echo Please edit $(USRCONFIG) and start again.
	@echo -------------------------------------------------------------------------
	@exit 1
