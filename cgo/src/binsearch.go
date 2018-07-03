package binsearch

/*
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

// FindFirstUint8 search for the first occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint8 search for the last occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint8(blklen, blkpos, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint16 search for the first occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint16 search for the last occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint16(blklen, blkpos, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint32 search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint32 search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint32(blklen, blkpos, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstUint64 search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastUint64 search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint64(blklen, blkpos, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint8 search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint8 search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint8_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint16 search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint16 search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint16_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint32 search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint32 search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint32_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindFirstSubUint64 search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_first_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// FindLastSubUint64 search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	cfirst := C.uint64_t(first)
	clast := C.uint64_t(last)
	ret := uint64(C.find_last_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cfirst, &clast, C.uint64_t(search)))
	return ret, uint64(cfirst), uint64(clast)
}

// HasNextUint8 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextUint16 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextUint32 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextUint64 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint8 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint16 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint32 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasNextSubUint64 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_next_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), &cpos, C.uint64_t(last), C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint8 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint16 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint32 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevUint64 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint8 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint8_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint8_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint16 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint16_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint16_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint32 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint32_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint32_t(search)))
	return ret, uint64(cpos)
}

// HasPrevSubUint64 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	cpos := C.uint64_t(pos)
	ret := bool(C.has_prev_sub_uint64_t((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(first), &cpos, C.uint64_t(search)))
	return ret, uint64(cpos)
}
