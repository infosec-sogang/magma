# Imported from the original Dockerfile in the magma repository
mkdir -p /magma_out
MAGMA=/magma/magma # Magma's base directory must be placed at this path
OUT=/magma_out
MAGMA_CFLAGS="-include ${MAGMA}/src/canary.h -DMAGMA_ENABLE_CANARIES -DMAGMA_FATAL_CANARIES -g -O0"
MAGMA_LDFLAGS="-L${OUT} -g"
MAGMA_LIBS="-l:magma.o -l:afl_driver.o -lrt"

# Prepare common object files in the output directory.
# - Arg1 : CFLAGS string argument
function setup_magma() {
  rm -rf $OUT/*
  export CFLAGS=$1
  $CC -c "$MAGMA/src/afl_driver.c" -fPIC -o $OUT/afl_driver.o
  $CC $CFLAGS -D"MAGMA_STORAGE=\"\"" -c "$MAGMA/src/canary.c" \
      -fPIC -I "$MAGMA/src/" -o "$OUT/canary.o" $LDFLAGS
  $CC $CFLAGS -D"MAGMA_STORAGE=\"\"" -c "$MAGMA/src/storage.c" \
      -fPIC -I "$MAGMA/src/" -o "$OUT/storage.o" $LDFLAGS
  $LD -r "$OUT/canary.o" "$OUT/storage.o" -o "$OUT/magma.o"
  rm "$OUT/canary.o" "$OUT/storage.o"
}
