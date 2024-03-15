# Binsearch

*Fast binary search for columnar data formats.*

[![Master Build Status](https://secure.travis-ci.org/tecnickcom/binsearch.png?branch=master)](https://travis-ci.org/tecnickcom/binsearch?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/tecnickcom/binsearch/badge.svg?branch=master&service=github)](https://coveralls.io/github/tecnickcom/binsearch?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **author**      Nicola Asuni
* **copyright**   2017-2024 Nicola Asuni - Tecnick.com
* **link**        https://github.com/tecnickcom/binsearch


## Description

The functions provided here allows to search unsigned integers in a binary file made of adjacent constant-length binary blocks sorted in ascending order.

For example, the first 4 bytes of each 8-bytes blocks below represent a `uint32` in big-endian.
The integers are sorted in ascending order.

```
2f 81 f5 77 1a cc 7b 43
2f 81 f5 78 76 5f 63 b8
2f 81 f5 79 ca a9 a6 52
```

This binary representation can be used to encode sortable key-value data, even with nested keys.

The xxd command-line application can be used to convert a binary file to hexdump and reverse.
For example:

```
xxd -p -c8 binaryfile.bin > hexfile.txt
xxd -r -p hexfile.txt > binaryfile.bin
```

This library also provide functions to read columnar data in Little-Endian format.

The `mmap_binfile` function is able to extract some basic data from files in Apache Arrow, Feather or custom BINSRC format.




## Getting Started

The reference code of this application is written in C language and includes wrappers for GO and Python.

A wrapper Makefile is available to allows building the project in a Linux-compatible system with simple commands.  
All the artifacts and reports produced using this Makefile are stored in the *target* folder.  

To see all available options:
```
make help
```

use the command ```make all``` to build and test all the implementations.

## NOTE

* the "_be_" or "BE" functions refer to source files sorted in Big-Endian.
* the "_le_" or "LE" functions refer to source files sorted in Little-Endian.



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
