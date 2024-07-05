# Imported from the original Dockerfile in the magma repository
export MAGMA_R=/magma
export MAGMA=/magma/magma
export OUT=/magma_out
export SHARED=/magma_shared

export LD=/usr/bin/ld
export AR=/usr/bin/ar
export AS=/usr/bin/as
export NM=/usr/bin/nm
export RANLIB=/usr/bin/ranlib

BUILD_FLAGS="-include ${MAGMA}/src/canary.h -DMAGMA_ENABLE_CANARIES -DMAGMA_FATAL_CANARIES -g -O0"

export CFLAGS=$BUILD_FLAGS
export CXXFLAGS=$BUILD_FLAGS
export LIBS="-l:magma.o -lrt"
export LDFLAGS="-L${OUT} -g"

# Prepare common object files in the output directory
rm -rf $OUT/*
clang++ -std=c++11 -c /benchmark/afl_driver.cpp -fPIC -o $OUT/afl_driver.o
clang -D"MAGMA_STORAGE=\"$MAGMA_STORAGE\"" -c "$MAGMA/src/canary.c" \
    -fPIC -I "$MAGMA/src/" -o "$OUT/canary.o" $LDFLAGS
clang -D"MAGMA_STORAGE=\"$MAGMA_STORAGE\"" -c "$MAGMA/src/storage.c" \
    -fPIC -I "$MAGMA/src/" -o "$OUT/storage.o" $LDFLAGS
ld -r "$OUT/canary.o" "$OUT/storage.o" -o "$OUT/magma.o"
rm "$OUT/canary.o" "$OUT/storage.o"
