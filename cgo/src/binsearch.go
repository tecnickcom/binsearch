package binsearch

/*
#cgo CFLAGS: -O3 -pedantic -std=c99 -Wextra -Wno-strict-prototypes -Wcast-align -Wundef -Wformat-security -Wshadow
#include "../../c/src/binsearch.h"
#include "../../c/src/binsearch.c"
*/
import "C"
import "unsafe"
import "fmt"

// TMMFile contains the memory mapped file info
type TMMFile struct {
	Src   unsafe.Pointer
	Fd    int
	Size  uint64
	Last  uint64
	Index []uint64
}

// StringToNTBytes safely convert a string to byte array with an extra null terminator
// This is to ensure a correct CGO conversion to char*
func StringToNTBytes(s string) []byte {
	b := make([]byte, len(s)+1)
	copy(b[:], s)
	return b
}

// MmapBinFile maps the specified file in memory.
func MmapBinFile(file string) (TMMFile, error) {
	bfile := StringToNTBytes(file)
	flen := len(bfile)
	var p unsafe.Pointer
	if flen > 0 {
		p = unsafe.Pointer(&bfile[0]) // #nosec
	}
	var mf C.mmfile_t
	C.mmap_binfile((*C.char)(p), &mf)
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return TMMFile{unsafe.Pointer(mf.src), int(mf.fd), uint64(mf.size), uint64(mf.last), []uint64{}}, nil // #nosec
}

// Close Unmap and close the memory-mapped file.
func (mf TMMFile) Close() error {
	var tmf C.mmfile_t
	tmf.src = (*C.uchar)(mf.Src)
	tmf.fd = C.int(mf.Fd)
	tmf.size = C.uint64_t(mf.Size)
	e := int(C.munmap_binfile(tmf))
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
func (mf TMMFile) FindFirstBEUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint8 search for the last occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint16 search for the first occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint16 search for the last occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint32 search for the first occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint32 search for the last occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstBEUint64 search for the first occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastBEUint64 search for the last occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint8 search for the first occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint8 search for the last occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint16 search for the first occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint16 search for the last occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint32 search for the first occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint32 search for the last occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubBEUint64 search for the first occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubBEUint64 search for the last occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// HasNextBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_be_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// FindFirstLEUint8 search for the first occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint8 search for the last occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint16 search for the first occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint16 search for the last occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint32 search for the first occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint32 search for the last occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstLEUint64 search for the first occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastLEUint64 search for the last occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint8 search for the first occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint8 search for the last occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint16 search for the first occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint16 search for the last occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint32 search for the first occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint32 search for the last occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubLEUint64 search for the first occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubLEUint64 search for the last occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// HasNextLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_le_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}
