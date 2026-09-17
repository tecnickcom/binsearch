// Copyright (c) 2017-2026 Nicola Asuni - Tecnick.com
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

// NOTE: This test is slow because it generates the test file from scratch.
//
// The searched values are drawn in a scrambled order. A benchmark that searches
// for consecutive values walks nearly the same path on every call, so both the
// branch predictor and the cache see a pattern that a real workload does not
// produce, and the reported cost is a fraction of the real one.

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

#define TEST_DATA_SIZE 10000000ULL //!< Number of rows of the test file
#define NQUERIES 1000000ULL        //!< Number of searches per measured round
#define ROUNDS 3                   //!< Number of measured rounds
#define WRITE_CHUNK 65536ULL       //!< Number of rows written per fwrite call

#define TEST_FILE "test.bin" //!< Name of the generated test file

// Returns the current time in nanoseconds.
static uint64_t get_time(void)
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

// Returns the next value of a 64-bit linear congruential generator.
static uint64_t next_random(uint64_t *state)
{
    *state = ((*state * 6364136223846793005ULL) + 1442695040888963407ULL);
    return (*state >> 33);
}

// Returns a buffer of values scattered over the range of the test data, with
// about one in sixteen falling above it so that the not-found path is measured
// as well.
static uint64_t *make_queries(void)
{
    uint64_t *q = (uint64_t *)malloc(NQUERIES * sizeof(uint64_t));
    if (q == NULL)
    {
        return NULL;
    }
    uint64_t state = 12345;
    uint64_t i = 0;
    for (i = 0; i < NQUERIES; i++)
    {
        q[i] = (next_random(&state) % (TEST_DATA_SIZE + (TEST_DATA_SIZE / 16)));
    }
    return q;
}

// Writes the test file as TEST_DATA_SIZE 8-byte blocks holding the row number.
// When "bigendian" is set the value is stored in the last 4 bytes in big-endian
// order, otherwise in the first 4 bytes in little-endian order.
static int write_test_file(int bigendian)
{
    FILE *f = fopen(TEST_FILE, "we");
    if (f == NULL)
    {
        (void)fprintf(stderr, " * Unable to open %s in writing mode.\n", TEST_FILE);
        return 1;
    }
    uint8_t *buf = (uint8_t *)malloc(WRITE_CHUNK * 8);
    if (buf == NULL)
    {
        (void)fclose(f);
        return 1;
    }
    uint64_t i = 0;
    uint64_t n = 0;
    int err = 0;
    for (i = 0; i < TEST_DATA_SIZE; i++)
    {
        uint8_t *b = (buf + (n * 8));
        memset(b, 0, 8);
        if (bigendian)
        {
            b[4] = (uint8_t)(i >> 24);
            b[5] = (uint8_t)(i >> 16);
            b[6] = (uint8_t)(i >> 8);
            b[7] = (uint8_t)i;
        }
        else
        {
            b[0] = (uint8_t)i;
            b[1] = (uint8_t)(i >> 8);
            b[2] = (uint8_t)(i >> 16);
            b[3] = (uint8_t)(i >> 24);
        }
        n++;
        if (n == WRITE_CHUNK)
        {
            if (fwrite(buf, 8, (size_t)n, f) != (size_t)n)
            {
                err = 1;
                break;
            }
            n = 0;
        }
    }
    if ((err == 0) && (n > 0) && (fwrite(buf, 8, (size_t)n, f) != (size_t)n))
    {
        err = 1;
    }
    free(buf);
    if (fclose(f) != 0)
    {
        err = 1;
    }
    if (err != 0)
    {
        (void)fprintf(stderr, " * Unable to write %s.\n", TEST_FILE);
    }
    return err;
}

// Maps the test file and checks that it holds the expected number of rows.
static int map_test_file(mmfile_t *mf)
{
    memset(mf, 0, sizeof(*mf));
    mf->ncols = 1;
    mf->ctbytes[0] = 8;
    mmap_binfile(TEST_FILE, mf);
    if (mf->nrows != TEST_DATA_SIZE)
    {
        (void)fprintf(stderr, " * Expecting %s %" PRIu64 " items, got instead: %" PRIu64 "\n",
                      TEST_FILE, (uint64_t)TEST_DATA_SIZE, mf->nrows);
        return 1;
    }
    return 0;
}

// Reports the result of one measured round.
static void report(const char *name, int round, uint64_t sum, uint64_t elapsed)
{
    (void)fprintf(stdout, "   * %s %d. sum: %" PRIu64 " -- time: %" PRIu64 " ns -- %" PRIu64 " ns/op\n",
                  name, round, sum, elapsed, (uint64_t)(elapsed / NQUERIES));
}

// Defines the measured rounds of a row mode search.
#define BENCH_ROW(NAME, FUNC, T) \
static int NAME(const uint64_t *queries) \
{ \
    mmfile_t mf; \
    if (map_test_file(&mf) != 0) \
    { \
        return 1; \
    } \
    const uint64_t lastitem = mf.nrows; \
    int j = 0; \
    for (j = 0; j < ROUNDS; j++) \
    { \
        uint64_t sum = 0; \
        const uint64_t tstart = get_time(); \
        uint64_t i = 0; \
        for (i = 0; i < NQUERIES; i++) \
        { \
            uint64_t first = 0; \
            uint64_t last = lastitem; \
            sum += FUNC(mf.src, 8, 0, &first, &last, (T)queries[i]); \
        } \
        const uint64_t tend = get_time(); \
        report(#FUNC, j, sum, (tend - tstart)); \
    } \
    if (munmap_binfile(&mf) != 0) \
    { \
        (void)fprintf(stderr, " * %s Unable to unmap %s.\n", #FUNC, TEST_FILE); \
        return 1; \
    } \
    return 0; \
}

// Defines the measured rounds of a column mode search.
#define BENCH_COL(NAME, FUNC, T) \
static int NAME(const uint64_t *queries) \
{ \
    mmfile_t mf; \
    if (map_test_file(&mf) != 0) \
    { \
        return 1; \
    } \
    const uint64_t lastitem = mf.nrows; \
    const T *src = get_src_offset_##T(mf.src, mf.index[0]); \
    int j = 0; \
    for (j = 0; j < ROUNDS; j++) \
    { \
        uint64_t sum = 0; \
        const uint64_t tstart = get_time(); \
        uint64_t i = 0; \
        for (i = 0; i < NQUERIES; i++) \
        { \
            uint64_t first = 0; \
            uint64_t last = lastitem; \
            sum += FUNC(src, &first, &last, (T)queries[i]); \
        } \
        const uint64_t tend = get_time(); \
        report(#FUNC, j, sum, (tend - tstart)); \
    } \
    if (munmap_binfile(&mf) != 0) \
    { \
        (void)fprintf(stderr, " * %s Unable to unmap %s.\n", #FUNC, TEST_FILE); \
        return 1; \
    } \
    return 0; \
}

BENCH_ROW(benchmark_find_first_be_uint64, find_first_be_uint64_t, uint64_t)
BENCH_ROW(benchmark_find_first_le_uint64, find_first_le_uint64_t, uint64_t)
BENCH_COL(benchmark_col_find_first_le_uint64, col_find_first_le_uint64_t, uint64_t)

int main(void)
{
    uint64_t *queries = make_queries();
    if (queries == NULL)
    {
        (void)fprintf(stderr, " * Unable to allocate the query buffer.\n");
        return 1;
    }
    int ret = 0;
    // The big-endian layout needs its own file, the other two share one.
    if (write_test_file(1) == 0)
    {
        ret |= benchmark_find_first_be_uint64(queries);
    }
    else
    {
        ret = 1;
    }
    if (write_test_file(0) == 0)
    {
        ret |= benchmark_find_first_le_uint64(queries);
        ret |= benchmark_col_find_first_le_uint64(queries);
    }
    else
    {
        ret = 1;
    }
    (void)remove(TEST_FILE);
    free(queries);
    return ret;
}
