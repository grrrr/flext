# required settings:
#
# PLATFORM - win/mac/lnx
# RTSYS - pd/max
# COMPILER - msvc/gcc/mingw/cygwin
# BUILDPATH including trailing /

# package info
USRINFO=package.txt

include $(USRINFO)


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
	BUILDPATH=$(UBUILDPATH) USRINFO=$(USRINFO)


ifdef BUILDDIR
USRCONFIG=config.txt
USRDEFAULT=$(BUILDDIR)/config-$(PLATFORM).def

USRMAKE=$(BUILDDIR)/makefile-$(PLATFORM)-$(COMPILER).inc

OPTIONS+=USRCONFIG=$(USRCONFIG) USRMAKE=$(USRMAKE)
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
