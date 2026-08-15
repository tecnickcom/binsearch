// Package binsearch wraps the binsearch C library.
//
// It searches unsigned integers in a memory mapped file, in two layouts:
// row mode, where the values sit at a fixed offset inside adjacent
// constant-length blocks, and column mode, where they form a contiguous array
// of one type. The values must be sorted in ascending order.
//
// The BE and LE methods read big-endian and little-endian values, the Col
// methods read values in host byte order, and the Sub methods match only the
// bits from bitstart to bitend, counted from the most significant bit.
package binsearch

/*
#cgo CFLAGS: -O3 -pedantic -std=c2x -Wextra -Wno-strict-prototypes -Wcast-align -Wundef -Wformat -Wformat-security -Wshadow

// None of the C functions retains a pointer it is given and none of them
// calls back into Go, so the arguments of the wrappers can stay on the
// goroutine stack instead of being moved to the heap: without these
// directives cgo has to assume the opposite and every search call costs one
// allocation for the first, last or pos argument.
#cgo noescape col_find_first_sub_uint16_t
#cgo nocallback col_find_first_sub_uint16_t
#cgo noescape col_find_first_sub_uint32_t
#cgo nocallback col_find_first_sub_uint32_t
#cgo noescape col_find_first_sub_uint64_t
#cgo nocallback col_find_first_sub_uint64_t
#cgo noescape col_find_first_sub_uint8_t
#cgo nocallback col_find_first_sub_uint8_t
#cgo noescape col_find_first_uint16_t
#cgo nocallback col_find_first_uint16_t
#cgo noescape col_find_first_uint32_t
#cgo nocallback col_find_first_uint32_t
#cgo noescape col_find_first_uint64_t
#cgo nocallback col_find_first_uint64_t
#cgo noescape col_find_first_uint8_t
#cgo nocallback col_find_first_uint8_t
#cgo noescape col_find_last_sub_uint16_t
#cgo nocallback col_find_last_sub_uint16_t
#cgo noescape col_find_last_sub_uint32_t
#cgo nocallback col_find_last_sub_uint32_t
#cgo noescape col_find_last_sub_uint64_t
#cgo nocallback col_find_last_sub_uint64_t
#cgo noescape col_find_last_sub_uint8_t
#cgo nocallback col_find_last_sub_uint8_t
#cgo noescape col_find_last_uint16_t
#cgo nocallback col_find_last_uint16_t
#cgo noescape col_find_last_uint32_t
#cgo nocallback col_find_last_uint32_t
#cgo noescape col_find_last_uint64_t
#cgo nocallback col_find_last_uint64_t
#cgo noescape col_find_last_uint8_t
#cgo nocallback col_find_last_uint8_t
#cgo noescape col_has_next_sub_uint16_t
#cgo nocallback col_has_next_sub_uint16_t
#cgo noescape col_has_next_sub_uint32_t
#cgo nocallback col_has_next_sub_uint32_t
#cgo noescape col_has_next_sub_uint64_t
#cgo nocallback col_has_next_sub_uint64_t
#cgo noescape col_has_next_sub_uint8_t
#cgo nocallback col_has_next_sub_uint8_t
#cgo noescape col_has_next_uint16_t
#cgo nocallback col_has_next_uint16_t
#cgo noescape col_has_next_uint32_t
#cgo nocallback col_has_next_uint32_t
#cgo noescape col_has_next_uint64_t
#cgo nocallback col_has_next_uint64_t
#cgo noescape col_has_next_uint8_t
#cgo nocallback col_has_next_uint8_t
#cgo noescape col_has_prev_sub_uint16_t
#cgo nocallback col_has_prev_sub_uint16_t
#cgo noescape col_has_prev_sub_uint32_t
#cgo nocallback col_has_prev_sub_uint32_t
#cgo noescape col_has_prev_sub_uint64_t
#cgo nocallback col_has_prev_sub_uint64_t
#cgo noescape col_has_prev_sub_uint8_t
#cgo nocallback col_has_prev_sub_uint8_t
#cgo noescape col_has_prev_uint16_t
#cgo nocallback col_has_prev_uint16_t
#cgo noescape col_has_prev_uint32_t
#cgo nocallback col_has_prev_uint32_t
#cgo noescape col_has_prev_uint64_t
#cgo nocallback col_has_prev_uint64_t
#cgo noescape col_has_prev_uint8_t
#cgo nocallback col_has_prev_uint8_t
#cgo noescape find_first_be_uint16_t
#cgo nocallback find_first_be_uint16_t
#cgo noescape find_first_be_uint32_t
#cgo nocallback find_first_be_uint32_t
#cgo noescape find_first_be_uint64_t
#cgo nocallback find_first_be_uint64_t
#cgo noescape find_first_be_uint8_t
#cgo nocallback find_first_be_uint8_t
#cgo noescape find_first_le_uint16_t
#cgo nocallback find_first_le_uint16_t
#cgo noescape find_first_le_uint32_t
#cgo nocallback find_first_le_uint32_t
#cgo noescape find_first_le_uint64_t
#cgo nocallback find_first_le_uint64_t
#cgo noescape find_first_le_uint8_t
#cgo nocallback find_first_le_uint8_t
#cgo noescape find_first_sub_be_uint16_t
#cgo nocallback find_first_sub_be_uint16_t
#cgo noescape find_first_sub_be_uint32_t
#cgo nocallback find_first_sub_be_uint32_t
#cgo noescape find_first_sub_be_uint64_t
#cgo nocallback find_first_sub_be_uint64_t
#cgo noescape find_first_sub_be_uint8_t
#cgo nocallback find_first_sub_be_uint8_t
#cgo noescape find_first_sub_le_uint16_t
#cgo nocallback find_first_sub_le_uint16_t
#cgo noescape find_first_sub_le_uint32_t
#cgo nocallback find_first_sub_le_uint32_t
#cgo noescape find_first_sub_le_uint64_t
#cgo nocallback find_first_sub_le_uint64_t
#cgo noescape find_first_sub_le_uint8_t
#cgo nocallback find_first_sub_le_uint8_t
#cgo noescape find_last_be_uint16_t
#cgo nocallback find_last_be_uint16_t
#cgo noescape find_last_be_uint32_t
#cgo nocallback find_last_be_uint32_t
#cgo noescape find_last_be_uint64_t
#cgo nocallback find_last_be_uint64_t
#cgo noescape find_last_be_uint8_t
#cgo nocallback find_last_be_uint8_t
#cgo noescape find_last_le_uint16_t
#cgo nocallback find_last_le_uint16_t
#cgo noescape find_last_le_uint32_t
#cgo nocallback find_last_le_uint32_t
#cgo noescape find_last_le_uint64_t
#cgo nocallback find_last_le_uint64_t
#cgo noescape find_last_le_uint8_t
#cgo nocallback find_last_le_uint8_t
#cgo noescape find_last_sub_be_uint16_t
#cgo nocallback find_last_sub_be_uint16_t
#cgo noescape find_last_sub_be_uint32_t
#cgo nocallback find_last_sub_be_uint32_t
#cgo noescape find_last_sub_be_uint64_t
#cgo nocallback find_last_sub_be_uint64_t
#cgo noescape find_last_sub_be_uint8_t
#cgo nocallback find_last_sub_be_uint8_t
#cgo noescape find_last_sub_le_uint16_t
#cgo nocallback find_last_sub_le_uint16_t
#cgo noescape find_last_sub_le_uint32_t
#cgo nocallback find_last_sub_le_uint32_t
#cgo noescape find_last_sub_le_uint64_t
#cgo nocallback find_last_sub_le_uint64_t
#cgo noescape find_last_sub_le_uint8_t
#cgo nocallback find_last_sub_le_uint8_t
#cgo noescape has_next_be_uint16_t
#cgo nocallback has_next_be_uint16_t
#cgo noescape has_next_be_uint32_t
#cgo nocallback has_next_be_uint32_t
#cgo noescape has_next_be_uint64_t
#cgo nocallback has_next_be_uint64_t
#cgo noescape has_next_be_uint8_t
#cgo nocallback has_next_be_uint8_t
#cgo noescape has_next_le_uint16_t
#cgo nocallback has_next_le_uint16_t
#cgo noescape has_next_le_uint32_t
#cgo nocallback has_next_le_uint32_t
#cgo noescape has_next_le_uint64_t
#cgo nocallback has_next_le_uint64_t
#cgo noescape has_next_le_uint8_t
#cgo nocallback has_next_le_uint8_t
#cgo noescape has_next_sub_be_uint16_t
#cgo nocallback has_next_sub_be_uint16_t
#cgo noescape has_next_sub_be_uint32_t
#cgo nocallback has_next_sub_be_uint32_t
#cgo noescape has_next_sub_be_uint64_t
#cgo nocallback has_next_sub_be_uint64_t
#cgo noescape has_next_sub_be_uint8_t
#cgo nocallback has_next_sub_be_uint8_t
#cgo noescape has_next_sub_le_uint16_t
#cgo nocallback has_next_sub_le_uint16_t
#cgo noescape has_next_sub_le_uint32_t
#cgo nocallback has_next_sub_le_uint32_t
#cgo noescape has_next_sub_le_uint64_t
#cgo nocallback has_next_sub_le_uint64_t
#cgo noescape has_next_sub_le_uint8_t
#cgo nocallback has_next_sub_le_uint8_t
#cgo noescape has_prev_be_uint16_t
#cgo nocallback has_prev_be_uint16_t
#cgo noescape has_prev_be_uint32_t
#cgo nocallback has_prev_be_uint32_t
#cgo noescape has_prev_be_uint64_t
#cgo nocallback has_prev_be_uint64_t
#cgo noescape has_prev_be_uint8_t
#cgo nocallback has_prev_be_uint8_t
#cgo noescape has_prev_le_uint16_t
#cgo nocallback has_prev_le_uint16_t
#cgo noescape has_prev_le_uint32_t
#cgo nocallback has_prev_le_uint32_t
#cgo noescape has_prev_le_uint64_t
#cgo nocallback has_prev_le_uint64_t
#cgo noescape has_prev_le_uint8_t
#cgo nocallback has_prev_le_uint8_t
#cgo noescape has_prev_sub_be_uint16_t
#cgo nocallback has_prev_sub_be_uint16_t
#cgo noescape has_prev_sub_be_uint32_t
#cgo nocallback has_prev_sub_be_uint32_t
#cgo noescape has_prev_sub_be_uint64_t
#cgo nocallback has_prev_sub_be_uint64_t
#cgo noescape has_prev_sub_be_uint8_t
#cgo nocallback has_prev_sub_be_uint8_t
#cgo noescape has_prev_sub_le_uint16_t
#cgo nocallback has_prev_sub_le_uint16_t
#cgo noescape has_prev_sub_le_uint32_t
#cgo nocallback has_prev_sub_le_uint32_t
#cgo noescape has_prev_sub_le_uint64_t
#cgo nocallback has_prev_sub_le_uint64_t
#cgo noescape has_prev_sub_le_uint8_t
#cgo nocallback has_prev_sub_le_uint8_t
#cgo noescape mmap_binfile
#cgo nocallback mmap_binfile
#cgo noescape munmap_binfile
#cgo nocallback munmap_binfile

#include "../../c/src/binsearch/binsearch.h"
*/
import "C"

import (
	"errors"
	"fmt"
	"unsafe"
)

// maxncols is the largest number of columns that fits the ncols field (uint8_t).
const maxncols = 255

// TMMFile describes a memory mapped file.
type TMMFile struct {
	Src     unsafe.Pointer // Pointer to the memory map.
	Fd      int            // File descriptor.
	Size    uint64         // File size in bytes.
	DOffset uint64         // Byte offset of the first item of the first column.
	DLength uint64         // Length in bytes of the data block.
	NRows   uint64         // Number of rows.
	NCols   uint8          // Number of columns.
	CTBytes []uint8        // Size in bytes of each column type (1, 2, 4 or 8).
	Index   []uint64       // Byte offset of the first item of each column.
}

// castCTMMFileToGo converts a C.mmfile_t to a TMMFile.
func castCTMMFileToGo(mf C.mmfile_t) TMMFile {
	ncols := uint8(mf.ncols)
	ctbytes := make([]uint8, ncols)
	index := make([]uint64, ncols)

	var i uint8

	for i = range ncols {
		ctbytes[i] = uint8(mf.ctbytes[i])
		index[i] = uint64(mf.index[i])
	}

	return TMMFile{
		Src:     unsafe.Pointer(mf.src),
		Fd:      int(mf.fd),
		Size:    uint64(mf.size),
		DOffset: uint64(mf.doffset),
		DLength: uint64(mf.dlength),
		NRows:   uint64(mf.nrows),
		NCols:   ncols,
		CTBytes: ctbytes,
		Index:   index,
	}
}

// StringToNTBytes converts a string to a byte slice with an extra null terminator.
// This is to ensure a correct CGO conversion to char*.
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b, s)

	return b
}

// validCTBytes reports whether v is a column type size the search functions can read.
func validCTBytes(v uint8) bool {
	return v == 1 || v == 2 || v == 4 || v == 8
}

// MmapBinFile maps the specified file in memory.
// The ctbytes slice lists the number of bytes of each column type: it must have
// between one and maxncols entries, each of them 1, 2, 4 or 8.
func MmapBinFile(file string, ctbytes []uint8) (TMMFile, error) {
	if len(ctbytes) == 0 || len(ctbytes) > maxncols {
		return TMMFile{}, fmt.Errorf("the number of columns must be between 1 and %d: got %d", maxncols, len(ctbytes))
	}

	for k, v := range ctbytes {
		if !validCTBytes(v) {
			return TMMFile{}, fmt.Errorf("the column type size must be 1, 2, 4 or 8: got %d for column %d", v, k)
		}
	}

	bfile := StringToNTBytes(file)
	p := unsafe.Pointer(&bfile[0])

	var mf C.mmfile_t

	mf.ncols = C.uint8_t(len(ctbytes))

	for k, v := range ctbytes {
		mf.ctbytes[k] = C.uint8_t(v)
	}

	C.mmap_binfile((*C.char)(p), &mf)

	if mf.fd < 0 || mf.size == 0 {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}

	return castCTMMFileToGo(mf), nil
}

// Close unmaps the file and closes its descriptor.
// The descriptor is cleared, so that a second call reports an error instead of
// unmapping an address and closing a descriptor that have been reused since.
func (mf *TMMFile) Close() error {
	if mf.Src == nil {
		return errors.New("the file is not memory mapped")
	}

	// Only the address, the size and the file descriptor are used to unmap the
	// file, so the column arrays of the C descriptor are left empty.
	var cmf C.mmfile_t

	cmf.src = (*C.uint8_t)(mf.Src)
	cmf.fd = C.int(mf.Fd)
	cmf.size = C.uint64_t(mf.Size)

	// The mapping is released before the descriptor is closed, so the file is
	// marked as unmapped whatever the result reports.
	mf.Src = nil
	mf.Fd = -1
	mf.Size = 0

	e := int(C.munmap_binfile(cmf))
	if e != 0 {
		return fmt.Errorf("got %d error while unmapping the file", e)
	}

	return nil
}

// GetAddress returns the byte offset of a value inside a memory mapped file.
func GetAddress(blklen, blkpos, item uint64) uint64 {
	return ((blklen * item) + blkpos)
}

// FindFirstBEUint8 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding big-endian 8 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstBEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint8 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding big-endian 8 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastBEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint16 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding big-endian 16 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstBEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint16 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding big-endian 16 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastBEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint32 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding big-endian 32 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstBEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint32 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding big-endian 32 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastBEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint64 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding big-endian 64 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstBEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint64 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding big-endian 64 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastBEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint8 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 8 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint8 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 8 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint16 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 16 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint16 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 16 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint32 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 32 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint32 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 32 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint64 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 64 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint64 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding big-endian 64 bit
// unsigned integers. The values must be sorted in ascending order. It returns the item number when
// found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// HasNextBEUint8 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 8 bit unsigned integers. Set
// pos to the item returned by FindFirstBEUint8 to walk forward over the matching items. It returns
// the result and the updated position.
func (mf *TMMFile) HasNextBEUint8(offset, blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextBEUint16 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 16 bit unsigned integers. Set
// pos to the item returned by FindFirstBEUint16 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasNextBEUint16(offset, blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextBEUint32 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 32 bit unsigned integers. Set
// pos to the item returned by FindFirstBEUint32 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasNextBEUint32(offset, blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextBEUint64 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 64 bit unsigned integers. Set
// pos to the item returned by FindFirstBEUint64 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasNextBEUint64(offset, blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint8 reports whether the bits from bitstart to bitend of the item after pos matches
// the search value in a memory mapped file of adjacent blocks of sorted binary data holding big-
// endian 8 bit unsigned integers. Set pos to the item returned by FindFirstSubBEUint8 to walk
// forward over the matching items. It returns the result and the updated position.
func (mf *TMMFile) HasNextSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint16 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 16 bit unsigned integers. Set pos to the item returned by
// FindFirstSubBEUint16 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasNextSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint32 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 32 bit unsigned integers. Set pos to the item returned by
// FindFirstSubBEUint32 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasNextSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint64 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 64 bit unsigned integers. Set pos to the item returned by
// FindFirstSubBEUint64 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasNextSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint8 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 8 bit unsigned integers. Set
// pos to the item returned by FindLastBEUint8 to walk backward over the matching items. It returns
// the result and the updated position.
func (mf *TMMFile) HasPrevBEUint8(offset, blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint16 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 16 bit unsigned integers. Set
// pos to the item returned by FindLastBEUint16 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasPrevBEUint16(offset, blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint32 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 32 bit unsigned integers. Set
// pos to the item returned by FindLastBEUint32 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasPrevBEUint32(offset, blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint64 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding big-endian 64 bit unsigned integers. Set
// pos to the item returned by FindLastBEUint64 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasPrevBEUint64(offset, blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint8 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 8 bit unsigned integers. Set pos to the item returned by FindLastSubBEUint8
// to walk backward over the matching items. It returns the result and the updated position.
func (mf *TMMFile) HasPrevSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint16 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 16 bit unsigned integers. Set pos to the item returned by FindLastSubBEUint16
// to walk backward over the matching items. It returns the result and the updated position.
func (mf *TMMFile) HasPrevSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint32 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 32 bit unsigned integers. Set pos to the item returned by FindLastSubBEUint32
// to walk backward over the matching items. It returns the result and the updated position.
func (mf *TMMFile) HasPrevSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint64 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding big-endian 64 bit unsigned integers. Set pos to the item returned by FindLastSubBEUint64
// to walk backward over the matching items. It returns the result and the updated position.
func (mf *TMMFile) HasPrevSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// FindFirstLEUint8 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding little-endian 8 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstLEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint8 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding little-endian 8 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastLEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint16 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding little-endian 16 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstLEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint16 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding little-endian 16 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastLEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint32 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding little-endian 32 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstLEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint32 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding little-endian 32 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastLEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint64 searches for the first occurrence of a value in a memory mapped file of
// adjacent blocks of sorted binary data holding little-endian 64 bit unsigned integers. The values
// must be sorted in ascending order. It returns the item number when found or the initial last
// value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstLEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint64 searches for the last occurrence of a value in a memory mapped file of adjacent
// blocks of sorted binary data holding little-endian 64 bit unsigned integers. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastLEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint8 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 8
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint8 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 8
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint16 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 16
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint16 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 16
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint32 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 32
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint32 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 32
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint64 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 64
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindFirstSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint64 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a memory mapped file of adjacent blocks of sorted binary data holding little-endian 64
// bit unsigned integers. The values must be sorted in ascending order. It returns the item number
// when found or the initial last value when not found, plus the updated first and last positions.
func (mf *TMMFile) FindLastSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// HasNextLEUint8 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 8 bit unsigned integers. Set
// pos to the item returned by FindFirstLEUint8 to walk forward over the matching items. It returns
// the result and the updated position.
func (mf *TMMFile) HasNextLEUint8(offset, blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextLEUint16 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 16 bit unsigned integers.
// Set pos to the item returned by FindFirstLEUint16 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasNextLEUint16(offset, blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextLEUint32 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 32 bit unsigned integers.
// Set pos to the item returned by FindFirstLEUint32 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasNextLEUint32(offset, blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextLEUint64 reports whether the item after pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 64 bit unsigned integers.
// Set pos to the item returned by FindFirstLEUint64 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasNextLEUint64(offset, blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint8 reports whether the bits from bitstart to bitend of the item after pos matches
// the search value in a memory mapped file of adjacent blocks of sorted binary data holding
// little-endian 8 bit unsigned integers. Set pos to the item returned by FindFirstSubLEUint8 to
// walk forward over the matching items. It returns the result and the updated position.
func (mf *TMMFile) HasNextSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint16 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 16 bit unsigned integers. Set pos to the item returned by
// FindFirstSubLEUint16 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasNextSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint32 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 32 bit unsigned integers. Set pos to the item returned by
// FindFirstSubLEUint32 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasNextSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint64 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 64 bit unsigned integers. Set pos to the item returned by
// FindFirstSubLEUint64 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasNextSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint8 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 8 bit unsigned integers. Set
// pos to the item returned by FindLastLEUint8 to walk backward over the matching items. It returns
// the result and the updated position.
func (mf *TMMFile) HasPrevLEUint8(offset, blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint16 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 16 bit unsigned integers.
// Set pos to the item returned by FindLastLEUint16 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasPrevLEUint16(offset, blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint32 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 32 bit unsigned integers.
// Set pos to the item returned by FindLastLEUint32 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasPrevLEUint32(offset, blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint64 reports whether the item before pos matches the search value in a memory mapped
// file of adjacent blocks of sorted binary data holding little-endian 64 bit unsigned integers.
// Set pos to the item returned by FindLastLEUint64 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) HasPrevLEUint64(offset, blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint8 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 8 bit unsigned integers. Set pos to the item returned by
// FindLastSubLEUint8 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasPrevSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint16 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 16 bit unsigned integers. Set pos to the item returned by
// FindLastSubLEUint16 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasPrevSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint32 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 32 bit unsigned integers. Set pos to the item returned by
// FindLastSubLEUint32 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasPrevSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint64 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a memory mapped file of adjacent blocks of sorted binary data
// holding little-endian 64 bit unsigned integers. Set pos to the item returned by
// FindLastSubLEUint64 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) HasPrevSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// --- COLUMN ---

// ColFindFirstUint8 searches for the first occurrence of a value in a contiguous array of 8 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindFirstUint8(offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint8 searches for the last occurrence of a value in a contiguous array of 8 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindLastUint8(offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint16 searches for the first occurrence of a value in a contiguous array of 16 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindFirstUint16(offset, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint16 searches for the last occurrence of a value in a contiguous array of 16 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindLastUint16(offset, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint32 searches for the first occurrence of a value in a contiguous array of 32 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindFirstUint32(offset, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint32 searches for the last occurrence of a value in a contiguous array of 32 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindLastUint32(offset, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint64 searches for the first occurrence of a value in a contiguous array of 64 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindFirstUint64(offset, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint64 searches for the last occurrence of a value in a contiguous array of 64 bit
// unsigned integers in host byte order. The values must be sorted in ascending order. It returns
// the item number when found or the initial last value when not found, plus the updated first and
// last positions.
func (mf *TMMFile) ColFindLastUint64(offset, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint8 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 8 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindFirstSubUint8(offset uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint8 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 8 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindLastSubUint8(offset uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint16 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 16 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindFirstSubUint16(offset uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint16 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 16 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindLastSubUint16(offset uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint32 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 32 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindFirstSubUint32(offset uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint32 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 32 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindLastSubUint32(offset uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint64 searches for the first occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 64 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindFirstSubUint64(offset uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint64 searches for the last occurrence of the bits from bitstart to bitend of a
// value in a contiguous array of 64 bit unsigned integers in host byte order. The values must be
// sorted in ascending order. It returns the item number when found or the initial last value when
// not found, plus the updated first and last positions.
func (mf *TMMFile) ColFindLastSubUint64(offset uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColHasNextUint8 reports whether the item after pos matches the search value in a contiguous
// array of 8 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindFirstUint8 to walk forward over the matching items. It returns the result and the updated
// position.
func (mf *TMMFile) ColHasNextUint8(offset, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextUint16 reports whether the item after pos matches the search value in a contiguous
// array of 16 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindFirstUint16 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) ColHasNextUint16(offset, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextUint32 reports whether the item after pos matches the search value in a contiguous
// array of 32 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindFirstUint32 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) ColHasNextUint32(offset, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextUint64 reports whether the item after pos matches the search value in a contiguous
// array of 64 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindFirstUint64 to walk forward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) ColHasNextUint64(offset, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint8 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a contiguous array of 8 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindFirstSubUint8 to walk forward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) ColHasNextSubUint8(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint16 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a contiguous array of 16 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindFirstSubUint16 to walk forward over the matching items.
// It returns the result and the updated position.
func (mf *TMMFile) ColHasNextSubUint16(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint32 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a contiguous array of 32 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindFirstSubUint32 to walk forward over the matching items.
// It returns the result and the updated position.
func (mf *TMMFile) ColHasNextSubUint32(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint64 reports whether the bits from bitstart to bitend of the item after pos
// matches the search value in a contiguous array of 64 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindFirstSubUint64 to walk forward over the matching items.
// It returns the result and the updated position.
func (mf *TMMFile) ColHasNextSubUint64(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint8 reports whether the item before pos matches the search value in a contiguous
// array of 8 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindLastUint8 to walk backward over the matching items. It returns the result and the updated
// position.
func (mf *TMMFile) ColHasPrevUint8(offset, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint16 reports whether the item before pos matches the search value in a contiguous
// array of 16 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindLastUint16 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) ColHasPrevUint16(offset, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint32 reports whether the item before pos matches the search value in a contiguous
// array of 32 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindLastUint32 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) ColHasPrevUint32(offset, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint64 reports whether the item before pos matches the search value in a contiguous
// array of 64 bit unsigned integers in host byte order. Set pos to the item returned by
// ColFindLastUint64 to walk backward over the matching items. It returns the result and the
// updated position.
func (mf *TMMFile) ColHasPrevUint64(offset, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint8 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a contiguous array of 8 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindLastSubUint8 to walk backward over the matching items. It
// returns the result and the updated position.
func (mf *TMMFile) ColHasPrevSubUint8(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint16 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a contiguous array of 16 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindLastSubUint16 to walk backward over the matching items.
// It returns the result and the updated position.
func (mf *TMMFile) ColHasPrevSubUint16(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint32 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a contiguous array of 32 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindLastSubUint32 to walk backward over the matching items.
// It returns the result and the updated position.
func (mf *TMMFile) ColHasPrevSubUint32(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint64 reports whether the bits from bitstart to bitend of the item before pos
// matches the search value in a contiguous array of 64 bit unsigned integers in host byte order.
// Set pos to the item returned by ColFindLastSubUint64 to walk backward over the matching items.
// It returns the result and the updated position.
func (mf *TMMFile) ColHasPrevSubUint64(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}
