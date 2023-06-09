package binsearch

import (
	"os"
	"testing"

	"github.com/stretchr/testify/require"
)

//nolint:gochecknoglobals
var (
	mf, cmf TMMFile
	retCode int
	nrows   uint64 = 251
)

func closeTMMFile(mmf TMMFile) {
	err := mmf.Close()
	if err != nil {
		retCode++
	}
}

func TestMain(m *testing.M) {
	var err error

	// memory map the input files

	mf, err = MmapBinFile("../../c/test/data/test_data.bin", []uint8{12})
	if err != nil {
		os.Exit(1)
	}

	defer closeTMMFile(mf)

	cmf, err = MmapBinFile("../../c/test/data/test_data_col.bin", []uint8{1, 2, 4, 8})
	if err != nil {
		os.Exit(1)
	}

	defer closeTMMFile(cmf)

	retCode += m.Run()

	os.Exit(retCode)
}

func TestClose(t *testing.T) {
	lmf, err := MmapBinFile("../../c/test/data/test_data.bin", []uint8{12})
	require.NoError(t, err)

	err = lmf.Close()
	require.NoError(t, err)
}

func TestCloseError(t *testing.T) {
	lmf := TMMFile{}
	err := lmf.Close()

	require.Error(t, err)
}

func TestMmapBinFileError(t *testing.T) {
	_, err := MmapBinFile("error", []uint8{})
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
