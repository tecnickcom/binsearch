// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include "binsearch.h"

#define TEST_DATA_SIZE 11

typedef struct test_data_32_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint32_t search;
    uint64_t foundFirst32;
    uint64_t foundLast32;
} test_data_32_t;

typedef struct test_data_64_t
{
    uint64_t blkpos;
    uint64_t first;
    uint64_t last;
    uint64_t search;
    uint64_t foundFirst64;
    uint64_t foundLast64;
} test_data_64_t;

static test_data_32_t test_data_32[TEST_DATA_SIZE] =
{
    {4, 0, 99, 0x00002722, 0, 0},
    {4, 0, 99, 0x000033f5, 99, 99},
    {0, 0, 99, 0x00000001, 0, 99},
    {4, 0, 99, 0xf00027f3, 100, 100},
    {4, 0, 99, 0x000027f3, 13, 14},
    {4, 13, 99, 0x000027f3, 13, 14},
    {4, 14, 99, 0x000027f3, 14, 14},
    {4, 0, 0, 0x00000001, 1, 1},
    {4, 0, 0, 0xfffffff0, 1, 1},
    {4, 99, 99, 0x00000001, 100, 100},
    {4, 99, 99, 0xfffffff0, 100, 100},
};

static test_data_64_t test_data_64[TEST_DATA_SIZE] =
{
    {4, 0, 99, 0x000027225FB6E591, 0, 0},
    {4, 0, 99, 0x000033F522A78FD9, 99, 99},
    {0, 0, 99, 0x0000000100002722, 0, 0},
    {4, 0, 99, 0xf0000001f00027f3, 100, 100},
    {4, 0, 99, 0x000027F35FB6E591, 13, 13},
    {0, 13, 99, 0x00000001000027f3, 13, 14},
    {0, 14, 99, 0x00000001000027f3, 14, 14},
    {4, 0, 0, 0x0000000000000001, 1, 1},
    {4, 0, 0, 0xfffffffffffffff0, 1, 1},
    {4, 99, 99, 0x0000000000000001, 100, 100},
    {4, 99, 99, 0xfffffffffffffff0, 100, 100},
};

// returns current time in nanoseconds
uint64_t get_time()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (((uint64_t)t.tv_sec * 1000000000) + (uint64_t)t.tv_nsec);
}

int test_find_first_uint32be(mmfile_t mf, uint64_t blklen)
{
    int errors = 0;
    int i;
    uint64_t found;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        found = find_first_uint32be(mf.src, blklen, test_data_32[i].blkpos, test_data_32[i].first, test_data_32[i].last, test_data_32[i].search);
        if (found != test_data_32[i].foundFirst32)
        {
            fprintf(stderr, "test_find_first_uint32be (%d) Expected %"PRIx64", got %"PRIx64"\n", i, test_data_32[i].foundFirst32, found);
            ++errors;
        }
    }
    return errors;
}

int test_find_last_uint32be(mmfile_t mf, uint64_t blklen)
{
    int errors = 0;
    int i;
    uint64_t found;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        found = find_last_uint32be(mf.src, blklen, test_data_32[i].blkpos, test_data_32[i].first, test_data_32[i].last, test_data_32[i].search);
        if (found != test_data_32[i].foundLast32)
        {
            fprintf(stderr, "test_find_last_uint32be (%d) Expected %"PRIx64", got %"PRIx64"\n", i, test_data_32[i].foundLast32, found);
            ++errors;
        }
    }
    return errors;
}

int test_find_first_uint64be(mmfile_t mf, uint64_t blklen)
{
    int errors = 0;
    int i;
    uint64_t found;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        found = find_first_uint64be(mf.src, blklen, test_data_64[i].blkpos, test_data_64[i].first, test_data_64[i].last, test_data_64[i].search);
        if (found != test_data_64[i].foundFirst64)
        {
            fprintf(stderr, "test_find_first_uint64be (%d) Expected %"PRIx64", got %"PRIx64"\n", i, test_data_64[i].foundFirst64, found);
            ++errors;
        }
    }
    return errors;
}

int test_find_last_uint64be(mmfile_t mf, uint64_t blklen)
{
    int errors = 0;
    int i;
    uint64_t found;
    for (i=0 ; i < TEST_DATA_SIZE; i++)
    {
        found = find_last_uint64be(mf.src, blklen, test_data_64[i].blkpos, test_data_64[i].first, test_data_64[i].last, test_data_64[i].search);
        if (found != test_data_64[i].foundLast64)
        {
            fprintf(stderr, "test_find_last_uint64be (%d) Expected %"PRIx64", got %"PRIx64"\n", i, test_data_64[i].foundLast64, found);
            ++errors;
        }
    }
    return errors;
}

void benchmark_find_first_uint32be(mmfile_t mf, uint64_t blklen, uint64_t nitems)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_first_uint32be(mf.src, blklen, 4, 0, (nitems - 1), 0x000027f3);
    }
    tend = get_time();
    fprintf(stdout, " * find_first_uint32be : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_last_uint32be(mmfile_t mf, uint64_t blklen, uint64_t nitems)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_last_uint32be(mf.src, blklen, 4, 0, (nitems - 1), 0x000027f3);
    }
    tend = get_time();
    fprintf(stdout, " * find_last_uint32be : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_first_uint64be(mmfile_t mf, uint64_t blklen, uint64_t nitems)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_first_uint64be(mf.src, blklen, 4, 0, (nitems - 1), 0x000027F35FB6E591);
    }
    tend = get_time();
    fprintf(stdout, " * find_first_uint64be : %lu ns/op\n", (tend - tstart)/(size*4));
}

void benchmark_find_last_uint64be(mmfile_t mf, uint64_t blklen, uint64_t nitems)
{
    uint64_t tstart, tend;
    int i;
    int size = 100000;
    tstart = get_time();
    for (i=0 ; i < size; i++)
    {
        find_last_uint64be(mf.src, blklen, 4, 0, (nitems - 1), 0x000027F35FB6E591);
    }
    tend = get_time();
    fprintf(stdout, " * find_last_uint64be : %lu ns/op\n", (tend - tstart)/(size*4));
}

int main()
{
    int errors = 0;

    char *file = "test_data.bin"; // file containing test data
    uint64_t blklen = 20; // lenght of each binary block
    uint64_t nitems; // number of binary bloack in the file

    mmfile_t mf = mmap_binfile(file);

    if (mf.fd < 0)
    {
        fprintf(stderr, "can't open %s for reading\n", file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "fstat error! [%s]\n", strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "mmap error! [%s]\n", strerror(errno));
        return 1;
    }

    nitems = (uint64_t)(mf.size / blklen);
    if (nitems != 100)
    {
        fprintf(stderr, "Expecting 100 items, got instead: %"PRIu64"\n", nitems);
        return 1;
    }

    errors += test_find_first_uint32be(mf, blklen);
    errors += test_find_last_uint32be(mf, blklen);

    errors += test_find_first_uint64be(mf, blklen);
    errors += test_find_last_uint64be(mf, blklen);

    benchmark_find_first_uint32be(mf, blklen, nitems);
    benchmark_find_last_uint32be(mf, blklen, nitems);

    benchmark_find_first_uint64be(mf, blklen, nitems);
    benchmark_find_last_uint64be(mf, blklen, nitems);

    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the file\n", e);
        return 1;
    }

    return errors;
}
