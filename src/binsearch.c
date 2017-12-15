// Copyright (c) 2017 Nicola Asuni - Tecnick.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "binsearch.h"

mmfile_t mmap_binfile(const char *file)
{
    mmfile_t mf = {MAP_FAILED, -1, 0};
    struct stat statbuf;
    if ((mf.fd = open(file, O_RDONLY)) < 0) return mf;
    if (fstat(mf.fd, &statbuf) < 0) return mf;
    mf.size = (uint64_t)statbuf.st_size;
    mf.src = mmap(0, mf.size, PROT_READ, MAP_PRIVATE, mf.fd, 0);
    if (mf.src == MAP_FAILED) return mf;
    return mf;
}

int munmap_binfile(mmfile_t mf)
{
    int err = munmap(mf.src, mf.size);
    if (err != 0) return err;
    return close(mf.fd);
}

uint64_t get_address(uint64_t blklen, uint64_t blkpos, uint64_t item)
{
    return ((blklen * item) + blkpos);
}

uint8_t bytes_to_uint8_t(const unsigned char *src, uint64_t i)
{
    return ((uint8_t)src[i]);
}

uint16_t bytes_to_uint16_t(const unsigned char *src, uint64_t i)
{
    return (((uint32_t)src[i] << 8)
            | (uint32_t)src[i+1]);
}

uint32_t bytes_to_uint32_t(const unsigned char *src, uint64_t i)
{
    return (((uint32_t)src[i] << 24)
            | ((uint32_t)src[i+1] << 16)
            | ((uint32_t)src[i+2] << 8)
            | (uint32_t)src[i+3]);
}

uint64_t bytes_to_uint64_t(const unsigned char *src, uint64_t i)
{
    return (((uint64_t)src[i] << 56)
            | ((uint64_t)src[i+1] << 48)
            | ((uint64_t)src[i+2] << 40)
            | ((uint64_t)src[i+3] << 32)
            | ((uint64_t)src[i+4] << 24)
            | ((uint64_t)src[i+5] << 16)
            | ((uint64_t)src[i+6] << 8)
            | (uint64_t)src[i+7]);
}

uint128_t bytes_to_uint128_t(const unsigned char *src, uint64_t i)
{
    return (uint128_t)
    {
        .hi = bytes_to_uint64_t(src, i),
         .lo = bytes_to_uint64_t(src, i + 8)
    };
}
