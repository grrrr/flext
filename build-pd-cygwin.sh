#!/bin/sh

. config-pd-cygwin.txt

make -f makefile.pd-cygwin &&
{
	if [ $INSTDIR != "" ]; then
		make -f makefile.pd-cygwin install
	fi
}
