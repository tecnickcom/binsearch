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
"column mode the data is a contiguous array of unsigned integers of one type, in\n"\
"host byte order, and the byte offset of the column must be a multiple of the\n"\
"size of its type. The values must be sorted in ascending order in both cases.\n"\
"\n"\
"The be and le functions read big-endian and little-endian values, the col\n"\
"functions read values in host byte order, and the sub functions match only the\n"\
"bits from bitstart to bitend, counted from the most significant bit.\n"\
"\n"\
"Every search function takes the memory map returned by mmap_binfile and checks\n"\
"its arguments against it: a ValueError is raised when the object is not a\n"\
"memory mapped file, when it has already been unmapped, or when the search\n"\
"range, the column alignment or the bit range fall outside their bounds. An\n"\
"OverflowError is raised when a value is negative or does not fit its type."

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
"    - List of the byte offsets of the first item of each column."

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
"    0 on success, -1 on failure."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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
"    - Updated first position.\n"\
"    - Updated last position."

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

#define PYHASNEXTSUBUINT8_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTSUBUINT16_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTSUBUINT32_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASNEXTSUBUINT64_DOCSTRING "Check whether the item after pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
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

#define PYHASPREVSUBUINT8_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVSUBUINT16_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVSUBUINT32_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYHASPREVSUBUINT64_DOCSTRING "Check whether the item before pos matches the search value in a memory mapped file of adjacent blocks of sorted binary data.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

// ----------

// ----------

#define PYCOLFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

// ----------

#define PYCOLFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

// ----------

#define PYCOLFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of the bits from bitstart to bitend of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

// ----------

#define PYCOLFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 8 bit unsigned integer in a contiguous array of 8 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 16 bit unsigned integer in a contiguous array of 16 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 32 bit unsigned integer in a contiguous array of 32 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

#define PYCOLFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of the bits from bitstart to bitend of a 64 bit unsigned integer in a contiguous array of 64 bit unsigned integers. The values must be in host byte order and sorted in ascending order.\n"\
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
"    - Updated first position.\n"\
"    - Updated last position."

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

#define PYCOLHASNEXTSUBUINT8_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTSUBUINT16_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTSUBUINT32_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASNEXTSUBUINT64_DOCSTRING "Check whether the item after pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
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

#define PYCOLHASPREVSUBUINT8_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVSUBUINT16_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVSUBUINT32_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#define PYCOLHASPREVSUBUINT64_DOCSTRING "Check whether the item before pos matches the search value in a contiguous array of unsigned integers of the same type.\n"\
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
"    - True when the item matches the search value, False otherwise.\n"\
"    - Updated position."

#endif  // BINSEARCH_PYBINSEARCH_H
