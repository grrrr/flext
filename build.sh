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
	sh $subbatch $platform $rtsys $target $5 $6 $7 $8 $9
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
fi

