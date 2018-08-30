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
#include "../src/binsearch.h"

int test_mmap_binfile_error(const char* file)
{
    mmfile_t mf = {0};
    mmap_binfile(file, &mf);
    if (mf.src != MAP_FAILED)
    {
        fprintf(stderr, "An mmap error was expected\n");
        return 1;
    }
    return 0;
}

int test_munmap_binfile_error()
{
    mmfile_t mf = {0};
    int e = munmap_binfile(mf);
    if (e == 0)
    {
        fprintf(stderr, "An mummap error was expected\n");
        return 1;
    }
    return 0;
}

int main()
{
    int errors = 0;

    errors += test_mmap_binfile_error("ERROR");
    errors += test_mmap_binfile_error("/dev/null");
    errors += test_munmap_binfile_error();

    char *file = "test_data.arrow"; // file containing test data

    mmfile_t mf = {0};
    mmap_binfile(file, &mf);

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

    if (mf.size != 730)
    {
        fprintf(stderr, "mf.size : Expecting 730 bytes, got instead: %" PRIu64 "\n", mf.size);
        errors++;
    }
    if (mf.doffset != 376)
    {
        fprintf(stderr, "mf.doffset : Expecting 376 bytes, got instead: %" PRIu64 "\n", mf.doffset);
        errors++;
    }
    if (mf.dlength != 136)
    {
        fprintf(stderr, "mf.dlength : Expecting 136 bytes, got instead: %" PRIu64 "\n", mf.dlength);
        errors++;
    }

    set_col_offset(&mf, 0, NULL);
    if (mf.nitems != 0)
    {
        fprintf(stderr, "mf.nitems (0): Expecting 0 items, got instead: %" PRIu64 "\n", mf.nitems);
        errors++;
    }

    uint8_t ncols = 2;
    uint8_t colbyte[] = {4, 8};
    set_col_offset(&mf, ncols, colbyte);

    if (mf.nitems != 11)
    {
        fprintf(stderr, "mf.nitems : Expecting 11 items, got instead: %" PRIu64 "\n", mf.nitems);
        errors++;
    }
    if (mf.index[0] != 376)
    {
        fprintf(stderr, "mf.index[0] : Expecting 376 bytes, got instead: %" PRIu64 "\n", mf.index[0]);
        errors++;
    }
    if (mf.index[1] != 424)
    {
        fprintf(stderr, "mf.index[1] : Expecting 424 bytes, got instead: %" PRIu64 "\n", mf.index[1]);
        errors++;
    }

    int e = munmap_binfile(mf);
    if (e != 0)
    {
        fprintf(stderr, "Got %d error while unmapping the file\n", e);
        return 1;
    }

    return errors;
}
