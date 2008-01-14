#! /bin/sh

# MacOS has no libtoolize... use glibtoolize instead
glibtoolize --force && \
aclocal && \
automake --foreign --add-missing && \
autoconf
