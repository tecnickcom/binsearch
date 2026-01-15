// Package binsearch is a Go wrapper for the binsearch C software library.
// It provides fast binary search for columnar data formats.
package binsearch

/*
#cgo CFLAGS: -O3 -pedantic -std=c2x -Wextra -Wno-strict-prototypes -Wcast-align -Wundef -Wformat -Wformat-security -Wshadow
#include "../../c/src/binsearch/binsearch.h"
*/
import "C" //nolint:nolintlint,gci,typecheck

import (
	"fmt"
	"unsafe"
)

// maxcols is the maximum number of indexable columns as in binsearch.h file.
const maxcols = 256

// TMMFile contains the memory mapped file info.
type TMMFile struct {
	Src     unsafe.Pointer // Pointer to the memory map.
	Fd      int            // File descriptor.
	Size    uint64         // File size in bytes.
	DOffset uint64         // Offset to the beginning of the data block (address of the first byte of the first item in the first column).
	DLength uint64         // Length in bytes of the data block.
	NRows   uint64         // Number of rows.
	NCols   uint8          // Number of columns.
	CTBytes []uint8        // Number of bytes per column type (i.e. 1 for uint8_t, 2 for uint16_t, 4 for uint32_t, 8 for uint64_t)
	Index   []uint64       // Index of the offsets to the beginning of each column.
}

// castCTMMFileToGo convert C.mmfile_t to GO TMMFile.
func castCTMMFileToGo(mf C.mmfile_t) TMMFile {
	ncols := uint8(mf.ncols)
	ctbytes := make([]uint8, ncols, maxcols)
	index := make([]uint64, ncols, maxcols)

	var i uint8

	for i = range ncols {
		ctbytes[i] = uint8(mf.ctbytes[i])
		index[i] = uint64(mf.index[i])
	}

	return TMMFile{
		Src:     unsafe.Pointer(mf.src), // #nosec
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

// castGoTMMFileToC convert GO TMMFile to C.mmfile_t.
func castGoTMMFileToC(mf TMMFile) C.mmfile_t {
	var cmf C.mmfile_t
	cmf.src = (*C.uint8_t)(mf.Src)
	cmf.fd = C.int(mf.Fd)
	cmf.size = C.uint64_t(mf.Size)
	cmf.doffset = C.uint64_t(mf.DOffset)
	cmf.dlength = C.uint64_t(mf.DLength)
	cmf.nrows = C.uint64_t(mf.NRows)
	cmf.ncols = C.uint8_t(mf.NCols)

	if len(mf.CTBytes) > 0 {
		cmf.ctbytes = *(*[maxcols]C.uint8_t)(unsafe.Pointer(&mf.CTBytes[0]))
		cmf.index = *(*[maxcols]C.uint64_t)(unsafe.Pointer(&mf.Index[0]))
	}

	return cmf
}

// StringToNTBytes safely convert a string to byte array with an extra null terminator.
// This is to ensure a correct CGO conversion to char*.
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b, s)

	return b
}

// MmapBinFile maps the specified file in memory.
func MmapBinFile(file string, ctbytes []uint8) (TMMFile, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)

	var p unsafe.Pointer

	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}

	var mf C.mmfile_t

	mf.ncols = C.uint8_t(len(ctbytes))

	for k, v := range ctbytes {
		mf.ctbytes[k] = C.uint8_t(v)
	}

	C.mmap_binfile((*C.char)(p), &mf) //nolint:gocritic

	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}

	return castCTMMFileToGo(mf), nil
}

// Close Unmap and close the memory-mapped file.
func (mf TMMFile) Close() error {
	e := int(C.munmap_binfile(castGoTMMFileToC(mf)))
	if e != 0 {
		return fmt.Errorf("got %d error while unmapping the file", e)
	}

	return nil
}

// GetAddress returns the absolute file start position of the specified item (binary block).
func GetAddress(blklen, blkpos, item uint64) uint64 {
	return ((blklen * item) + blkpos)
}

// FindFirstBEUint8 search for the first occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint8 search for the last occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint16 search for the first occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint16 search for the last occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint32 search for the first occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint32 search for the last occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint64 search for the first occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint64 search for the last occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint8 search for the first occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint8 search for the last occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint16 search for the first occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint16 search for the last occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint32 search for the first occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint32 search for the last occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint64 search for the first occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint64 search for the last occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// HasNextBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint8(offset, blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint16(offset, blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint32(offset, blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint64(offset, blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint8(offset, blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint16(offset, blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint32(offset, blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint64(offset, blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// FindFirstLEUint8 search for the first occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint8 search for the last occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint8(offset, blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint16 search for the first occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint16 search for the last occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint16(offset, blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint32 search for the first occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint32 search for the last occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint32(offset, blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint64 search for the first occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint64 search for the last occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint64(offset, blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint8 search for the first occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint8 search for the last occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint16 search for the first occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint16 search for the last occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint32 search for the first occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint32 search for the last occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint64 search for the first occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint64 search for the last occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// HasNextLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint8(offset, blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint16(offset, blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint32(offset, blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint64(offset, blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasNextSubLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint8(offset, blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint16(offset, blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint32(offset, blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint64(offset, blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint8(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint16(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint16_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint32(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint32_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// HasPrevSubLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint64(offset, blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint64_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// --- COLUMN ---

// ColFindFirstUint8 search for the first occurrence of a LE 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint8(offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint8 search for the last occurrence of a LE 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint8(offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint16 search for the first occurrence of a LE 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint16(offset, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint16 search for the last occurrence of a LE 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint16(offset, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint32 search for the first occurrence of a LE 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint32(offset, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint32 search for the last occurrence of a LE 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint32(offset, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstUint64 search for the first occurrence of a LE 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstUint64(offset, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastUint64 search for the last occurrence of a LE 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastUint64(offset, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint8 search for the first occurrence of a LE bit set contained in a 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint8(offset uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint8 search for the last occurrence of a LE bit set contained in a 8 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint8(offset uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint16 search for the first occurrence of a LE bit set contained in a 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint16(offset uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint16 search for the last occurrence of a LE bit set contained in a 16 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint16(offset uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint32 search for the first occurrence of a LE bit set contained in a 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint32(offset uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint32 search for the last occurrence of a LE bit set contained in a 32 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint32(offset uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindFirstSubUint64 search for the first occurrence of a LE bit set contained in a 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindFirstSubUint64(offset uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_first_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColFindLastSubUint64 search for the last occurrence of a LE bit set contained in a 64 bit unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) ColFindLastSubUint64(offset uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.col_find_last_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))

	return ret, uint64(cfirst), uint64(clast)
}

// ColHasNextUint8 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint8(offset, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextUint16 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint16(offset, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextUint32 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint32(offset, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextUint64 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextUint64(offset, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint8 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint8(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint16 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint16(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint32 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint32(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasNextSubUint64 checks if the next occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by ColFindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasNextSubUint64(offset uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_next_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint8 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint8(offset, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint16 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint16(offset, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint32 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint32(offset, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevUint64 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevUint64(offset, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint8 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint8(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint8_t((*C.uint8_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint16 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint16(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint16_t((*C.uint16_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint32 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint32(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint32_t((*C.uint32_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))

	return ret, uint64(cpos)
}

// ColHasPrevSubUint64 checks if the previous occurrence of a LE unsigned integer
// on a memory buffer containing contiguos blocks of unsigned integers of the same type.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after ColFindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by ColFindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) ColHasPrevSubUint64(offset uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.col_has_prev_sub_uint64_t((*C.uint64_t)(unsafe.Add(mf.Src, offset)), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))

	return ret, uint64(cpos)
}
