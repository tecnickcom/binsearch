# Binsearch

*Fast binary search for columnar data formats.*

[![Sponsor on GitHub](https://img.shields.io/badge/sponsor-github-EA4AAA.svg?logo=githubsponsors&logoColor=white)](https://github.com/sponsors/tecnickcom)

If this project is useful to you, please consider [supporting development via GitHub Sponsors](https://github.com/sponsors/tecnickcom).

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **author**      Nicola Asuni
* **copyright**   2017-2026 Nicola Asuni - Tecnick.com
* **link**        https://github.com/tecnickcom/binsearch


## Description

This library searches unsigned integers in a memory mapped binary file. Two data layouts are supported.

In row mode the file is a sequence of adjacent constant-length blocks, each holding the searched value at a fixed offset. In the 8-byte blocks below the first 4 bytes are a `uint32` in big-endian and the blocks are sorted by that value.

```
2f 81 f5 77 1a cc 7b 43
2f 81 f5 78 76 5f 63 b8
2f 81 f5 79 ca a9 a6 52
```

This representation can be used to encode sortable key-value data, even with nested keys.

In column mode the data is a contiguous array of unsigned integers of one type, in host byte order. The `col_` functions take a typed pointer to that array, so the byte offset of a column must be a multiple of the size of its type.

The values must be sorted in ascending order in both layouts.

Each search returns the item number when the value is found, or the initial `last` value when it is not, along with the updated `first` and `last` positions. The `has_next` and `has_prev` functions walk over the remaining matches.

The `mmap_binfile` function maps a file and reads the header of the Apache Arrow, Feather and custom BINSRC1 formats. For any other content the caller sets the number of columns and their type sizes before the call. The data block is reported as empty (zero rows) when the header is inconsistent or when the columns it describes do not fit in the file.

The xxd command-line application converts a binary file to a hexdump and back:

```
xxd -p -c8 binaryfile.bin > hexfile.txt
xxd -r -p hexfile.txt > binaryfile.bin
```

## Naming

* the `_be_` or `BE` functions read big-endian values.
* the `_le_` or `LE` functions read little-endian values.
* the `col_` or `Col` functions read a contiguous array in host byte order.
* the `_sub_` or `Sub` functions match only the bits from `bitstart` to `bitend`, counted from the most significant bit of the type.

## Getting Started

The reference implementation is a header-only C library, with wrappers for Go and Python.

A wrapper Makefile builds the project on a Linux-compatible system.
All the artifacts and reports it produces are stored in the *target* folder.

To see all available options:
```
make help
```

Use `make all` to build and test every implementation, `make test` to run the unit tests, `make linter` to run the linters and `make coverage` to check that the unit test coverage is 100%.


## BINSRC Format:

* 8 BYTE  : `BINSRC1\0` magic number
* 1 BYTE  : Number of columns.
* One byte for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).
* (PADDING TO ALIGN THE DATA TO 8 BYTE)
* 8 BYTE  : number of rows
* cols * 8 BYTE : offset to the start of each column
* (DATA BODY AS IN APACHE ARROW)

### Example

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
