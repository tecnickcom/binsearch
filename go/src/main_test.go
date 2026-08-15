package binsearch

import (
	"os"
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/require"
)

var (
	mf, cmf TMMFile
	retCode int
)

func closeTMMFile(mmf *TMMFile) {
	err := mmf.Close()
	if err != nil {
		retCode++
	}
}

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../c/test/data/test_data.bin", []uint8{4})
	if err != nil {
		os.Exit(1)
	}

	cmf, err = MmapBinFile("../../c/test/data/test_data_col.bin", []uint8{1, 2, 4, 8})
	if err != nil {
		os.Exit(1)
	}

	retCode += m.Run()

	// NOTE: os.Exit does not run the deferred functions, so the fixtures are
	// unmapped explicitly.
	closeTMMFile(&mf)
	closeTMMFile(&cmf)

	os.Exit(retCode)
}

func TestClose(t *testing.T) {
	lmf, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{4})
	require.NoError(t, err)

	err = lmf.Close()
	require.NoError(t, err)
}

func TestCloseTwice(t *testing.T) {
	lmf, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{4})
	require.NoError(t, err)

	err = lmf.Close()
	require.NoError(t, err)

	// The second call must not unmap an address and close a descriptor that
	// have been reused since the first one.
	err = lmf.Close()
	require.Error(t, err)
}

func TestCloseError(t *testing.T) {
	lmf := TMMFile{}
	err := lmf.Close()

	require.Error(t, err)
}

func TestCloseUnmapError(t *testing.T) {
	lmf, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{4})
	require.NoError(t, err)

	src, fd, size := lmf.Src, lmf.Fd, lmf.Size

	// A zero length is always rejected by munmap, so the call fails without
	// releasing the mapping or closing the descriptor.
	lmf.Size = 0
	require.Error(t, lmf.Close())

	// Release the mapping that the failed call left behind.
	lmf.Src, lmf.Fd, lmf.Size = src, fd, size
	require.NoError(t, lmf.Close())
}

func TestMmapBinFileError(t *testing.T) {
	_, err := MmapBinFile("error", []uint8{4})
	require.Error(t, err)
}

func TestMmapBinFileNoColumns(t *testing.T) {
	_, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{})
	require.Error(t, err)
}

func TestMmapBinFileTooManyColumns(t *testing.T) {
	_, err := MmapBinFile("../../c/test/data/test_data.bin", make([]uint8, maxncols+1))
	require.Error(t, err)
}

func TestMmapBinFileInvalidColumnType(t *testing.T) {
	_, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{4, 12})
	require.Error(t, err)
}

func TestGetAddress(t *testing.T) {
	h := GetAddress(3, 5, 7)
	require.Equal(t, uint64(26), h)
}

func TestLoadArrow(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data_arrow.bin", ctbytes)

	require.NoError(t, err)

	require.LessOrEqual(t, 0, amf.Fd, "can't open test_data_arrow.bin for reading")
	require.Equal(t, uint64(730), amf.Size)
	require.Equal(t, uint64(376), amf.DOffset)
	require.Equal(t, uint64(136), amf.DLength)
	require.Equal(t, uint64(11), amf.NRows)
	require.Equal(t, uint8(2), amf.NCols)
	require.Equal(t, uint64(376), amf.Index[0])
	require.Equal(t, uint64(424), amf.Index[1])

	err = amf.Close()
	require.NoError(t, err)
}

func TestLoadFeather(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data_feather.bin", ctbytes)

	require.NoError(t, err)

	require.LessOrEqual(t, 0, amf.Fd, "can't open test_data_feather.bin for reading")
	require.Equal(t, uint64(384), amf.Size)
	require.Equal(t, uint64(8), amf.DOffset)
	require.Equal(t, uint64(136), amf.DLength)
	require.Equal(t, uint64(11), amf.NRows)
	require.Equal(t, uint8(2), amf.NCols)
	require.Equal(t, uint64(8), amf.Index[0])
	require.Equal(t, uint64(56), amf.Index[1])

	err = amf.Close()
	require.NoError(t, err)
}

func TestLoadBinsrc(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data_binsrc.bin", ctbytes)

	require.NoError(t, err)

	require.LessOrEqual(t, 0, amf.Fd, "can't open test_data_binsrc.bin for reading")
	require.Equal(t, uint64(176), amf.Size)
	require.Equal(t, uint64(40), amf.DOffset)
	require.Equal(t, uint64(136), amf.DLength)
	require.Equal(t, uint64(11), amf.NRows)
	require.Equal(t, uint8(2), amf.NCols)
	require.Equal(t, uint64(40), amf.Index[0])
	require.Equal(t, uint64(88), amf.Index[1])

	err = amf.Close()
	require.NoError(t, err)
}

// TestLoadPaddedColumns checks that the number of rows is derived from the
// padded layout: every column but the last is padded to an 8-byte boundary, so
// 100 rows of two uint8 columns occupy 100 + 4 padding + 100 bytes.
func TestLoadPaddedColumns(t *testing.T) {
	data := make([]byte, 0, 204)

	for i := range 100 {
		data = append(data, byte(i))
	}

	data = append(data, make([]byte, 4)...)

	for i := range 100 {
		data = append(data, byte(i))
	}

	file := filepath.Join(t.TempDir(), "padded.bin")
	require.NoError(t, os.WriteFile(file, data, 0o600))

	pmf, err := MmapBinFile(file, []uint8{1, 1})
	require.NoError(t, err)

	require.Equal(t, uint64(204), pmf.Size)
	require.Equal(t, uint64(0), pmf.DOffset)
	require.Equal(t, uint64(204), pmf.DLength)
	require.Equal(t, uint64(100), pmf.NRows)
	require.Equal(t, uint8(2), pmf.NCols)
	require.Equal(t, uint64(0), pmf.Index[0])
	require.Equal(t, uint64(104), pmf.Index[1])

	item, _, _ := pmf.ColFindFirstUint8(pmf.Index[1], 0, pmf.NRows, 42)
	require.Equal(t, uint64(42), item)

	require.NoError(t, pmf.Close())
}
