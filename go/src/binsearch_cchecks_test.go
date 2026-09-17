//go:build binsearchdiff

package binsearch

import (
	"testing"

	"github.com/stretchr/testify/require"
)

// TestCheckRowRangeMatchesC checks the Go bounds check against the C one it is
// a copy of. The two are written separately because calling the C one would
// cost a cgo crossing on every search, so this is what keeps them the same.
func TestCheckRowRangeMatchesC(t *testing.T) {
	t.Parallel()

	sizes := []uint64{0, 1, 2, 3, 4, 7, 8, 15, 16, 31, 64, 1 << 20}
	tsizes := []uint64{1, 2, 4, 8}

	for _, avail := range sizes {
		for _, blklen := range sizes {
			for _, blkpos := range sizes {
				for _, last := range sizes {
					for _, tsize := range tsizes {
						want := cCheckRowRange(avail, blklen, blkpos, last, tsize)
						got := checkRowRange(avail, blklen, blkpos, last, tsize)
						require.Equal(t, want, got,
							"avail %d blklen %d blkpos %d last %d tsize %d",
							avail, blklen, blkpos, last, tsize)
					}
				}
			}
		}
	}
}

// TestCheckBitsMatchesC checks the Go bit range check against the C one.
func TestCheckBitsMatchesC(t *testing.T) {
	t.Parallel()

	for _, tsize := range []uint64{1, 2, 4, 8} {
		for bitstart := range 65 {
			for bitend := range 65 {
				want := cCheckBits(uint8(bitstart), uint8(bitend), tsize)
				got := checkBits(uint8(bitstart), uint8(bitend), tsize) == nil
				require.Equal(t, want, got, "bitstart %d bitend %d tsize %d", bitstart, bitend, tsize)
			}
		}
	}
}
