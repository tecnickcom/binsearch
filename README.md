# Binsearch

*Provides binsearch*

[![Master Build Status](https://secure.travis-ci.org/tecnickcom/binsearch.png?branch=master)](https://travis-ci.org/tecnickcom/binsearch?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/tecnickcom/binsearch/badge.svg?branch=master&service=github)](https://coveralls.io/github/tecnickcom/binsearch?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **author**      Nicola Asuni
* **copyright**   2017-2018 Nicola Asuni - Tecnick.com
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

The `mmap_binfile` function is able to extract some basic data from files in Apache Arrow, Feather or custom Binsrc format.

The Binsrc format is as follows:

* 8 BYTE  : `BINSRC1\0` magic number
* 1 BYTE  : Number of columns.
* One byte for each column type (i.e. 1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64).
* (PADDING TO ALIGN THE DATA TO 8 BYTE)
* (DATA BODY AS IN APACHE ARROW)


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
