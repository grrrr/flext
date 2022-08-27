#! /bin/bash

if 
    [ -n "$1" -a -n "$2" ]
then 
    # make config first
    if
        bash ../build.sh $1 $2 config "PKGINFO=" "NAME=tutorial" "SRCS=\"\""
    then
        success=1
        
        # iterate through tutorial categories 1...8
        for c in `seq 1 8`; do
            for i in ${c}_* ; do
                if [ -e $i/package.txt ] ; then
                    pushd $i
                    if 
                        bash ../../build.sh $1 $2
                    then
                        true # we are ok!
                    else
                        # category 8 doesn't count, but other compilations need to succeed
                        if [ $c -lt 8 ]; then success=0; fi
                    fi
                    popd
                fi
            done
        done

        if [ $success -ne 0 ]; then
            ok=0
        else
            echo Compilation failed >/dev/stderr
            ok=-3
        fi
    else
        echo Config failed >/dev/stderr
        ok=-2
    fi
else
    echo 
    echo SYNTAX: build.sh [platform] [system] [compiler]
    echo system ..... pd / max
    echo compiler ... msvc / gcc / mingw / cygwin
    echo 
    echo Please make sure that your make program and compiler can be accessed with the
    echo system path and that all relevant environment variables are properly set.
    echo
    ok=-1
fi

exit $ok
