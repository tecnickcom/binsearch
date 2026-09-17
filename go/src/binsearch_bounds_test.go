package binsearch

import (
	"syscall"
	"testing"

	"github.com/stretchr/testify/require"
)

func TestCheckBits(t *testing.T) {
	require.NoError(t, checkBits(0, 7, 1))
	require.NoError(t, checkBits(3, 3, 1))
	require.NoError(t, checkBits(0, 63, 8))
	require.ErrorIs(t, checkBits(4, 3, 1), ErrBitRange, "bitstart must not be above bitend")
	require.ErrorIs(t, checkBits(0, 8, 1), ErrBitRange, "bitend must be inside the type")
	require.ErrorIs(t, checkBits(0, 64, 8), ErrBitRange, "bitend must be inside the type")
}

func TestCheckRowRange(t *testing.T) {
	// The whole block of the last item must fit in the available bytes.
	require.True(t, checkRowRange(24, 8, 0, 3, 8))
	require.False(t, checkRowRange(24, 8, 0, 4, 8))
	require.True(t, checkRowRange(24, 8, 4, 3, 4))
	require.False(t, checkRowRange(24, 8, 20, 3, 4))

	// Fewer bytes than the searched type, so no item can be read.
	require.False(t, checkRowRange(4, 8, 0, 1, 8))

	// The value offset inside the block must leave room for the value.
	require.False(t, checkRowRange(8, 8, 4, 1, 8))

	// Every item is at the same address when the blocks have no length.
	require.True(t, checkRowRange(8, 0, 0, 1000, 8))
	require.False(t, checkRowRange(8, 0, 4, 1000, 8))
}

func TestRowSrcUnmapped(t *testing.T) {
	var lmf TMMFile

	_, err := lmf.rowSrc(0, 8, 0, 1, 8)
	require.ErrorIs(t, err, ErrNotMapped, "an unmapped file has no address to search")
}

func TestRowSrcOutOfBounds(t *testing.T) {
	_, err := mf.rowSrc(mf.Size+1, 8, 0, 1, 4)
	require.ErrorIs(t, err, ErrOutOfRange, "the offset must be inside the mapping")

	_, err = mf.rowSrc(0, 8, 0, mf.Size, 4)
	require.ErrorIs(t, err, ErrOutOfRange, "the range must be inside the mapping")

	_, err = mf.rowSrc(0, 8, 0, 0, 4)
	require.NoError(t, err, "an empty range reads nothing")
}

func TestColSrcUnmapped(t *testing.T) {
	var lmf TMMFile

	_, err := lmf.colSrc(0, 1, 8)
	require.ErrorIs(t, err, ErrNotMapped, "an unmapped file has no address to search")
}

func TestColSrcOutOfBounds(t *testing.T) {
	_, err := cmf.colSrc(cmf.Size+8, 1, 8)
	require.ErrorIs(t, err, ErrOutOfRange, "the offset must be inside the mapping")

	_, err = cmf.colSrc(4, 1, 8)
	require.ErrorIs(t, err, ErrMisaligned, "the offset must be a multiple of the column type size")

	_, err = cmf.colSrc(0, cmf.Size, 8)
	require.ErrorIs(t, err, ErrOutOfRange, "the range must be inside the mapping")

	_, err = cmf.colSrc(0, 1, 8)
	require.NoError(t, err)
}

func TestMapError(t *testing.T) {
	require.Equal(t, syscall.EINVAL, mapError(nil), "a failure with no errno is reported as EINVAL")
	require.Equal(t, syscall.ENOENT, mapError(syscall.ENOENT))
}
