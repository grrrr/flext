#! /bin/sh

aclocal \
&& automake --foreign \
&& autoconf
