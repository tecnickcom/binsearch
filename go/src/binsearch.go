package binsearch

// Copyright (c) 2017 Nicola Asuni - Tecnick.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

import "golang.org/x/exp/mmap"

// TMMFile contains the memory mapped file reader
type TMMFile struct {
	r *mmap.ReaderAt
}

// MmapBinFile maps the specified file in memory.
func MmapBinFile(file string) (TMMFile, error) {
	mf, err := mmap.Open(file)
	return TMMFile{r: mf}, err
}

// Close unmap and close the memory-mapped file.
func (mf TMMFile) Close() error {
	if mf.r != nil {
		return mf.r.Close()
	}
	return nil
}

// GetAddress returns the absolute file start position of the specified item (binary block).
func GetAddress(blklen, blkpos, item uint64) uint64 {
	return ((blklen * item) + blkpos)
}

// BytesToBEUint8 convert 1 byte in BE format to uint8.
func (mf TMMFile) BytesToBEUint8(i int) uint8 {
	return uint8(mf.r.At(i))
}

// BytesToBEUint16 convert 2 bytes in BE format to uint16.
func (mf TMMFile) BytesToBEUint16(i int) uint16 {
	return ((uint16(mf.r.At(i)) << 8) | uint16(mf.r.At(i+1)))
}

// BytesToBEUint32 convert 4 bytes in BE format to uint32.
func (mf TMMFile) BytesToBEUint32(i int) uint32 {
	return ((uint32(mf.r.At(i)) << 24) | (uint32(mf.r.At(i+1)) << 16) | (uint32(mf.r.At(i+2)) << 8) | uint32(mf.r.At(i+3)))
}

// BytesToBEUint64 convert 8 bytes in BE format to uint64.
func (mf TMMFile) BytesToBEUint64(i int) uint64 {
	return ((uint64(mf.r.At(i)) << 56) | (uint64(mf.r.At(i+1)) << 48) | (uint64(mf.r.At(i+2)) << 40) | (uint64(mf.r.At(i+3)) << 32) | (uint64(mf.r.At(i+4)) << 24) | (uint64(mf.r.At(i+5)) << 16) | (uint64(mf.r.At(i+6)) << 8) | uint64(mf.r.At(i+7)))
}

// BytesToLEUint8 convert 1 byte in LE format to uint8.
func (mf TMMFile) BytesToLEUint8(i int) uint8 {
	return uint8(mf.r.At(i))
}

// BytesToLEUint16 convert 2 bytes in LE format to uint16.
func (mf TMMFile) BytesToLEUint16(i int) uint16 {
	return ((uint16(mf.r.At(i+1)) << 8) | uint16(mf.r.At(i)))
}

// BytesToLEUint32 convert 4 bytes in LE format to uint32.
func (mf TMMFile) BytesToLEUint32(i int) uint32 {
	return ((uint32(mf.r.At(i+3)) << 24) | (uint32(mf.r.At(i+2)) << 16) | (uint32(mf.r.At(i+1)) << 8) | uint32(mf.r.At(i)))
}

// BytesToLEUint64 convert 8 bytes in LE format to uint64.
func (mf TMMFile) BytesToLEUint64(i int) uint64 {
	return ((uint64(mf.r.At(i+7)) << 56) | (uint64(mf.r.At(i+6)) << 48) | (uint64(mf.r.At(i+5)) << 40) | (uint64(mf.r.At(i+4)) << 32) | (uint64(mf.r.At(i+3)) << 24) | (uint64(mf.r.At(i+2)) << 16) | (uint64(mf.r.At(i+1)) << 8) | uint64(mf.r.At(i)))
}

// FindFirstBEUint8 search for the first occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint8(blklen, blkpos uint64, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint8(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastBEUint8 search for the last occurrence of a BE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint8(blklen, blkpos uint64, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint8(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstBEUint16 search for the first occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint16(blklen, blkpos uint64, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint16(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastBEUint16 search for the last occurrence of a BE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint16(blklen, blkpos uint64, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint16(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstBEUint32 search for the first occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint32(blklen, blkpos uint64, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint32(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastBEUint32 search for the last occurrence of a BE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint32(blklen, blkpos uint64, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint32(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstBEUint64 search for the first occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstBEUint64(blklen, blkpos uint64, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint64(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastBEUint64 search for the last occurrence of a BE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastBEUint64(blklen, blkpos uint64, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToBEUint64(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubBEUint8 search for the first occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint8(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubBEUint8 search for the last occurrence of a BE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint8(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubBEUint16 search for the first occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint16(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubBEUint16 search for the last occurrence of a BE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint16(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubBEUint32 search for the first occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint32(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubBEUint32 search for the last occurrence of a BE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint32(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubBEUint64 search for the first occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint64(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubBEUint64 search for the last occurrence of a BE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in BE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToBEUint64(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// HasNextBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToBEUint8(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToBEUint16(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToBEUint32(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextBEUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToBEUint64(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextSubBEUint8 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint8(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasNextSubBEUint16 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint16(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasNextSubBEUint32 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint32(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasNextSubBEUint64 checks if the next occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindFirstBEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstBEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint64(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToBEUint8(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToBEUint16(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToBEUint32(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevBEUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToBEUint64(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevSubBEUint8 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint8(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevSubBEUint16 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint16(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevSubBEUint32 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint32(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevSubBEUint64 checks if the previous occurrence of a BE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in BE format and sorted in ascending order.
// This function can be used after FindLastBEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastBEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubBEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToBEUint64(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// FindFirstLEUint8 search for the first occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint8(blklen, blkpos uint64, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint8(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastLEUint8 search for the last occurrence of a LE 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint8(blklen, blkpos uint64, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint8(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstLEUint16 search for the first occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint16(blklen, blkpos uint64, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint16(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastLEUint16 search for the last occurrence of a LE 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint16(blklen, blkpos uint64, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint16(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstLEUint32 search for the first occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint32(blklen, blkpos uint64, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint32(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastLEUint32 search for the last occurrence of a LE 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint32(blklen, blkpos uint64, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint32(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstLEUint64 search for the first occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstLEUint64(blklen, blkpos uint64, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint64(int(i))
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastLEUint64 search for the last occurrence of a LE 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastLEUint64(blklen, blkpos uint64, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToLEUint64(int(i))
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubLEUint8 search for the first occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint8(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubLEUint8 search for the last occurrence of a LE bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint8(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubLEUint16 search for the first occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint16(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubLEUint16 search for the last occurrence of a LE bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint16(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubLEUint32 search for the first occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint32(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubLEUint32 search for the last occurrence of a LE bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint32(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindFirstSubLEUint64 search for the first occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint64(int(i)) & bitmask) >> rshift
		if x == search {
			if middle == 0 {
				return middle, first, last
			}
			found = middle
			last = (middle - 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// FindLastSubLEUint64 search for the last occurrence of a LE bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in LE format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToLEUint64(int(i)) & bitmask) >> rshift
		if x == search {
			found = middle
			first = (middle + 1)
		} else {
			if x < search {
				first = (middle + 1)
			} else {
				if middle > 0 {
					last = (middle - 1)
				} else {
					return found, first, last
				}
			}
		}
	}
	return found, first, last
}

// HasNextLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToLEUint8(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToLEUint16(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToLEUint32(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextLEUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToLEUint64(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextSubLEUint8 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint8(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasNextSubLEUint16 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint16(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasNextSubLEUint32 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint32(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasNextSubLEUint64 checks if the next occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindFirstLEUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstLEUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint64(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToLEUint8(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToLEUint16(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToLEUint32(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevLEUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToLEUint64(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevSubLEUint8 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(1*8) - 1
	bitmask := uint8(uint8(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint8(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevSubLEUint16 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(2*8) - 1
	bitmask := uint16(uint16(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint16(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevSubLEUint32 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(4*8) - 1
	bitmask := uint32(uint32(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint32(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}

// HasPrevSubLEUint64 checks if the previous occurrence of a LE unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in LE format and sorted in ascending order.
// This function can be used after FindLastLEUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastLEUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubLEUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	mbit := uint8(8*8) - 1
	bitmask := uint64(uint64(1) << (mbit - bitstart))
	bitmask ^= (bitmask - 1)
	rshift := (mbit - bitend)
	return (search == ((mf.BytesToLEUint64(int(GetAddress(blklen, blkpos, pos))) & bitmask) >> rshift)), pos
}
