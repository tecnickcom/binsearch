package binsearch

/*
#include "../../src/binsearch.h"
#include "../../src/binsearch.c"
*/
import "C"
import "unsafe"
import "fmt"

// TMMFile contains the memory mapped file info
type TMMFile struct {
	Src  unsafe.Pointer
	Fd   int
	Size uint64
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
		p = unsafe.Pointer(&bfile[0]) /* #nosec */
	}
	mf := (C.mmap_binfile((*C.char)(p)))
	if mf.fd < 0 || mf.size == 0 || mf.src == nil {
		return TMMFile{}, fmt.Errorf("unable to map the file: %s", file)
	}
	return TMMFile{unsafe.Pointer(mf.src), int(mf.fd), uint64(mf.size)}, nil // #nosec
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

// FindFirstUint32be search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindFirstUint32be(blklen, blkpos, first, last uint64, search uint32) uint64 {
	return uint64(C.find_first_uint32be((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), C.uint64_t(last), C.uint32_t(search)))
}

// FindLastUint32be search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindLastUint32be(blklen, blkpos, first, last uint64, search uint32) uint64 {
	return uint64(C.find_last_uint32be((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), C.uint64_t(last), C.uint32_t(search)))
}

// FindFirstUint64be search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindFirstUint64be(blklen, blkpos, first, last uint64, search uint64) uint64 {
	return uint64(C.find_first_uint64be((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), C.uint64_t(last), C.uint64_t(search)))
}

// FindLastUint64be search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found.
func (mf TMMFile) FindLastUint64be(blklen, blkpos, first, last uint64, search uint64) uint64 {
	return uint64(C.find_last_uint64be((*C.uchar)(mf.Src), C.uint64_t(blklen), C.uint64_t(blkpos), C.uint64_t(first), C.uint64_t(last), C.uint64_t(search)))
}
