# Binsearch

*Provides binsearch*

[![Master Build Status](https://secure.travis-ci.org/tecnickcom/binsearch.png?branch=master)](https://travis-ci.org/tecnickcom/binsearch?branch=master)
[![Master Coverage Status](https://coveralls.io/repos/tecnickcom/binsearch/badge.svg?branch=master&service=github)](https://coveralls.io/github/tecnickcom/binsearch?branch=master)

* **category**    Libraries
* **license**     MIT (see LICENSE)
* **link**        https://github.com/tecnickcom/binsearch


## Description

The functions provided here allows to search big-endian
unsigned integers in a binary file made of adjacent
constant-lenght binary blocks sorted in ascending order.

For example, the first 4 bytes of each 8-bytes blocks below represent a uint32 in big-endian.
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
xxd -p binaryfile.bin > hexfile.txt
xxd -r -p hexfile.txt > binaryfile.bin
```

## Getting Started

This application is written in C language and it is C99 compatible.

This project can be built using CMake.
A wrapper Makefile is available to allows building the project in a Linux-compatible system with simple commands.  
All the artifacts and reports produced using this Makefile are stored in the *target* folder.  

To see all available options:
```
make help
```

To execute all the default test builds and generate reports in the current environment:
```
make test
```

To format the code (please use this command before submitting any pull request):
```
make format
```

To build a library:
```
make build
```
Set *BS_BUILD_SHARED_LIB* in config.mk to build a static or shared library.


## Python Module

A shared library can be built using the command:
```
make python
```


### Python Usage Example

```
# copy this code in the same directory of libpybinsearch library

import libpybinsearch as bs

print('\nUSAGE EXAMPLE:\n')
```
