# This env will be used in other project
# For example, DAFL
export MAGMA_R=/magma
export MAGMA=/magma/magma
export OUT=/magma_out
export SHARED=/magma_shared

export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
export LD=/usr/bin/ld
export AR=/usr/bin/ar
export AS=/usr/bin/as
export NM=/usr/bin/nm
export RANLIB=/usr/bin/ranlib

BUILD_FLAGS="-include ${MAGMA}/src/canary.h -DMAGMA_ENABLE_CANARIES -DMAGMA_FATAL_CANARIES -g -O0"

export CFLAGS=${BUILD_FLAGS}
export CXXFLAGS=${BUILD_FLAGS}
export LIBS="-l:magma.o -lrt"
export LDFLAGS="-L${OUT} -g"