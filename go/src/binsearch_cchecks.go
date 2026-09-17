//go:build binsearchdiff

// The C argument checks are only reached from the differential test below, and
// cgo cannot be used from a _test.go file, so this file would otherwise ship to
// consumers as code nothing in the package calls. The build tag keeps it out of
// a normal build; "make test" sets it.

package binsearch

/*
#cgo noescape binsearch_check_row_range
#cgo nocallback binsearch_check_row_range
#cgo noescape binsearch_check_bits
#cgo nocallback binsearch_check_bits

#include "../../c/src/binsearch/binsearch.h"
*/
import "C"

// The argument checks of this package are written twice: once in Go, where the
// searches use them, and once in C, where the library and the Python extension
// use them. Calling the C ones from Go would cost a cgo crossing on every
// search, which is more than the search itself on data that fits in the cache,
// so they are kept as separate copies and compared against each other by
// TestCheckRowRangeMatchesC and TestCheckBitsMatchesC.

// cCheckRowRange reports what binsearch_check_row_range reports.
func cCheckRowRange(avail, blklen, blkpos, last, tsize uint64) bool {
	return bool(C.binsearch_check_row_range(
		C.uint64_t(avail), C.uint64_t(blklen), C.uint64_t(blkpos),
		C.uint64_t(last), C.uint64_t(tsize)))
}

// cCheckBits reports what binsearch_check_bits reports.
func cCheckBits(bitstart, bitend uint8, tsize uint64) bool {
	return bool(C.binsearch_check_bits(
		C.uint8_t(bitstart), C.uint8_t(bitend), C.uint64_t(tsize)))
}
