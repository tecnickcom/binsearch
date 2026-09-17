// Package binsearch wraps the binsearch C library.
//
// It searches unsigned integers in a memory mapped file, in two layouts:
// row mode, where the values sit at a fixed offset inside adjacent
// constant-length blocks, and column mode, where they form a contiguous array
// of one type. The values must be sorted in ascending order.
//
// The BE and LE methods read big-endian and little-endian values in both
// layouts, and the Sub methods match only the bits from bitstart to bitend,
// counted from the most significant bit.
//
// The find methods narrow their first and last arguments to a bound of the
// searched value: FindFirst to the lower bound, which is the item it found,
// and FindLast to the upper bound, which is the position after it. On return
// last is that bound and first is the same position, or one below it when the
// value is absent, in which case it can fall one position below the range that
// was passed in. Neither describes a range to iterate: the HasNext and HasPrev
// methods take the bounds of the original range.
//
// The two bounds delimit the items equal to the searched value, so the last of
// FindFirst and the last of FindLast are the ends of a half-open range. The
// FindRange methods compute both in one descent and return the number of items
// in it, which costs about half of the two searches on data larger than the
// cache. Prefer them to walking the items with HasNext, which crosses into C
// once per item.
//
// The FindMany methods search a slice of values in one call, advancing the
// searches in lockstep so that their cache misses overlap. That is about three
// to four times as fast per value on data larger than the CPU cache and still
// held in RAM, and it crosses into C once for the whole batch. On a file too
// large for RAM the misses become page faults, which the kernel serves one at a
// time; the Prefetch field of the mapping selects whether the pages of a batch
// are asked for before they are read, which puts those reads in the device
// queue together.
//
// MmapBinFile maps a file and reads the header of the BINSRC1, Apache Arrow and
// Feather formats; for any other content the caller lists the size of each
// column type. The Border field reports the byte order of the values, which
// tells the caller whether to use the BE or the LE methods.
//
// Every search method checks its arguments against the memory map and returns
// an error when they do not describe a range of items inside it, so that a
// wrong offset or bound cannot read outside the mapping. The error wraps
// ErrNotMapped, ErrOutOfRange, ErrMisaligned or ErrBitRange, and the other
// results are then the ones of a value that was not found.
//
// A mapped file may be searched from several goroutines at once, as the
// searches only read it. Close must not run at the same time as a search: it
// releases the mapping and clears the fields the searches read. Two concurrent
// calls to Close are safe, and only one of them unmaps the file.
package binsearch

/*
// _DEFAULT_SOURCE makes the POSIX memory advice macros visible. Without it a
// strict ISO C build sees neither MADV_WILLNEED nor POSIX_MADV_WILLNEED, and
// the page requests of the FindMany methods are not compiled at all.
#cgo CFLAGS: -O3 -pedantic -std=c2x -D_DEFAULT_SOURCE -Wall -Wextra -Wno-strict-prototypes -Wcast-align -Wundef -Wformat -Wformat-security -Wshadow

// Neither function retains a pointer it is given and neither calls back into
// Go, so the arguments of the wrappers can stay on the goroutine stack instead
// of being moved to the heap.
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
	"sync/atomic"
	"syscall"
	"unsafe"
)

// maxncols is the largest number of columns that fits the ncols field (uint8_t).
const maxncols = 255

// bitsPerByte is the number of bits of a byte, used to turn the size of a
// searched type into the number of bits a Sub search can address.
const bitsPerByte = 8

// Byte order of the values stored in a data block.
const (
	OrderLE = uint8(0) // The values are little-endian: use the LE methods.
	OrderBE = uint8(1) // The values are big-endian: use the BE methods.
)

// Errors reported when the arguments of a search do not describe a range of
// items inside the memory mapped file. They are wrapped with the values that
// were rejected, so they are matched with errors.Is rather than compared.
var (
	// ErrNotMapped is reported when the file is not memory mapped, which is
	// also the state a file is left in by Close.
	ErrNotMapped = errors.New("the file is not memory mapped")

	// ErrOutOfRange is reported when the searched items are not all inside the
	// memory mapped file.
	ErrOutOfRange = errors.New("the search range is outside the memory mapped file")

	// ErrMisaligned is reported when the byte offset of a column is not a
	// multiple of the size of its type, which the column mode searches require
	// because they read the values through a typed pointer.
	ErrMisaligned = errors.New("the column offset is not a multiple of the size of the column type")

	// ErrBitRange is reported when the bits a Sub search matches are not inside
	// the searched type.
	ErrBitRange = errors.New("the bit range is not inside the searched type")
)

// TMMFile describes a memory mapped file.
//
// A mapped file may be searched from several goroutines at once. Close must not
// run at the same time as a search.
type TMMFile struct {
	Src     unsafe.Pointer // Pointer to the memory map.
	Fd      int            // File descriptor.
	Size    uint64         // File size in bytes.
	DOffset uint64         // Byte offset of the first item of the first column.
	DLength uint64         // Length in bytes of the data block.
	NRows   uint64         // Number of rows.
	NCols   uint8          // Number of columns.
	Border  uint8          // Byte order of the values: OrderLE or OrderBE.
	CTBytes []uint8        // Size in bytes of each column type (1, 2, 4 or 8).
	Index   []uint64       // Byte offset of the first item of each column.
	// Prefetch selects whether the FindMany methods ask for the pages of a
	// batch before reading them. MmapBinFile sets it to PrefetchAuto.
	Prefetch PrefetchMode
}

// PrefetchMode selects whether the FindMany methods ask for the pages of a
// batch before reading them.
//
// The request only pays off when the pages are not resident. On a file too
// large for RAM the misses of a batch become page faults, which the kernel
// serves one at a time, and asking for the whole batch first puts the reads in
// the device queue together. On data already in memory it is a system call per
// value per step that buys nothing and costs more than the search.
type PrefetchMode uint32

const (
	// PrefetchAuto asks when a timed batch is slow enough to be reading from a
	// device. It is the value MmapBinFile sets.
	PrefetchAuto PrefetchMode = 0
	// PrefetchAlways always asks.
	PrefetchAlways PrefetchMode = 1
	// PrefetchNever never asks.
	PrefetchNever PrefetchMode = 2
)

// castCTMMFileToGo converts a C.mmfile_t to a TMMFile.
func castCTMMFileToGo(mf *C.mmfile_t) TMMFile {
	ncols := uint8(mf.ncols)
	ctbytes := make([]uint8, ncols)
	index := make([]uint64, ncols)

	var i uint8

	for i = range ncols {
		ctbytes[i] = uint8(mf.ctbytes[i])
		index[i] = uint64(mf.index[i])
	}

	return TMMFile{
		Src:      unsafe.Pointer(mf.src),
		Fd:       int(mf.fd),
		Size:     uint64(mf.size),
		DOffset:  uint64(mf.doffset),
		DLength:  uint64(mf.dlength),
		NRows:    uint64(mf.nrows),
		NCols:    ncols,
		Border:   uint8(mf.border),
		CTBytes:  ctbytes,
		Index:    index,
		Prefetch: PrefetchMode(mf.prefetch),
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

// checkBits reports whether the bit range is inside a value of tsize bytes.
func checkBits(bitstart, bitend uint8, tsize uint64) error {
	if (bitstart > bitend) || (uint64(bitend) >= (tsize * bitsPerByte)) {
		return fmt.Errorf("%w: bitstart %d, bitend %d, type of %d bytes", ErrBitRange, bitstart, bitend, tsize)
	}

	return nil
}

// checkRowRange reports whether every item of a row mode search is inside the
// memory map.
//
// It is the Go form of binsearch_check_row_range, which the C library and the
// Python extension use: calling that one would cost a cgo crossing per search,
// which is more than the search itself on data that fits in the cache. The two
// are checked against each other by TestCheckRowRangeMatchesC.
//
// The last item that can be read is (last - 1); the values are compared using a
// division because the products can overflow.
func checkRowRange(avail, blklen, blkpos, last, tsize uint64) bool {
	if last == 0 { // no item is read
		return true
	}

	if (avail < tsize) || (blkpos > (avail - tsize)) {
		return false
	}

	if blklen == 0 { // every item is at the same address
		return true
	}

	return (last - 1) <= (((avail - tsize) - blkpos) / blklen)
}

// MmapBinFile maps the specified file in memory.
//
// The ctbytes slice lists the number of bytes of each column type: it must have
// at most maxncols entries, each of them 1, 2, 4 or 8. Pass an empty slice for a
// BINSRC1 file, which carries the number of columns and their type sizes in its
// header and ignores the ones given here.
func MmapBinFile(file string, ctbytes []uint8) (TMMFile, error) {
	if len(ctbytes) > maxncols {
		return TMMFile{}, fmt.Errorf("the number of columns must not exceed %d: got %d", maxncols, len(ctbytes))
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

	// The two-result form of a cgo call reports the errno the C function left
	// behind, which mmap_binfile preserves across the cleanup of a failure.
	_, err := C.mmap_binfile((*C.char)(p), &mf)

	if mf.fd < 0 || mf.size == 0 {
		return TMMFile{}, fmt.Errorf("unable to map the file %s: %w", file, mapError(err))
	}

	return castCTMMFileToGo(&mf), nil
}

// mapError returns the error to report for a failed mapping, falling back to
// EINVAL when the C call left no errno behind.
func mapError(err error) error {
	if err == nil {
		return syscall.EINVAL
	}

	return err
}

// Close unmaps the file and closes its descriptor.
//
// The descriptor is cleared, so that a second call reports an error instead of
// unmapping an address and closing a descriptor that have been reused since.
//
// The address is taken with an atomic swap, so two goroutines closing the same
// file cannot both reach the unmapping: one of them releases it and the other
// reports ErrNotMapped. A plain read and write would let both unmap the address
// and close the descriptor, and the second close would land on whatever
// descriptor number the process has reused in between.
func (mf *TMMFile) Close() error {
	// The mapping is taken before it is released, so the file is marked as
	// unmapped whatever the result reports.
	src := atomic.SwapPointer(&mf.Src, nil)
	if src == nil {
		return ErrNotMapped
	}

	// Only the address, the size and the file descriptor are used to unmap the
	// file, so the column arrays of the C descriptor are left empty.
	var cmf C.mmfile_t

	cmf.src = (*C.uint8_t)(src)
	cmf.fd = C.int(mf.Fd)
	cmf.size = C.uint64_t(mf.Size)

	mf.Fd = -1
	mf.Size = 0

	// munmap_binfile clears the descriptor it is given before it closes it, so
	// the result is the only report of a failure of that close.
	ret, err := C.munmap_binfile(&cmf)
	if ret != 0 {
		return fmt.Errorf("unable to unmap the file: %w", mapError(err))
	}

	return nil
}

// rowSrc returns the address of the first block of a row mode search, or the
// reason the range of items it describes is not inside the memory map.
func (mf *TMMFile) rowSrc(offset, blklen, blkpos, last, tsize uint64) (unsafe.Pointer, error) {
	if mf.Src == nil {
		return nil, ErrNotMapped
	}

	if (offset > mf.Size) || !checkRowRange(mf.Size-offset, blklen, blkpos, last, tsize) {
		return nil, fmt.Errorf("%w: offset %d, blklen %d, blkpos %d, last %d, type of %d bytes, file of %d bytes",
			ErrOutOfRange, offset, blklen, blkpos, last, tsize, mf.Size)
	}

	return unsafe.Add(mf.Src, offset), nil
}

// colSrc returns the address of the first item of a column mode search, or the
// reason the range of items it describes is not inside the memory map.
//
// The offset must be a multiple of the size of the column type, because the
// column mode functions read the values through a typed pointer.
func (mf *TMMFile) colSrc(offset, last, tsize uint64) (unsafe.Pointer, error) {
	if mf.Src == nil {
		return nil, ErrNotMapped
	}

	if (offset % tsize) != 0 {
		return nil, fmt.Errorf("%w: offset %d, type of %d bytes", ErrMisaligned, offset, tsize)
	}

	if (offset > mf.Size) || (last > ((mf.Size - offset) / tsize)) {
		return nil, fmt.Errorf("%w: offset %d, last %d, type of %d bytes, file of %d bytes",
			ErrOutOfRange, offset, last, tsize, mf.Size)
	}

	return unsafe.Add(mf.Src, offset), nil
}

// GetAddress returns the byte offset of a value inside a memory mapped file.
func GetAddress(blklen, blkpos, item uint64) uint64 {
	return ((blklen * item) + blkpos)
}
