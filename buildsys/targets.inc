all: build

build: build-default-default

clean: clean-default-default

install: install-default-default

###########################################################

build-default-default: build-$(TARGETMODE)-$(TARGETTYPE)

install-default-default: install-$(TARGETMODE)-$(TARGETTYPE)

clean-default-default: clean-$(TARGETMODE)-$(TARGETTYPE)

###########################################################

build-release-single: config
	$(MAKE) $(OPTIONS) build

build-debug-single: config
	$(MAKE) $(OPTIONS) DEBUG=1 build

build-release-multi: config
	$(MAKE) $(OPTIONS) THREADED=1 build

build-debug-multi: config
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 build

build-release-shared: config
	$(MAKE) $(OPTIONS) SHARED=1 build

build-debug-shared: config
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 build

###########################################################

install-release-single:
	$(MAKE) $(OPTIONS) install

install-debug-single:
	$(MAKE) $(OPTIONS) DEBUG=1 install

install-release-multi:
	$(MAKE) $(OPTIONS) THREADED=1 install

install-debug-multi:
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 install

install-release-shared:
	$(MAKE) $(OPTIONS) SHARED=1 install

install-debug-shared:
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 install

###########################################################

clean-release-single:
	$(MAKE) $(OPTIONS) clean

clean-debug-single:
	$(MAKE) $(OPTIONS) DEBUG=1 clean

clean-release-multi:
	$(MAKE) $(OPTIONS) THREADED=1 clean

clean-debug-multi:
	$(MAKE) $(OPTIONS) THREADED=1 DEBUG=1 clean

clean-release-shared:
	$(MAKE) $(OPTIONS) SHARED=1 clean

clean-debug-shared:
	$(MAKE) $(OPTIONS) SHARED=1 DEBUG=1 clean

###########################################################

config: $(USRMAKE) $(SYSCONFIG) $(USRCONFIG) 
