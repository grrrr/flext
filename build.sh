#! /bin/bash

# flext - C++ layer for Max/MSP and pd (pure data) externals
#
# Copyright (c) 2001-2005 Thomas Grill (gr@grrrr.org)
# For information on usage and redistribution, and for a DISCLAIMER OF ALL
# WARRANTIES, see the file, "license.txt," in this distribution.  
#
# more information on http://grrrr.org/ext
# ------------------------------------------------------------------------
#
# To build flext or flext-based externals simply run this script.
# Running it without arguments will print some help to the console.
#
# ------------------------------------------------------------------------

flext=${0%/*}/
if [ "$flext" = "$0"/ ]; then flext=./ ; fi

# Arguments:
# $1 - platform (win/lnx/mac)
# $2 - system (pd/max)
# $3 - compiler (msvc/gcc/mingw/cygwin/bcc/icc)
# $4 - target (build/clean/install)

platform=$1
rtsys=$2
compiler=$3
target=$4

# --- The subbatch knowns which make utility to use ---
subbatch=${flext}buildsys/build-${compiler}.sh

if 
	[ -n "$platform" -a -n "$rtsys" -a -n "$compiler" -a -f $subbatch ]
then 
	bash $subbatch $platform $rtsys $target $5 $6 $7 $8 $9
else
	echo 
	echo SYNTAX: build.sh [platform] [system] [compiler] {target}
	echo platform ... win / lnx / mac
	echo system ..... pd / max
	echo compiler ... msvc / gcc / mingw / cygwin / bcc / icc
	echo target ..... build \(default\) / clean / install
	echo 
	echo Please make sure that your make program and compiler can be accessed with the
	echo system path and that all relevant environment variables are properly set.
	echo
fi

