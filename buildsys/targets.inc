all: build-default-default

clean: clean-default-default

install: install-default-default

###########################################################

build-default-default: build-$(TARGETMODE)-$(TARGETTYPE)

install-default-default: install-$(TARGETMODE)-$(TARGETTYPE)

clean-default-default: clean-$(TARGETMODE)-$(TARGETTYPE)

###########################################################

build-release-single: config
	$(MAKE) $(OPTIONS) _build_

build-debug-single: config
	$(MAKE) $(OPTIONS) DEBUG=1 _build_

build-release-multi: config
	$(MAKE) $(OPTIONS) THREADED=1 _build_

build-debug-multi: config
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 _build_

build-release-shared: config
	$(MAKE) $(OPTIONS) SHARED=1 _build_

build-debug-shared: config
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 _build_

###########################################################

install-release-single:
	$(MAKE) $(OPTIONS) _install_

install-debug-single:
	$(MAKE) $(OPTIONS) DEBUG=1 _install_

install-release-multi:
	$(MAKE) $(OPTIONS) THREADED=1 _install_

install-debug-multi:
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 _install_

install-release-shared:
	$(MAKE) $(OPTIONS) SHARED=1 _install_

install-debug-shared:
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 _install_

###########################################################

clean-release-single:
	$(MAKE) $(OPTIONS) _clean_

clean-debug-single:
	$(MAKE) $(OPTIONS) DEBUG=1 _clean_

clean-release-multi:
	$(MAKE) $(OPTIONS) THREADED=1 _clean_

clean-debug-multi:
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 _clean_

clean-release-shared:
	$(MAKE) $(OPTIONS) SHARED=1 _clean_

clean-debug-shared:
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 _clean_

###########################################################

config: $(USRMAKE) $(SYSCONFIG) $(USRCONFIG) 
