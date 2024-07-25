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
export LIBS="-l:magma.o -l:afl_driver.o -lrt"
export LDFLAGS="-L${OUT} -g"

# Prepare common object files in the output directory. While the original Magma
# repository uses fuzzer-specific $CC and $CFLAGS to build these binaries, it
# won't hurt to compile these with a common compiler (clang) and flags.
rm -rf $OUT/*
clang -c "$MAGMA/src/afl_driver.c" -fPIC -o $OUT/afl_driver.o
clang $CFLAGS -D"MAGMA_STORAGE=\"$MAGMA_STORAGE\"" -c "$MAGMA/src/canary.c" \
    -fPIC -I "$MAGMA/src/" -o "$OUT/canary.o" $LDFLAGS
clang $CFLAGS -D"MAGMA_STORAGE=\"$MAGMA_STORAGE\"" -c "$MAGMA/src/storage.c" \
    -fPIC -I "$MAGMA/src/" -o "$OUT/storage.o" $LDFLAGS
ld -r "$OUT/canary.o" "$OUT/storage.o" -o "$OUT/magma.o"
rm "$OUT/canary.o" "$OUT/storage.o"
