#!/bin/bash
set -e

## ISLAB COMMENT ####
#### build.sh : This file contains codes to build this fuzzer.

##
# Pre-requirements:
# - env FUZZER: path to fuzzer work dir
##

# compile afl_driver.cpp
g++ $CXXFLAGS -std=c++11 -c "$FUZZER/src/afl_driver.cpp" -fPIC \
    -o "$OUT/afl_driver.o"
