// Copyright (c) 2017 Nicola Asuni - Tecnick.com
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

/**
 * @file binsearch.h
 * @brief File containing the definition of public functions.
 *
 * The functions provided here allows to search big-endian
 * unsigned integers in a binary file made of adjacent
 * constant-length binary blocks sorted in ascending order.
 *
 * For example, the first 4 bytes of each 8-bytes blocks below
 * represent a uint32 in big-endian. The integers are sorted in
 * ascending order.
 *
 * 2f 81 f5 77 1a cc 7b 43
 * 2f 81 f5 78 76 5f 63 b8
 * 2f 81 f5 79 ca a9 a6 52
 *
 * This binary representation can be used to encode sortable key-value data,
 * even with nested keys.
 *
 * The xxd command-line application can be used to convert a binary file to
 * hexdump and reverse. For example:
 *
 * xxd -p binaryfile.bin > hexfile.txt
 * xxd -r -p hexfile.txt > binaryfile.bin
 *
 */

#ifndef BINSEARCH_H
#define BINSEARCH_H

#include <inttypes.h>

/**
 * Struct containing 128 bits as 2 64bit unsigned integer
 */
typedef struct uint128_t
{
    uint64_t hi;
    uint64_t lo;
} uint128_t;

/**
 * Struct containing the memory mapped file info.
 */
typedef struct mmfile_t
{
    /** Pointer to the memory map. */
    unsigned char *src;
    /** File descriptor. */
    int fd;
    /** File size in bytes. */
    uint64_t size;
} mmfile_t;

/**
 * Memory map the specified file.
 *
 * @param file  Path to the file to map.
 *
 * @return Returns the memory-mapped file descriptors.
 */
mmfile_t mmap_binfile(const char *file);

/**
 * Unmap and close the memory-mapped file.
 *
 * @param mf Descriptor of memory-mapped file.
 *
 * @return On success, munmap() returns 0,
 *         on failure -1, and errno is set (probably to EINVAL).
 */
int munmap_binfile(mmfile_t mf);

/**
 * Returns the absolute file start position of the specified item (binary block).
 *
 * @param blklen    Length of the binary block in bytes.
 * @param blkpos    Indicates the position of the number to search inside a binary block.
 * @param item      Item number to search.
 *
 * Return First byte position of the specified item number.
 */
uint64_t get_address(uint64_t blklen, uint64_t blkpos, uint64_t item);

/**
 * Convert 1 byte in big-endian format to uint8
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint8_t bytes_to_uint8_t(const unsigned char *src, uint64_t i);

/**
 * Convert 2 bytes in big-endian format to uint16
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint16_t bytes_to_uint16_t(const unsigned char *src, uint64_t i);

/**
 * Convert 4 bytes in big-endian format to uint32
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint32_t bytes_to_uint32_t(const unsigned char *src, uint64_t i);

/**
 * Convert 8 bytes in big-endian format to uint64
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint64_t bytes_to_uint64_t(const unsigned char *src, uint64_t i);

/**
 * Convert 16 bytes in big-endian format to uint128
 *
 * @param src Memory mapped file address.
 * @param i   Start position.
 *
 * @return Converted number
 */
uint128_t bytes_to_uint128_t(const unsigned char *src, uint64_t i);

#define define_compare(T) \
/** Compare two integers
@param a First integer to compare
@param b Second integer to compare
@return Negative value if a < b, positive value if a > b and zero if a = b.
*/ \
int compare_##T(T a, T b) {return (a < b) ? -1 : (a > b);}

define_compare(uint8_t)
define_compare(uint16_t)
define_compare(uint32_t)
define_compare(uint64_t)

/**
 * Compare two uint128_t integers
 *
 * @param a First integer to compare
 * @param b Second integer to compare
 *
 * @return Negative value if a < b, positive value if a > b and zero if a = b.
 * */
int compare_uint128_t(uint128_t a, uint128_t b)
{
    if (a.hi < b.hi) return -1;
    if (a.hi > b.hi) return 1;
    return compare_uint64_t(a.lo, b.lo);
}

/**
 * Generic function to search for the first occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_find_first(T) \
/** Search for the first occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must encoded in big-endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return item number if found or (last + 1) if not found.
 */ \
uint64_t find_first_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    T x; \
    int cmp; \
    while (*first <= *last) \
    { \
        middle = ((*first + *last) >> 1); \
        i = get_address(blklen, blkpos, middle); \
        x = bytes_to_##T(src, i); \
        cmp = compare_##T(x, search); \
        if (cmp == 0) \
        { \
            if (middle == 0) return middle; \
            found = middle; \
            *last = (middle - 1); \
        } \
        else if (cmp < 0) *first = (middle + 1); \
        else if (middle > 0) *last = (middle - 1); \
        else return found; \
    } \
    return found; \
}

/**
 * Generic function to search for the last occurrence of an unsigned integer
 * on a memory mapped binary file containing adjacent blocks of sorted binary data.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define define_find_last(T) \
/** Search for the last occurrence of an unsigned integer on a memory mapped
binary file containing adjacent blocks of sorted binary data.
The values in the file must encoded in big-endian format and sorted in ascending order.
@param src       Memory mapped file address.
@param blklen    Length of the binary block in bytes.
@param blkpos    Indicates the position of the number to search inside a binary block.
@param first     Pointer to the first element of the range to search (min value = 0).
@param last      Pointer to the last element of the range to search (max value = nitems - 1).
@param search    Unsigned number to search (type T).
@return Item number if found or (last + 1) if not found.
*/ \
uint64_t find_last_##T(const unsigned char *src, uint64_t blklen, uint64_t blkpos, uint64_t *first, uint64_t *last, T search) \
{ \
    uint64_t i, middle, found = (*last + 1); \
    T x; \
    int cmp; \
    while (*first <= *last) \
    { \
        middle = ((*first + *last) >> 1); \
        i = get_address(blklen, blkpos, middle); \
        x = bytes_to_##T(src, i); \
        cmp = compare_##T(x, search); \
        if (cmp == 0) \
        { \
            found = middle; \
            *first = (middle + 1); \
        } \
        else if (cmp < 0) *first = (middle + 1); \
        else if (middle > 0) *last = (middle - 1); \
        else return found; \
    } \
    return found; \
}

define_find_first(uint8_t)
define_find_last(uint8_t)
define_find_first(uint16_t)
define_find_last(uint16_t)
define_find_first(uint32_t)
define_find_last(uint32_t)
define_find_first(uint64_t)
define_find_last(uint64_t)
define_find_first(uint128_t)
define_find_last(uint128_t)

/**
 * Search for the first occurrence of an unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t
 */
#define find_first(T) find_first_##T

/**
 * Search for the last occurrence of an unsigned integer on a memory mapped
 * binary file containing adjacent blocks of sorted binary data.
 * The values in the file must encoded in big-endian format and sorted in ascending order.
 *
 * @param T Unsigned integer tupe, one of: uint8_t, uint16_t, uint32_t, uint64_t

 */
#define find_last(T) find_last_##T

#endif  // BINSEARCH_H
