build-default-default: build-all-all

install-default-default: install-all-all

clean-default-default: clean-all-all

###################################################

build-all-all: build-release-all build-debug-all

build-release-all: build-release-single build-release-multi build-release-shared

build-debug-all: build-debug-single build-debug-multi build-debug-shared

###################################################

install-all-all: install-release-all install-debug-all

install-release-all: install-release-single install-release-multi install-release-shared

install-debug-all: install-debug-single install-debug-multi install-debug-shared

###################################################

clean-all-all: clean-release-all clean-debug-all

clean-release-all: clean-release-single clean-release-multi clean-release-shared

clean-debug-all: clean-debug-single clean-debug-multi clean-debug-shared
