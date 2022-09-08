#!/bin/bash

# This script tests the tutorial externals in gitlab CI

root="$1"
pd="${2:-pd}"
patches="${root}/tutorial/pd"
testpatch="${patches}/test.pd"
logfile="${patches}/test.log"

for f in "$root"/tutorial/?_*/*.pd_*; do
  p="${f%/*}"
  pp="${p##*_}"
  ext="${f##*/}"
  n="${ext%%.*}"
  ex="${patches}/ex-${pp}"
  echo Testing object $n with example patch $ex
  if ! [ -f ${ex}.pd ]; then
    echo Example patch $ex not found, testing mere object $n
    ex=$n
  fi
  if
    ! "$pd" -nort -noprefs -noaudio -nomidi -nogui -path "$p" -path "$patches" -open "$testpatch" -send "r obj $ex" -send "r del 100" -send "r quit 1000" &> "$logfile" || grep error "$logfile" 
  then
    exit -1
  fi
done
