/* Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tiffio.h>

/* stolen from tiffiop.h, which is a private header so we can't just include it */
/* safe multiply returns either the multiplied value or 0 if it overflowed */
#define __TIFFSafeMultiply(t, v, m) ((((t)(m) != (t)0) && (((t)(((v) * (m)) / (m))) == (t)(v))) ? (t)((v) * (m)) : (t)0)

const uint64 MAX_SIZE = 500000000;

void handle_error(const char *unused, const char *unused2, va_list unused3)
{
    return;
}

typedef struct
{
    uint8_t *data;
    size_t size;
    size_t offset;
} MemoryBuffer;

tsize_t readProc(thandle_t handle, tdata_t buf, tsize_t size)
{
    MemoryBuffer *mem = (MemoryBuffer *)handle;
    if (mem->offset + size > mem->size)
    {
        size = mem->size - mem->offset;
    }
    memcpy(buf, mem->data + mem->offset, size);
    mem->offset += size;
    return size;
}

tsize_t writeProc(thandle_t handle, tdata_t buf, tsize_t size)
{
    MemoryBuffer *mem = (MemoryBuffer *)handle;
    if (mem->offset + size > mem->size)
    {
        mem->data = (uint8_t *)realloc(mem->data, mem->offset + size);
        mem->size = mem->offset + size;
    }
    memcpy(mem->data + mem->offset, buf, size);
    mem->offset += size;
    return size;
}

toff_t seekProc(thandle_t handle, toff_t off, int whence)
{
    MemoryBuffer *mem = (MemoryBuffer *)handle;
    switch (whence)
    {
    case SEEK_SET:
        mem->offset = off;
        break;
    case SEEK_CUR:
        mem->offset += off;
        break;
    case SEEK_END:
        mem->offset = mem->size + off;
        break;
    default:
        return -1;
    }
    return mem->offset;
}

toff_t sizeProc(thandle_t handle)
{
    MemoryBuffer *mem = (MemoryBuffer *)handle;
    return mem->size;
}

int closeProc(thandle_t handle)
{
    MemoryBuffer *mem = (MemoryBuffer *)handle;
    mem->offset = 0;
    return 0;
}

int mapProc(thandle_t handle, tdata_t* base, toff_t* psize)
{
    MemoryBuffer *mem = (MemoryBuffer *) handle;
    *base = mem->data;
    *psize = mem->size;
    return (1);
}

void unmapProc(thandle_t handle, tdata_t base, toff_t size){
    return;
}

TIFF *TIFFOpenFromMemory(uint8_t *data, size_t size, const char *mode)
{
    MemoryBuffer *mem = (MemoryBuffer *)malloc(sizeof(MemoryBuffer));
    mem->data = data;
    mem->size = size;
    mem->offset = 0;

    return TIFFClientOpen("MemTIFF", mode, (thandle_t)mem,
                          readProc,
                          writeProc,
                          seekProc,
                          closeProc,
                          sizeProc,
                          mapProc,
                          unmapProc);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
#ifndef STANDALONE
    TIFFSetErrorHandler(handle_error);
    TIFFSetWarningHandler(handle_error);
#endif
#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
    // libjpeg-turbo has issues with MSAN and SIMD code
    // See https://bugs.chromium.org/p/oss-fuzz/issues/detail?id=7547
    // and https://github.com/libjpeg-turbo/libjpeg-turbo/pull/365
    setenv("JSIMD_FORCENONE", "1", 1);
#endif
#endif
    TIFF *tif = TIFFOpenFromMemory((uint8_t *)Data, Size, "r");
    if (!tif)
    {
        return 0;
    }
    uint32 w, h;
    uint32 *raster;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    /* don't continue if file size is ludicrous */
    if (TIFFTileSize64(tif) > MAX_SIZE)
    {
        TIFFClose(tif);
        return 0;
    }
    uint64 bufsize = TIFFTileSize64(tif);
    /* don't continue if the buffer size greater than the max allowed by the fuzzer */
    if (bufsize > MAX_SIZE || bufsize == 0)
    {
        TIFFClose(tif);
        return 0;
    }
    /* another hack to work around an OOM in tif_fax3.c */
    uint32 tilewidth = 0, tilewidth2;
    uint32 imagewidth = 0;
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tilewidth);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
    tilewidth2 = __TIFFSafeMultiply(uint32, tilewidth, 2);
    imagewidth = __TIFFSafeMultiply(uint32, imagewidth, 2);
    if (tilewidth2 * 2 > MAX_SIZE || imagewidth * 2 > MAX_SIZE || (tilewidth != 0 && tilewidth2 == 0) || imagewidth == 0)
    {
        TIFFClose(tif);
        return 0;
    }
    uint32 size = __TIFFSafeMultiply(uint32, w, h);
    if (size > MAX_SIZE || size == 0)
    {
        TIFFClose(tif);
        return 0;
    }
    raster = (uint32 *)_TIFFmalloc(size * sizeof(uint32));
    if (raster != NULL)
    {
        TIFFReadRGBAImage(tif, w, h, raster, 0);
        _TIFFfree(raster);
    }
    TIFFClose(tif);
    return 0;
}