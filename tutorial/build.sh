#! /bin/bash

if 
	[ -n "$1" -a -n "$2" -a -n "$3" ]
then 
	# make config first
	if
		bash ../build.sh $1 $2 $3 config "PKGINFO="
	then
		for i in * ; do
			if [ -e $i/package.txt ] ; then
				pushd $i
				bash ../../build.sh $1 $2 $3
				popd
			fi
		done
	fi
else
	echo 
	echo SYNTAX: build.sh [platform] [system] [compiler]
	echo platform ... win / lnx / mac
	echo system ..... pd / max
	echo compiler ... msvc / gcc / mingw / cygwin / bcc / icc
	echo 
	echo Please make sure that your make program and compiler can be accessed with the
	echo system path and that all relevant environment variables are properly set.
	echo
fi
