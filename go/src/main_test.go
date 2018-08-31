package binsearch

import "testing"
import "os"

var mf, cmf TMMFile
var retCode int

var nrows uint64 = 251

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
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	err = lmf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}

func TestCloseError(t *testing.T) {
	lmf := TMMFile{}
	err := lmf.Close()
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestMmapBinFileError(t *testing.T) {
	_, err := MmapBinFile("error", []uint8{})
	if err == nil {
		t.Errorf("An error was expected")
	}
}

func TestGetAddress(t *testing.T) {
	h := GetAddress(3, 5, 7)
	if h != 26 {
		t.Errorf("Expected 26, got %d", h)
	}
}

func TestLoadArrow(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data.arrow", ctbytes)
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	if amf.Fd < 0 {
		t.Errorf("can't open test_data.arrow for reading")
	}
	if amf.Size != 730 {
		t.Errorf("Size : Expecting 730 bytes, got instead: %d", amf.Size)
	}
	if amf.DOffset != 376 {
		t.Errorf("DOffset : Expecting 376 bytes, got instead: %d", amf.DOffset)
	}
	if amf.DLength != 136 {
		t.Errorf("DLength : Expecting 136 bytes, got instead: %d", amf.DLength)
	}
	if amf.NRows != 11 {
		t.Errorf("mf.NRows : Expecting 11 items, got instead: %d", amf.NRows)
	}
	if amf.NCols != 2 {
		t.Errorf("mf.NCols : Expecting 2 items, got instead: %d", amf.NCols)
	}
	if amf.Index[0] != 376 {
		t.Errorf("mf.Index[0] : Expecting 376 bytes, got instead: %d", amf.Index[0])
	}
	if amf.Index[1] != 424 {
		t.Errorf("mf.Index[1] : Expecting 424 bytes, got instead: %d", amf.Index[1])
	}
	err = amf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}

func TestLoadFeather(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data.feather", ctbytes)
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	if amf.Fd < 0 {
		t.Errorf("can't open test_data.arrow for reading")
	}
	if amf.Size != 384 {
		t.Errorf("Size : Expecting 384 bytes, got instead: %d", amf.Size)
	}
	if amf.DOffset != 8 {
		t.Errorf("DOffset : Expecting 8 bytes, got instead: %d", amf.DOffset)
	}
	if amf.DLength != 136 {
		t.Errorf("DLength : Expecting 136 bytes, got instead: %d", amf.DLength)
	}
	if amf.NRows != 11 {
		t.Errorf("mf.NRows : Expecting 11 items, got instead: %d", amf.NRows)
	}
	if amf.NCols != 2 {
		t.Errorf("mf.NCols : Expecting 2 items, got instead: %d", amf.NCols)
	}
	if amf.Index[0] != 8 {
		t.Errorf("mf.Index[0] : Expecting 8 bytes, got instead: %d", amf.Index[0])
	}
	if amf.Index[1] != 56 {
		t.Errorf("mf.Index[1] : Expecting 56 bytes, got instead: %d", amf.Index[1])
	}
	err = amf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}

func TestLoadBinsrc(t *testing.T) {
	ctbytes := []uint8{4, 8}
	amf, err := MmapBinFile("../../c/test/data/test_data.binsrc", ctbytes)
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
	if amf.Fd < 0 {
		t.Errorf("can't open test_data.arrow for reading")
	}
	if amf.Size != 152 {
		t.Errorf("Size : Expecting 152 bytes, got instead: %d", amf.Size)
	}
	if amf.DOffset != 16 {
		t.Errorf("DOffset : Expecting 16 bytes, got instead: %d", amf.DOffset)
	}
	if amf.DLength != 136 {
		t.Errorf("DLength : Expecting 136 bytes, got instead: %d", amf.DLength)
	}
	if amf.NRows != 11 {
		t.Errorf("mf.NRows : Expecting 11 items, got instead: %d", amf.NRows)
	}
	if amf.NCols != 2 {
		t.Errorf("mf.NCols : Expecting 2 items, got instead: %d", amf.NCols)
	}
	if amf.Index[0] != 16 {
		t.Errorf("mf.Index[0] : Expecting 16 bytes, got instead: %d", amf.Index[0])
	}
	if amf.Index[1] != 64 {
		t.Errorf("mf.Index[1] : Expecting 64 bytes, got instead: %d", amf.Index[1])
	}
	err = amf.Close()
	if err != nil {
		t.Errorf("Unexpected error: %v", err)
	}
}
