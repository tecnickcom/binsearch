package binsearch

import (
	"sync"
	"testing"

	"github.com/stretchr/testify/require"
)

// The row mode fixture holds 251 blocks of 16 bytes, starting at offset 0.
const (
	rowNRows  = uint64(251)
	rowBlkLen = uint64(16)
)

// searchValues covers the whole range of a byte, so that the fixtures report
// hits and misses at both ends and in between.
func searchValues() []uint64 {
	out := make([]uint64, 0, 40)
	for v := 0; v < 256; v += 7 {
		out = append(out, uint64(v))
	}

	return append(out, 0, 1, 0xFF)
}

// colRanges lists the sub-ranges the column tests search, including the empty
// one and the one that covers the whole fixture.
func colRanges(nrows uint64) [][2]uint64 {
	return [][2]uint64{{0, 0}, {5, 5}, {3, 4}, {0, 1}, {10, 100}, {0, nrows}}
}

// TestColFindRange checks that ColFindRange reports the half-open range that
// ColFindFirst and ColFindLast delimit, for every byte order and type.
func TestColFindRange(t *testing.T) {
	t.Parallel()

	for _, o := range colOrders8() {
		t.Run(o.name, func(t *testing.T) {
			t.Parallel()

			offset := o.mf.Index[0]

			for _, r := range colRanges(o.mf.NRows) {
				for _, s := range searchValues() {
					wantCount, wantFirst, wantLast := colRangeReference8(t, o, offset, r[0], r[1], uint8(s))

					n, first, last, err := o.findRange(offset, r[0], r[1], uint8(s))
					require.NoError(t, err)
					require.Equal(t, wantCount, n, "range [%d, %d) search %d", r[0], r[1], s)

					if n > 0 {
						require.Equal(t, wantFirst, first)
						require.Equal(t, wantLast, last)
					}
				}
			}
		})
	}
}

// colRangeReference8 returns the range of matching items, computed with the two
// searches that ColFindRange replaces.
func colRangeReference8(t *testing.T, o colOrder[uint8], offset, first, last uint64, search uint8) (uint64, uint64, uint64) {
	t.Helper()

	hit, _, lo, err := o.findFirst(offset, first, last, search)
	require.NoError(t, err)

	if hit == last {
		return 0, lo, lo
	}

	_, _, hi, err := o.findLast(offset, first, last, search)
	require.NoError(t, err)

	return hi - lo, lo, hi
}

// TestColFindMany checks that ColFindMany reports, for every searched value,
// what ColFindFirst reports for it, for every number of values around a batch
// boundary.
func TestColFindMany(t *testing.T) {
	t.Parallel()

	// The batch is eight values wide, so the counts straddle one and two of
	// them and leave every possible remainder.
	counts := []int{0, 1, 7, 8, 9, 15, 16, 17, 20}

	for _, o := range colOrders8() {
		t.Run(o.name, func(t *testing.T) {
			t.Parallel()

			offset := o.mf.Index[0]
			values := make([]uint8, 20)

			for i := range values {
				values[i] = uint8((i * 13) % 256)
			}

			for _, r := range colRanges(o.mf.NRows) {
				for _, count := range counts {
					search := values[:count]

					pos, err := o.findMany(offset, r[0], r[1], search)
					require.NoError(t, err)
					require.Len(t, pos, count)

					for i, v := range search {
						want, _, _, err := o.findFirst(offset, r[0], r[1], v)
						require.NoError(t, err)
						require.Equal(t, want, pos[i], "range [%d, %d) value %d", r[0], r[1], v)
					}
				}
			}
		})
	}
}

// TestFindRange checks the row mode form against the two searches it replaces.
func TestFindRange(t *testing.T) {
	t.Parallel()

	for _, r := range [][2]uint64{{0, 0}, {3, 4}, {0, 1}, {0, rowNRows}} {
		for _, s := range searchValues() {
			hit, _, lo, err := mf.FindFirstBEUint8(0, rowBlkLen, 0, r[0], r[1], uint8(s))
			require.NoError(t, err)

			wantCount, wantFirst, wantLast := uint64(0), lo, lo

			if hit != r[1] {
				_, _, hi, err := mf.FindLastBEUint8(0, rowBlkLen, 0, r[0], r[1], uint8(s))
				require.NoError(t, err)

				wantCount, wantLast = hi-lo, hi
			}

			n, first, last, err := mf.FindRangeBEUint8(0, rowBlkLen, 0, r[0], r[1], uint8(s))
			require.NoError(t, err)
			require.Equal(t, wantCount, n, "range [%d, %d) search %d", r[0], r[1], s)

			if n > 0 {
				require.Equal(t, wantFirst, first)
				require.Equal(t, wantLast, last)
			}
		}
	}
}

// TestFindMany checks the row mode form against the search it replaces.
func TestFindMany(t *testing.T) {
	t.Parallel()

	values := make([]uint8, 20)
	for i := range values {
		values[i] = uint8((i * 13) % 256)
	}

	for _, count := range []int{0, 1, 8, 9, 20} {
		search := values[:count]

		pos, err := mf.FindManyBEUint8(0, rowBlkLen, 0, 0, rowNRows, search)
		require.NoError(t, err)
		require.Len(t, pos, count)

		for i, v := range search {
			want, _, _, err := mf.FindFirstBEUint8(0, rowBlkLen, 0, 0, rowNRows, v)
			require.NoError(t, err)
			require.Equal(t, want, pos[i])
		}
	}
}

// TestFindRangeSub and TestFindManySub check that matching the whole value
// selects the same items as the plain search does.
func TestFindRangeSub(t *testing.T) {
	t.Parallel()

	for _, s := range searchValues() {
		wantN, wantF, wantL, err := mf.FindRangeBEUint8(0, rowBlkLen, 0, 0, rowNRows, uint8(s))
		require.NoError(t, err)

		n, f, l, err := mf.FindRangeSubBEUint8(0, rowBlkLen, 0, 0, 7, 0, rowNRows, uint8(s))
		require.NoError(t, err)
		require.Equal(t, [3]uint64{wantN, wantF, wantL}, [3]uint64{n, f, l})
	}
}

func TestFindManySub(t *testing.T) {
	t.Parallel()

	search := []uint8{0, 13, 26, 39, 52, 65, 78, 91, 104, 117}

	want, err := mf.FindManyBEUint8(0, rowBlkLen, 0, 0, rowNRows, search)
	require.NoError(t, err)

	got, err := mf.FindManySubBEUint8(0, rowBlkLen, 0, 0, 7, 0, rowNRows, search)
	require.NoError(t, err)
	require.Equal(t, want, got)
}

// TestCloseTwiceConcurrently checks that two goroutines closing the same file
// do not both unmap it: one releases the mapping and the other is refused.
func TestCloseTwiceConcurrently(t *testing.T) {
	t.Parallel()

	for range 64 {
		tmf, err := MmapBinFile(colFile, colCTBytes)
		require.NoError(t, err)

		var (
			wg   sync.WaitGroup
			errs [2]error
		)

		wg.Add(2)

		for i := range 2 {
			go func() {
				defer wg.Done()

				errs[i] = tmf.Close()
			}()
		}

		wg.Wait()

		// Exactly one of the two releases the mapping, and the other is told
		// that there was nothing left to release.
		require.NotEqual(t, errs[0] == nil, errs[1] == nil, "got %v and %v", errs[0], errs[1])

		if errs[0] != nil {
			require.ErrorIs(t, errs[0], ErrNotMapped)
		} else {
			require.ErrorIs(t, errs[1], ErrNotMapped)
		}
	}
}
