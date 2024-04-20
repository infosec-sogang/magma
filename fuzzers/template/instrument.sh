#!/bin/bash
set -e

## ISLAB COMMENT ####
#### instrument.sh : This file contains codes \
#### to build magma.o and \
#### to build target library

##
# Pre-requirements:
# - env FUZZER: path to fuzzer work dir
# - env TARGET: path to target work dir
# - env MAGMA: path to Magma support files
# - env OUT: path to directory where artifacts are stored
# - env CFLAGS and CXXFLAGS must be set to link against Magma instrumentation
##

export LIBS="$LIBS -l:afl_driver.o -lstdc++"

export TARGET="$TARGET_libpng"
"$MAGMA/build.sh"
"$TARGET/build.sh"
export TARGET="$TARGET_libtiff"
"$MAGMA/build.sh"
"$TARGET/build.sh"
export TARGET="$TARGET_libxml2"
"$MAGMA/build.sh"
"$TARGET/build.sh"
export TARGET="$TARGET_openssl"
"$MAGMA/build.sh"
"$TARGET/build.sh"
export TARGET="$TARGET_sqlite"
"$MAGMA/build.sh"
"$TARGET/build.sh"
export TARGET="$TARGET_php"
"$MAGMA/build.sh"
"$TARGET/build.sh"

# NOTE: We pass $OUT directly to the target build.sh script, since the artifact
#       itself is the fuzz target. In the case of Angora, we might need to
#       replace $OUT by $OUT/fast and $OUT/track, for instance.
