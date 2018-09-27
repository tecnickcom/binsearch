// Copyright (c) 2017-2018 Nicola Asuni - Tecnick.com
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

// NOTE: This test is slow because it generates the test files from scratch.

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include "../../src/binsearch/binsearch.h"

#define TEST_DATA_SIZE 10000000ULL

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int benchmark_find_first_be_uint64()
{
    const char *filename = "test.bin";

    uint64_t i;

    FILE *f = fopen(filename, "we");
    if (f == NULL)
    {
        fprintf(stderr, " * %s Unable to open %s file in writing mode.\n", __func__, filename);
        return 1;
    }
    uint8_t b0, b1, b2, b3, z = 0;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        b0 = i & 0xFF;
        b1 = (i >> 8) & 0xFF;
        b2 = (i >> 16) & 0xFF;
        b3 = (i >> 24) & 0xFF;
        fprintf(f, "%c%c%c%c%c%c%c%c", z, z, z, z, b3, b2, b1, b0);
    }
    fclose(f);

    mmfile_t mf = {0};
    mf.nrows = TEST_DATA_SIZE;
    mf.ncols = 1;
    mf.ctbytes[0] = 8;
    mmap_binfile(filename, &mf);
    if (mf.nrows != TEST_DATA_SIZE)
    {
        fprintf(stderr, " * %s Expecting test.bin %" PRIu64 " items, got instead: %" PRIu64 "\n", __func__, (uint64_t)TEST_DATA_SIZE, mf.nrows);
        return 1;
    }

    uint64_t tstart, tend, offset;
    volatile uint64_t sum = 0;
    uint64_t first, last;
    uint64_t lastitem = mf.nrows;

    tstart = get_time();
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        last = lastitem;
        sum += i;
    }
    tend = get_time();
    offset = (tend - tstart);
    fprintf(stdout, " * %s sum: %" PRIu64 "\n", __func__, sum);

    int j;
    for (j=0 ; j < 3; j++)
    {
        sum = 0;
        tstart = get_time();
        for (i=0 ; i < TEST_DATA_SIZE; i++)
        {
            first = 0;
            last = lastitem;
            sum += find_first_be_uint64_t(mf.src, 8, 0, &first, &last, i);
        }
        tend = get_time();
        fprintf(stdout, "   * %s %d. sum: %" PRIu64 " -- time: %" PRIu64 " ns -- %" PRIu64 " ns/op\n", __func__, j, sum, (tend - tstart - offset), (tend - tstart - offset)/(uint64_t)TEST_DATA_SIZE);
    }
    return 0;
}

int benchmark_find_first_le_uint64()
{
    const char *filename = "test.bin";

    uint64_t i;

    FILE *f = fopen(filename, "we");
    if (f == NULL)
    {
        fprintf(stderr, " * %s Unable to open %s file in writing mode.\n", __func__, filename);
        return 1;
    }
    uint8_t b0, b1, b2, b3, z = 0;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        b0 = i & 0xFF;
        b1 = (i >> 8) & 0xFF;
        b2 = (i >> 16) & 0xFF;
        b3 = (i >> 24) & 0xFF;
        fprintf(f, "%c%c%c%c%c%c%c%c", b0, b1, b2, b3, z, z, z, z);
    }
    fclose(f);

    mmfile_t mf = {0};
    mf.nrows = TEST_DATA_SIZE;
    mf.ncols = 1;
    mf.ctbytes[0] = 8;
    mmap_binfile(filename, &mf);
    if (mf.nrows != TEST_DATA_SIZE)
    {
        fprintf(stderr, " * %s Expecting test.bin %" PRIu64 " items, got instead: %" PRIu64 "\n", __func__, (uint64_t)TEST_DATA_SIZE, mf.nrows);
        return 1;
    }

    uint64_t tstart, tend, offset;
    volatile uint64_t sum = 0;
    uint64_t first, last;
    uint64_t lastitem = mf.nrows;

    tstart = get_time();
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        last = lastitem;
        sum += i;
    }
    tend = get_time();
    offset = (tend - tstart);
    fprintf(stdout, " * %s sum: %" PRIu64 "\n", __func__, sum);

    int j;
    for (j=0 ; j < 3; j++)
    {
        sum = 0;
        tstart = get_time();
        for (i=0 ; i < TEST_DATA_SIZE; i++)
        {
            first = 0;
            last = lastitem;
            sum += find_first_le_uint64_t(mf.src, 8, 0, &first, &last, i);
        }
        tend = get_time();
        fprintf(stdout, "   * %s %d. sum: %" PRIu64 " -- time: %" PRIu64 " ns -- %" PRIu64 " ns/op\n", __func__, j, sum, (tend - tstart - offset), (tend - tstart - offset)/(uint64_t)TEST_DATA_SIZE);
    }
    return 0;
}

int benchmark_col_find_first_uint64()
{
    const char *filename = "test.bin";

    uint64_t i;

    FILE *f = fopen(filename, "we");
    if (f == NULL)
    {
        fprintf(stderr, " * %s Unable to open %s file in writing mode.\n", __func__, filename);
        return 1;
    }
    uint8_t b0, b1, b2, b3, z = 0;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        b0 = i & 0xFF;
        b1 = (i >> 8) & 0xFF;
        b2 = (i >> 16) & 0xFF;
        b3 = (i >> 24) & 0xFF;
        fprintf(f, "%c%c%c%c%c%c%c%c", b0, b1, b2, b3, z, z, z, z);
    }
    fclose(f);

    mmfile_t mf = {0};
    mf.nrows = TEST_DATA_SIZE;
    mf.ncols = 1;
    mf.ctbytes[0] = 8;
    mmap_binfile(filename, &mf);
    if (mf.nrows != TEST_DATA_SIZE)
    {
        fprintf(stderr, " * %s Expecting test.bin %" PRIu64 " items, got instead: %" PRIu64 "\n", __func__, (uint64_t)TEST_DATA_SIZE, mf.nrows);
        return 1;
    }

    uint64_t tstart, tend, offset;
    volatile uint64_t sum = 0;
    uint64_t first, last;
    uint64_t lastitem = mf.nrows;

    tstart = get_time();
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        first = 0;
        last = lastitem;
        sum += i;
    }
    tend = get_time();
    offset = (tend - tstart);
    fprintf(stdout, " * %s sum: %" PRIu64 "\n", __func__, sum);

    const uint64_t *src = (const uint64_t *)(mf.src);

    int j;
    for (j=0 ; j < 3; j++)
    {
        sum = 0;
        tstart = get_time();
        for (i=0 ; i < TEST_DATA_SIZE; i++)
        {
            first = 0;
            last = lastitem;
            sum += col_find_first_uint64_t(src, &first, &last, i);
        }
        tend = get_time();
        fprintf(stdout, "   * %s %d. sum: %" PRIu64 " -- time: %" PRIu64 " ns -- %" PRIu64 " ns/op\n", __func__, j, sum, (tend - tstart - offset), (tend - tstart - offset)/(uint64_t)TEST_DATA_SIZE);
    }
    return 0;
}

int main()
{
    benchmark_find_first_be_uint64();
    benchmark_find_first_le_uint64();
    benchmark_col_find_first_uint64();
    return 0;
}
