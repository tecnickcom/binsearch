// Python binsearch Module
//
// @category   Libraries
// @author     Nicola Asuni <info@tecnick.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/binsearch

// NOTE: PY_SSIZE_T_CLEAN must be defined before <Python.h> is included, which
// happens in pybinsearch.c: defining it here would be too late.
#ifndef BINSEARCH_PYBINSEARCH_H
#define BINSEARCH_PYBINSEARCH_H

#include <Python.h>

// This header holds the docstrings of the module. The functions themselves are
// defined in pybinsearch.c, where the method table is their only user.

// ----------

#define PYBINSEARCH_DOCSTRING "Binary search of unsigned integers in memory mapped binary files.\n"\
"\n"\
"Two data layouts are supported. In row mode the file is a sequence of adjacent\n"\
"constant-length blocks, each holding the searched value at a fixed offset. In\n"\
"column mode the data is a contiguous array of unsigned integers of one type,\n"\
"and the byte offset of the column must be a multiple of the size of its type.\n"\
"The values must be sorted in ascending order in both cases.\n"\
"\n"\
"The be and le functions read big-endian and little-endian values in both\n"\
"layouts, and the sub functions match only the bits from bitstart to bitend,\n"\
"counted from the most significant bit.\n"\
"\n"\
"The find functions narrow the first and last arguments to a bound of the\n"\
"searched value: find_first to the lower bound, the first item that is not less\n"\
"than it, and find_last to the upper bound, the first item that is greater than\n"\
"it. On return last is that bound and first is the same position, or one below\n"\
"it when the value is absent, in which case it can fall one position below the\n"\
"range that was passed in. So find_first reports the item it found while\n"\
"find_last reports the position after it, and neither is a range to iterate:\n"\
"the has_next and has_prev functions take the bounds of the original range.\n"\
"\n"\
"The two bounds delimit the items equal to the searched value, so the last of\n"\
"find_first and the last of find_last are the ends of a half-open range.\n"\
"\n"\
"The find_range functions report the half-open range of the items equal to the\n"\
"searched value in one descent, which costs about half of a find_first and a\n"\
"find_last. The find_many functions search a sequence of values in one call,\n"\
"advancing the searches in lockstep so that their cache misses overlap: that is\n"\
"several times faster per value than calling find_first in a loop. On a file\n"\
"too large for RAM the misses become page faults, which the kernel serves one\n"\
"at a time; the optional prefetch argument selects whether the pages of a batch\n"\
"are asked for before they are read, which puts those reads in the device queue\n"\
"together. It is one of PREFETCH_AUTO, PREFETCH_ALWAYS or PREFETCH_NEVER.\n"\
"\n"\
"Every search function takes the memory map returned by mmap_binfile and checks\n"\
"its arguments against it: a ValueError is raised when the object is not a\n"\
"memory mapped file, when it has already been unmapped, or when the search\n"\
"range, the column alignment or the bit range fall outside their bounds. An\n"\
"OverflowError is raised when a value is negative or does not fit its type.\n"\
"\n"\
"A mapped file may be searched from several threads at once, as the searches\n"\
"only read it. munmap_binfile must not run at the same time as a search. Two\n"\
"threads unmapping the same file at the same time is safe: the address is taken\n"\
"with an atomic exchange, so only one of them releases the mapping and closes\n"\
"the descriptor, and the other is told that there was nothing left to release."

#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file.\n"\
"\n"\
"The header of the BINSRC1, Apache Arrow and Feather formats is read from the\n"\
"file; for any other content the ctbytes argument describes the columns.\n"\
"On failure the file descriptor is -1 and the size is 0.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"ctbytes : iterable of int\n"\
"    Size in bytes of each column type (1 for uint8, 2 for uint16, 4 for uint32, 8 for uint64). At most 255 entries.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map.\n"\
"    - File descriptor.\n"\
"    - File size in bytes.\n"\
"    - Byte offset of the first item of the first column.\n"\
"    - Length in bytes of the data block.\n"\
"    - Number of rows.\n"\
"    - Number of columns.\n"\
"    - Byte order of the values: 0 for little-endian, 1 for big-endian.\n"\
"    - List of the byte offsets of the first item of each column.\n"\
"    - List of the size in bytes of each column type."

#define PYMUNMAPBINFILE_DOCSTRING "Unmap the file and close its descriptor.\n"\
"\n"\
"The descriptor and the size travel with the memory map: the ones passed here\n"\
"are only checked against them and a mismatch raises a ValueError, leaving the\n"\
"mapping usable. A mapping that is dropped without being unmapped is released\n"\
"when the memory mapped file object is garbage collected.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"fd : int\n"\
"    File descriptor, as returned by mmap_binfile.\n"\
"size : int\n"\
"    File size, as returned by mmap_binfile.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    0 on success, -1 on failure, 1 when another thread released the mapping first."

#define PYGETADDRESS_DOCSTRING "Return the byte offset of a value inside a memory mapped file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"item : int\n"\
"    Item number.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    Byte offset of the value of the given item."

// ----------

#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

// ----------

#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

// ----------

#define PYFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 8 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 16 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 32 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 64 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

// ----------

#define PYFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 8 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 16 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 32 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 64 bit unsigned integer in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

// ----------

#define PYHASNEXTUINT8_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTUINT16_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTUINT32_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTUINT64_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

// ----------

#define PYHASNEXTSUBUINT8_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTSUBUINT16_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTSUBUINT32_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTSUBUINT64_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

// ----------

#define PYHASPREVUINT8_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVUINT16_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVUINT32_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVUINT64_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

// ----------

#define PYHASPREVSUBUINT8_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVSUBUINT16_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVSUBUINT32_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVSUBUINT64_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

// ----------

// ----------

#define PYCOLFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYCOLFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYCOLFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYCOLFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

// ----------

#define PYCOLFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYCOLFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYCOLFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYCOLFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

// ----------

#define PYCOLFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYCOLFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYCOLFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

#define PYCOLFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Lower bound of the searched value, or the position below it when the value is absent.\n"\
"    - Lower bound of the searched value: the item that was found, or where it would be inserted."

// ----------

#define PYCOLFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYCOLFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYCOLFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

#define PYCOLFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number when found, or the initial value of last when not found.\n"\
"    - Upper bound of the searched value, or the position below it when the value is absent.\n"\
"    - Upper bound of the searched value: the position after the item that was found, or where it would be inserted."

// ----------

#define PYCOLHASNEXTUINT8_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTUINT16_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTUINT32_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTUINT64_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

// ----------

#define PYCOLHASNEXTSUBUINT8_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTSUBUINT16_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTSUBUINT32_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTSUBUINT64_DOCSTRING "Check whether the bits from bitstart to bitend of the item after pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"pos : int\n"\
"    Current item.\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

// ----------

#define PYCOLHASPREVUINT8_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVUINT16_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVUINT32_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVUINT64_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

// ----------

#define PYCOLHASPREVSUBUINT8_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 7).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVSUBUINT16_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 15).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVSUBUINT32_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 31).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVSUBUINT64_DOCSTRING "Check whether the bits from bitstart to bitend of the item before pos match the search value in a contiguous array of unsigned integers of the same type.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (min 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (max 63).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"pos : int\n"\
"    Current item.\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - True when the bits of the item match the search value, False otherwise.\n"\
"    - Updated position."

#define PYFINDRANGEUINT8_DOCSTRING "Narrow a range to the 8 bit unsigned integers equal to a value in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDRANGESUBUINT8_DOCSTRING "Narrow a range to the 8 bit unsigned integers whose bits from bitstart to bitend equal a value, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (7 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGEUINT8_DOCSTRING "Narrow a range to the 8 bit unsigned integers equal to a value in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGESUBUINT8_DOCSTRING "Narrow a range to the 8 bit unsigned integers whose bits from bitstart to bitend equal a value, in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (7 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDMANYUINT8_DOCSTRING "Search for the first occurrence of each of several 8 bit unsigned integers in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDMANYSUBUINT8_DOCSTRING "Search for the first occurrence of each of several 8 bit unsigned integers, matching only the bits from bitstart to bitend, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (7 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYUINT8_DOCSTRING "Search for the first occurrence of each of several 8 bit unsigned integers in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYSUBUINT8_DOCSTRING "Search for the first occurrence of each of several 8 bit unsigned integers, matching only the bits from bitstart to bitend, in a contiguous array of 8 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (7 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 8 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDRANGEUINT16_DOCSTRING "Narrow a range to the 16 bit unsigned integers equal to a value in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDRANGESUBUINT16_DOCSTRING "Narrow a range to the 16 bit unsigned integers whose bits from bitstart to bitend equal a value, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (15 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGEUINT16_DOCSTRING "Narrow a range to the 16 bit unsigned integers equal to a value in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGESUBUINT16_DOCSTRING "Narrow a range to the 16 bit unsigned integers whose bits from bitstart to bitend equal a value, in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (15 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDMANYUINT16_DOCSTRING "Search for the first occurrence of each of several 16 bit unsigned integers in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDMANYSUBUINT16_DOCSTRING "Search for the first occurrence of each of several 16 bit unsigned integers, matching only the bits from bitstart to bitend, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (15 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYUINT16_DOCSTRING "Search for the first occurrence of each of several 16 bit unsigned integers in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYSUBUINT16_DOCSTRING "Search for the first occurrence of each of several 16 bit unsigned integers, matching only the bits from bitstart to bitend, in a contiguous array of 16 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (15 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 16 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDRANGEUINT32_DOCSTRING "Narrow a range to the 32 bit unsigned integers equal to a value in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDRANGESUBUINT32_DOCSTRING "Narrow a range to the 32 bit unsigned integers whose bits from bitstart to bitend equal a value, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (31 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGEUINT32_DOCSTRING "Narrow a range to the 32 bit unsigned integers equal to a value in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGESUBUINT32_DOCSTRING "Narrow a range to the 32 bit unsigned integers whose bits from bitstart to bitend equal a value, in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (31 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDMANYUINT32_DOCSTRING "Search for the first occurrence of each of several 32 bit unsigned integers in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDMANYSUBUINT32_DOCSTRING "Search for the first occurrence of each of several 32 bit unsigned integers, matching only the bits from bitstart to bitend, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (31 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYUINT32_DOCSTRING "Search for the first occurrence of each of several 32 bit unsigned integers in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYSUBUINT32_DOCSTRING "Search for the first occurrence of each of several 32 bit unsigned integers, matching only the bits from bitstart to bitend, in a contiguous array of 32 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (31 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 32 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDRANGEUINT64_DOCSTRING "Narrow a range to the 64 bit unsigned integers equal to a value in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDRANGESUBUINT64_DOCSTRING "Narrow a range to the 64 bit unsigned integers whose bits from bitstart to bitend equal a value, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (63 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGEUINT64_DOCSTRING "Narrow a range to the 64 bit unsigned integers equal to a value in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. It computes in one descent what find_first and find_last compute in two.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYCOLFINDRANGESUBUINT64_DOCSTRING "Narrow a range to the 64 bit unsigned integers whose bits from bitstart to bitend equal a value, in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (63 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : int\n"\
"    Value to search, as an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Number of items equal to the searched value, which is 0 when it is absent.\n"\
"    - First item equal to the searched value, or where it would be inserted.\n"\
"    - Item past the last one equal to the searched value, or where it would be inserted."

#define PYFINDMANYUINT64_DOCSTRING "Search for the first occurrence of each of several 64 bit unsigned integers in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYFINDMANYSUBUINT64_DOCSTRING "Search for the first occurrence of each of several 64 bit unsigned integers, matching only the bits from bitstart to bitend, in a memory mapped file of adjacent blocks of sorted binary data. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first block.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Byte offset of the value inside a binary block.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (63 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYUINT64_DOCSTRING "Search for the first occurrence of each of several 64 bit unsigned integers in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order. The searches are advanced in lockstep so that their cache misses overlap, and one call replaces one per value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#define PYCOLFINDMANYSUBUINT64_DOCSTRING "Search for the first occurrence of each of several 64 bit unsigned integers, matching only the bits from bitstart to bitend, in a contiguous array of 64 bit unsigned integers. The values must be encoded in the byte order named by the function and sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset of the first item.\n"\
"bitstart : int\n"\
"    First bit to match, counted from the most significant bit (usually 0).\n"\
"bitend : int\n"\
"    Last bit to match, counted from the most significant bit (63 for this type).\n"\
"first : int\n"\
"    First item of the search range (min value = 0).\n"\
"last : int\n"\
"    Item past the last one of the search range (max value = nrows).\n"\
"search : iterable of int\n"\
"    Values to search, each an unsigned 64 bit integer.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"list of int :\n"\
"    For each searched value, the item number when it is found or last when it is not."

#endif  // BINSEARCH_PYBINSEARCH_H
