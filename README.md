# Binsearch

*Fast binary search for columnar data formats.*

[![Sponsor on GitHub](https://img.shields.io/badge/sponsor-github-EA4AAA.svg?logo=githubsponsors&logoColor=white)](https://github.com/sponsors/tecnickcom)

If this project is useful to you, please consider [supporting development via GitHub Sponsors](https://github.com/sponsors/tecnickcom).

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **author**      Nicola Asuni
* **copyright**   2017-2026 Nicola Asuni - Tecnick.com
* **link**        https://github.com/tecnickcom/binsearch


## What this library does

Binsearch finds sorted unsigned integers inside a memory mapped binary file. The bytes are read where they lie: nothing is parsed, decoded or copied into the process, so a 72 GB lookup table needs no space in the process heap and answers its first query as soon as it is mapped. Two processes searching the same file share one copy of it in the page cache.

It targets data produced once and read many times: index files, lookup tables, the sorted key column of a columnar dataset. [VariantKey](https://github.com/tecnickcom/variantkey) uses it to resolve rsIDs and variant keys against files of billions of rows.

The reference implementation is one C header that needs nothing but libc. The Go package and the Python extension compile that same header, so all three behave alike and differ only by the cost of their language boundary.


## Features

* Two layouts: **row mode**, where the searched value sits at a fixed offset inside constant-length records, and **column mode**, where it is a contiguous array of one type.
* `uint8`, `uint16`, `uint32` and `uint64` values, big-endian or little-endian, whatever the byte order of the host.
* Lower bound, upper bound, or the half-open range of the items equal to the searched value. That range costs one descent, not two.
* Bit range matching for keys that pack several fields into one integer. Searching the top 33 bits of a 64-bit VariantKey finds every variant at a chromosome and position, with no unpacking and no separate index.
* Batch search of many values in one call: the searches advance in lockstep so their cache misses overlap.
* Page requests issued ahead of a batch, so a file larger than RAM stays usable.
* `mmap_binfile` reads the header of BINSRC1, Apache Arrow (single RecordBatch) and Feather files. Any other content is described by the caller.
* The Go and Python searches check every argument against the mapping first, so a wrong offset or bound returns an error instead of reading outside the file.
* A mapped file can be searched from several threads or goroutines at once.
* One header for C, a Go package and a Python extension, each with its own test suite: `make coverage` fails below 100% of lines, functions and branches in C and Python, and below 100% of statements in Go.


## Example

All three snippets below search `c/test/data/test_data_binsrc.bin`, a BINSRC1 file of 11 rows whose first column holds the `uint32` values 1, 7, 11, 97, 101, 997, 1009, 9973, 99999, 104729, 104729. The last value appears twice, so it makes a range of two items. The comments show what each call returns.

```c
#include <inttypes.h>
#include <stdio.h>
#include "binsearch.h"

int main(void)
{
    mmfile_t mf;
    mmap_binfile("test_data_binsrc.bin", &mf);
    if (mf.fd < 0)
    {
        return 1;
    }

    // A BINSRC1 header describes its own columns: 11 rows, first column uint32.
    const uint32_t *col = get_src_offset_uint32_t(mf.src, mf.index[0]);

    uint64_t first = 0;
    uint64_t last = mf.nrows;
    uint64_t n = col_find_range_le_uint32_t(col, &first, &last, 104729);
    printf("%" PRIu64 " in [%" PRIu64 ",%" PRIu64 ")\n", n, first, last); // 2 in [9,11)

    const uint32_t search[3] = {7, 101, 123456};
    uint64_t pos[3] = {0};
    col_find_many_le_uint32_t(col, 0, mf.nrows, search, pos, 3, mf.prefetch);
    // pos is now 1, 4, 11; the last value is absent, so its position is "last"

    return munmap_binfile(&mf);
}
```

Build it against the header in the repository, or against the copy `make install` puts under `usr/include/tecnickcom/binsearch`:

```
gcc -O3 -std=c2x -D_DEFAULT_SOURCE -I c/src/binsearch -o example example.c
```

```go
package main

import (
	"fmt"
	"log"

	binsearch "github.com/tecnickcom/binsearch/go/src"
)

func main() {
	// A BINSRC1 file carries its own column description, so none is passed here.
	mf, err := binsearch.MmapBinFile("test_data_binsrc.bin", nil)
	if err != nil {
		log.Fatal(err)
	}
	defer mf.Close()

	n, first, last, err := mf.ColFindRangeLEUint32(mf.Index[0], 0, mf.NRows, 104729)
	fmt.Println(n, first, last, err) // 2 9 11 <nil>

	pos, err := mf.ColFindManyLEUint32(mf.Index[0], 0, mf.NRows, []uint32{7, 101, 123456})
	fmt.Println(pos, err) // [1 4 11] <nil>
}
```

```python
import binsearch as bs

src, fd, size, doffset, dlength, nrows, ncols, border, index, ctbytes = bs.mmap_binfile(
    "test_data_binsrc.bin", []
)

print(bs.col_find_range_le_uint32(src, index[0], 0, nrows, 104729))         # (2, 9, 11)
print(bs.col_find_many_le_uint32(src, index[0], 0, nrows, [7, 101, 123456]))  # [1, 4, 11]

bs.munmap_binfile(src, fd, size)
```


## Why not bsearch, sort.Search or bisect

A binary search is a dozen lines of code, so the question is what those lines cost and what they leave out.

`bsearch` calls its comparison through a function pointer at every step, so it can neither inline the comparison nor compile the step without a branch. `bisect_left` compares Python objects and boxes an `int` out of an `array.array` at every step. The loops here are specialised on the type and branchless, and the batch variants keep several descents in flight so their cache misses overlap.

Neither standard library searches a file, and neither offers the bit range match, the choice of byte order, the row layout, the bounds or the batch call.

10,000,000 unique `uint32` values in ascending order, column layout, 38 MiB, larger than the 18 MiB L3 of the test machine:

| | ns per search | vs baseline |
|---|---|---|
| C, `bsearch` | 193 | 1.00x |
| C, `col_find_first` | 110 | **1.75x** |
| C, `col_find_many` | 50 | **3.86x** |
| Go, `sort.Search` over the same mapping | 245 | 1.00x |
| Go, `ColFindFirstLEUint32` | 248 | 0.99x |
| Go, `ColFindManyLEUint32` | 59 | **4.15x** |
| Python, `bisect_left` on an `array.array` | 458 | 1.00x |
| Python, `col_find_first` | 356 | **1.29x** |
| Python, `col_find_many` | 78 | **5.87x** |

Same data at 100,000 rows, 390 KiB, small enough for L2:

| | ns per search | vs baseline |
|---|---|---|
| C, `bsearch` | 73 | 1.00x |
| C, `col_find_first` | 16 | **4.56x** |
| C, `col_find_many` | 6 | **12.2x** |
| Go, `sort.Search` over the same mapping | 77 | 1.00x |
| Go, `ColFindFirstLEUint32` | 45 | **1.71x** |
| Go, `ColFindManyLEUint32` | 7.2 | **10.7x** |
| Python, `bisect_left` on an `array.array` | 266 | 1.00x |
| Python, `col_find_first` | 106 | **2.51x** |
| Python, `col_find_many` | 21 | **12.7x** |

Measured on a 12th Gen Intel i7-1260P with 30 GiB of RAM, gcc 14.2.0 `-O3 -std=c2x`, go 1.27.1, CPython 3.13.5, Linux 6.12. Median of the measured rounds, queries drawn in a scrambled order with about one in seventeen falling outside the data. The Go and Python baselines search the same memory mapping rather than a heap copy, so the cost of mapping the file is charged to both sides. [c/test/bench/bench.c](c/test/bench/bench.c) measures the row mode functions of the library on a generated file of 10,000,000 rows.

Some of those numbers are warnings rather than selling points.

A single Go search on data larger than the cache is level with `sort.Search`, 248 ns against 245. The cgo crossing costs about 29 ns whatever the data size, and the 100,000 row case prices it: 16 ns in C, 45 ns through the Go method. At 10,000,000 rows the memory stalls both sides share leave less than that to win. `ColFindMany` crosses once for a whole slice and is the only Go path ahead of the standard library at that size.

At 3,000,000,000 rows a single search is level with `bsearch` too, 1223 ns against 1208, because by then every step misses both the cache and the TLB and the comparator call hides behind the stall. `col_find_many` keeps its factor of 3 to 4 at every size from 10 million to 3 billion rows, as long as the working set is in RAM.

Memory mapping is not free either. At 10,000,000 rows the same algorithm takes 179 ns over a Go heap slice and 245 ns over the mapping. Transparent huge pages back the anonymous heap while 4 KiB pages back the file, so 38 MiB is 19 TLB entries in one case and 9,728 in the other. That 27% belongs to any search over a mapping, and it is the price of not loading the file.


## Row and column layouts

In row mode the file is a sequence of adjacent constant-length blocks, each holding the searched value at a fixed offset. The 8-byte blocks below start with a `uint32` in big-endian, and the blocks are sorted by that value:

```
2f 81 f5 77 1a cc 7b 43
2f 81 f5 78 76 5f 63 b8
2f 81 f5 79 ca a9 a6 52
```

This representation can encode sortable key-value data, even with nested keys. A search takes `blklen`, the length of a block, and `blkpos`, the byte offset of the value inside it.

In column mode the data is a contiguous array of unsigned integers of one type. The `col_` functions take a typed pointer to that array, so the byte offset of a column must be a multiple of the size of its type.

Values must be sorted in ascending order in both layouts.


## What a search returns

Each search reports the item number when the value is found, or the initial `last` value when it is not, along with updated `first` and `last` positions. The Go methods and the Python functions also report an error, and refuse to search when the arguments do not describe a range of items inside the mapping: Go returns an error wrapping `ErrNotMapped`, `ErrOutOfRange`, `ErrMisaligned` or `ErrBitRange`, and Python raises a `ValueError`. The C functions trust their arguments, and `binsearch_check_row_range`, `binsearch_check_col_range`, `binsearch_check_col_offset` and `binsearch_check_bits` expose the same checks to a caller of the reference implementation.

Both updated positions describe a bound of the searched value inside the range that was given. `find_first` converges to the lower bound, the first item that is not less than the searched value, and `find_last` to the upper bound, the first item greater than it. On return `last` is that bound and `first` is the same position, or one below it when the value is absent, in which case it can fall one position below the range that was passed in. So `find_first` reports the item it found while `find_last` reports the position after it, and neither is a range to iterate over. The `has_next` and `has_prev` functions walk the remaining matches and take the bounds of the original range.

Those two bounds delimit the items equal to the searched value, so the `last` of `find_first` and the `last` of `find_last` are the ends of a half-open range. The `find_range` functions compute both in one descent and return the number of items between them, which costs about half of the two searches on data larger than the cache. Prefer them to walking the matches one at a time, a loop the wrappers pay a language boundary for on every item.


## Searching many values at once

The `find_many` functions search several values in one call and advance the searches in lockstep, so the cache misses of a whole batch are outstanding together instead of one at a time. That is about three to four times as fast per value in C on data larger than the CPU cache and still held in RAM, and more in the wrappers, where one call crosses the language boundary once for the batch instead of once per value. A count that is not a multiple of the batch leaves a remainder of two or more values, searched as one padded batch rather than one value at a time.

On a file too large for RAM the cache misses become page faults, which the kernel serves one at a time: the first lane to touch a missing page traps into the kernel and the others cannot issue their reads until it returns. The overlap the batch is built for never happens.

So the `find_many` functions ask for the pages of a batch before reading any of them. The whole batch then enters the device queue at once. On a 67 GiB file with a cold page cache that takes a search from about 1.6 ms to about 170 us, in C and through the Go wrapper alike. The request is a system call per value per step and buys nothing on resident data, where it costs far more than the search, so it is not made unconditionally.

That mode is the last argument of the C functions, the `Prefetch` field of the mapping in Go, and the optional `prefetch` argument in Python. `AUTO`, the default, times one batch out of every `BINSEARCH_PROBE_EVERY` with the request turned off and turns it on for the batches in between when that one took more than `BINSEARCH_SLOW_NS` per value. A resident batch costs three orders of magnitude less than that, so the threshold sits in a wide gap. Probing again rather than deciding once follows a mapping whose residency changes while it is being searched. `ALWAYS` and `NEVER` skip the probe.

Nothing similar can help a single search, whose descent learns each address only by reading the previous one. That is what makes `find_many` worth much more than `find_first` on a file that does not fit in RAM.

Page requests need `MADV_WILLNEED` or `POSIX_MADV_WILLNEED`. A strict ISO C build declares neither, so a caller compiling with `-std=c2x` and no feature test macro gets no page requests at all. The Go and Python builds define `_DEFAULT_SOURCE` for this reason.


## Naming

* the `_be_` or `BE` functions read big-endian values.
* the `_le_` or `LE` functions read little-endian values.
* the `col_` or `Col` functions read a contiguous array instead of adjacent blocks.
* the `_sub_` or `Sub` functions match only the bits from `bitstart` to `bitend`, counted from the most significant bit of the type.
* the `find_range` or `FindRange` functions report the half-open range of the items equal to the searched value.
* the `find_many` or `FindMany` functions search several values in one call.


## Mapping, unmapping and concurrency

`mmap_binfile` maps a file and reads the header of the Apache Arrow, Feather and custom BINSRC1 formats. For any other content the caller sets the number of columns and their type sizes before the call. Only a regular file is mapped: a path naming a directory, a FIFO or a device is rejected. The data block is reported as empty, zero rows, when the header is inconsistent or when the columns it describes do not fit in the file.

The `border` field reports the byte order of the values. No format recognised here declares one in a part of its header that is read, so the field is always little-endian and a caller whose content is big-endian overwrites it. An Apache Arrow file carries its byte order in the schema flatbuffer, which is skipped over rather than parsed.

`munmap_binfile` releases the mapping and resets the descriptor it is given, so a second call reports an error instead of unmapping an address that has been reused since. A mapped file may be searched from several threads at once, as the searches only read it, but unmapping must not overlap a search. Two threads unmapping the same file at the same time is safe: the address is taken with an atomic exchange, so only one of them releases the mapping and closes the descriptor.

An optional call to `binsearch_advise_random` tells the kernel that the mapping is read in a scattered order, so that it does not read ahead the pages surrounding the ones a search touches. It is a hint, and a caller that scans the data block as well as searching it is better off without it.


## BINSRC1 format

* 8 BYTE  : `BINSRC1\0` magic number
* 1 BYTE  : Number of columns.
* One byte for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).
* (PADDING TO ALIGN THE DATA TO 8 BYTE)
* 8 BYTE  : number of rows
* cols * 8 BYTE : offset to the start of each column
* (DATA BODY AS IN APACHE ARROW)

Here is `c/test/data/test_data_binsrc.bin`, the file the snippets above search:

```
42494e5352433100    : BINSRC1 magic number
02                  : 2 columns
04                  : first column is uint32_t (4 bytes)
08                  : second column is uint64_t (8 bytes)
0000000000          : padding to 8 byte
0b00000000000000    : 11 rows per columns
2800000000000000    : byte offset to the start of the first column
5800000000000000    : byte offset to the start of the second column
01000000            : first column - first row
07000000            : ...
0b000000            : 
61000000            : 
65000000            : 
e5030000            : 
f1030000            : 
f5260000            : 
a3860100            : 
19990100            : 
19990100            : 
00000000            : 
00803380257a0208    : second column - first row
18399e43fea10048    : ...
16eb5575fea10048    : 
00003a0074020180    : 
008013008d020180    : 
00007a0099020180    : 
00003a00622b01a0    : 
00807080622b01a0    : 
926625e3652b01a0    : 
039843d5672b01a0    : 
039843d5672b01a0    : 
```

Several fixtures under `c/test/data` are stored as a hexdump next to the binary. The xxd command-line application converts between the two:

```
xxd -p -c8 binaryfile.bin > hexfile.txt
xxd -r -p hexfile.txt > binaryfile.bin
```


## Getting started

A wrapper Makefile builds the project on a Linux-compatible system. All the artifacts and reports it produces are stored in the *target* folder of each language directory. To see all available options:

```
make help
```

`make all` builds and tests every implementation, `make test` runs the unit tests, `make linter` runs the linters and `make coverage` checks the unit test coverage. The same targets exist inside `c`, `go` and `python` for one language at a time.

The C library is the header `c/src/binsearch/binsearch.h`, which can be copied into a project as it is. `cd c && make install` installs it under `usr/include/tecnickcom/binsearch`, and `make rpm` or `make deb` builds a package of it.

Go code imports it as `github.com/tecnickcom/binsearch/go/src`, and cgo means a C compiler is required.

`cd python && make build` writes the Python extension as a wheel and a source distribution under `python/dist`.
