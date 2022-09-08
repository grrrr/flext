#!/bin/bash

# This script tests the tutorial externals in gitlab CI

root="$1"
pd="${2:-pd}"
pdroot="${root}/tutorial/pd"
testpatch="${pdroot}/test.pd"
logfile="${pdroot}/test.log"

if ! [ -x "$pd" ]; then
  echo Pure data not found
  exit -1
fi

for f in "$root"/tutorial/{1_simple1,2_adv2,3_*,4_*,5_*,7_*}/*.pd_*; do
  p="${f%/*}"
  pp="${p##*_}"
  ext="${f##*/}"
  n="${ext%%.*}"
  ex="${pdroot}/ex-${pp}"
  echo Testing object $n with example patch $ex
  if ! [ -f ${ex}.pd ]; then
    echo Example patch $ex not found, testing mere object $n
    ex=$n
  fi
  if
    ! "$pd" -nort -noprefs -noaudio -nomidi -nogui -path "$p" -path "$pdroot" -open "$testpatch" -send "r obj $ex" -send "r del 100" -send "r quit 1000" &> "$logfile" || grep error "$logfile" 
  then
    exit -1
  fi
done
