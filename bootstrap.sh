#! /bin/sh

aclocal \
&& automake-1.8 --add-missing \
&& autoconf