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


ifdef FLEXTBUILD
all: flext
else
all: build-sr

shared: build-tr
endif

flext: flext-release flext-debug

flext-release: build-dr build-tr build-sr

flext-debug: build-dd build-td build-sd

install: install-dr install-tr install-sr install-dd install-td install-sd

clean: clean-dr clean-tr clean-sr clean-dd clean-td clean-sd


build-sr: config
	$(MAKE) $(OPTIONS) _all_

build-sd: config
	$(MAKE) $(OPTIONS) DEBUG=1 _all_

build-tr: config
	$(MAKE) $(OPTIONS) THREADED=1 _all_

build-td: config
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 _all_

build-dr: config
	$(MAKE) $(OPTIONS) SHARED=1 _all_

build-dd: config
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 _all_


install-sr:
	$(MAKE) $(OPTIONS) _install_

install-sd:
	$(MAKE) $(OPTIONS) DEBUG=1 _install_

install-tr:
	$(MAKE) $(OPTIONS) THREADED=1 _install_

install-td:
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 _install_

install-dr:
	$(MAKE) $(OPTIONS) SHARED=1 _install_

install-dd:
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 _install_


clean-sr:
	$(MAKE) $(OPTIONS) _clean_

clean-sd:
	$(MAKE) $(OPTIONS) DEBUG=1 _clean_

clean-tr:
	$(MAKE) $(OPTIONS) THREADED=1 _clean_

clean-td:
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 _clean_

clean-dr:
	$(MAKE) $(OPTIONS) SHARED=1 _clean_

clean-dd:
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 _clean_



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
