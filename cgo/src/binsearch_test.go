package binsearch

import "testing"
import "os"

var mf TMMFile

var testData32 = []struct {
	blkpos       uint64
	first        uint64
	last         uint64
	search       uint32
	foundFirst32 uint64
	foundLast32  uint64
}{
	{4, 0, 99, 0x00002722, 0, 0},
	{4, 0, 99, 0x000033f5, 99, 99},
	{0, 0, 99, 0x00000001, 0, 99},
	{4, 0, 99, 0xf00027f3, 100, 100},
	{4, 0, 99, 0x000027f3, 13, 14},
	{4, 13, 99, 0x000027f3, 13, 14},
	{4, 14, 99, 0x000027f3, 14, 14},
}

var testData64 = []struct {
	blkpos       uint64
	first        uint64
	last         uint64
	search       uint64
	foundFirst64 uint64
	foundLast64  uint64
}{
	{4, 0, 99, 0x000027225FB6E591, 0, 0},
	{4, 0, 99, 0x000033F522A78FD9, 99, 99},
	{0, 0, 99, 0x0000000100002722, 0, 0},
	{4, 0, 99, 0xf0000001f00027f3, 100, 100},
	{4, 0, 99, 0x000027F35FB6E591, 13, 13},
	{0, 13, 99, 0x00000001000027f3, 13, 14},
	{0, 14, 99, 0x00000001000027f3, 14, 14},
}

func TestMain(m *testing.M) {
	var err error

	// memory map the input file
	mf, err = MmapBinFile("../../test/test_data.bin")
	if err != nil {
		return
	}

	retCode := m.Run()

	// close the memory-mapped file
	err = mf.Close()
	if err != nil {
		retCode++
	}

	os.Exit(retCode)
}

func TestFindFirstUint32be(t *testing.T) {
	for i, tt := range testData32 {
		h := mf.FindFirstUint32be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundFirst32 {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.foundFirst32, h)
		}
	}
}

func BenchmarkFindFirstUint32be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint32be(20, 4, 0, 99, 0x000027f3)
	}
}

func TestFindLastUint32be(t *testing.T) {
	for i, tt := range testData32 {
		h := mf.FindLastUint32be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundLast32 {
			t.Errorf("%d. Expected %d, got %d", i, tt.foundLast32, h)
		}
	}
}

func BenchmarkFindLastUint32be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint32be(20, 4, 0, 99, 0x000027f3)
	}
}

func TestFindFirstUint64be(t *testing.T) {
	for i, tt := range testData64 {
		h := mf.FindFirstUint64be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundFirst64 {
			t.Errorf("%d. Expected %#016x, got %#016x", i, tt.foundFirst64, h)
		}
	}
}

func BenchmarkFindFirstUint64be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindFirstUint64be(20, 4, 0, 99, 0x000027F35FB6E591)
	}
}

func TestFindLastUint64be(t *testing.T) {
	for i, tt := range testData64 {
		h := mf.FindLastUint64be(20, tt.blkpos, tt.first, tt.last, tt.search)
		if h != tt.foundLast64 {
			t.Errorf("%d. Expected %d, got %d", i, tt.foundLast64, h)
		}
	}
}

func BenchmarkFindLastUint64be(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		mf.FindLastUint64be(20, 4, 0, 99, 0x000027F35FB6E591)
	}
}

func TestClose(t *testing.T) {
	lmf, err := MmapBinFile("../../test/test_data_b.bin")
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

func TestGetAddress(t *testing.T) {
	res := GetAddress(7, 11, 13)
	exp := uint64(102)
	if res != exp {
		t.Errorf("Expected %v, got %v", exp, res)
	}
}

func TestMmapBinFileError(t *testing.T) {
	_, err := MmapBinFile("error")
	if err == nil {
		t.Errorf("An error was expected")
	}
}
