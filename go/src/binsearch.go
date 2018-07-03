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

// BytesToUint8 convert 1 byte in big-endian format to uint8.
func (mf TMMFile) BytesToUint8(i int) uint8 {
	return uint8(mf.r.At(i))
}

// BytesToUint16 convert 2 bytes in big-endian format to uint16.
func (mf TMMFile) BytesToUint16(i int) uint16 {
	return ((uint16(mf.r.At(i)) << 8) | uint16(mf.r.At(i+1)))
}

// BytesToUint32 convert 4 bytes in big-endian format to uint32.
func (mf TMMFile) BytesToUint32(i int) uint32 {
	return ((uint32(mf.r.At(i)) << 24) | (uint32(mf.r.At(i+1)) << 16) | (uint32(mf.r.At(i+2)) << 8) | uint32(mf.r.At(i+3)))
}

// BytesToUint64 convert 8 bytes in big-endian format to uint64.
func (mf TMMFile) BytesToUint64(i int) uint64 {
	return ((uint64(mf.r.At(i)) << 56) | (uint64(mf.r.At(i+1)) << 48) | (uint64(mf.r.At(i+2)) << 40) | (uint64(mf.r.At(i+3)) << 32) | (uint64(mf.r.At(i+4)) << 24) | (uint64(mf.r.At(i+5)) << 16) | (uint64(mf.r.At(i+6)) << 8) | uint64(mf.r.At(i+7)))
}

// FindFirstUint8 search for the first occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint8(blklen, blkpos uint64, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint8(int(i))
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

// FindLastUint8 search for the last occurrence of a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint8(blklen, blkpos uint64, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint8(int(i))
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

// FindFirstUint16 search for the first occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint16(blklen, blkpos uint64, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint16(int(i))
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

// FindLastUint16 search for the last occurrence of a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint16(blklen, blkpos uint64, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint16(int(i))
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

// FindFirstUint32 search for the first occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint32(blklen, blkpos uint64, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint32(int(i))
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

// FindLastUint32 search for the last occurrence of a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint32(blklen, blkpos uint64, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint32(int(i))
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

// FindFirstUint64 search for the first occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstUint64(blklen, blkpos uint64, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint64(int(i))
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

// FindLastUint64 search for the last occurrence of a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastUint64(blklen, blkpos uint64, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = mf.BytesToUint64(int(i))
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

// FindFirstSubUint8 search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	rshift := (7 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint8(int(i)) << bitstart) >> rshift
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

// FindLastSubUint8 search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 8 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint8) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint8
	rshift := (7 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint8(int(i)) << bitstart) >> rshift
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

// FindFirstSubUint16 search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	rshift := (15 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint16(int(i)) << bitstart) >> rshift
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

// FindLastSubUint16 search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 16 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint16) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint16
	rshift := (15 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint16(int(i)) << bitstart) >> rshift
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

// FindFirstSubUint32 search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	rshift := (31 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint32(int(i)) << bitstart) >> rshift
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

// FindLastSubUint32 search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 32 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint32) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint32
	rshift := (31 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint32(int(i)) << bitstart) >> rshift
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

// FindFirstSubUint64 search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindFirstSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	rshift := (63 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint64(int(i)) << bitstart) >> rshift
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

// FindLastSubUint64 search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data.
// The 64 bit values in the file must encoded in big-endian format and sorted in ascending order.
// Return the item number if found or (last + 1) if not found, plus the first and last positions.
func (mf TMMFile) FindLastSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, last uint64, search uint64) (uint64, uint64, uint64) {
	var i, middle uint64
	var x uint64
	rshift := (63 - bitend + bitstart)
	found := (last + 1)
	for first <= last {
		middle = first + ((last - first) >> 1)
		i = GetAddress(blklen, blkpos, middle)
		x = (mf.BytesToUint64(int(i)) << bitstart) >> rshift
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

// HasNextUint8 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint8(blklen, blkpos, pos, last uint64, search uint8) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToUint8(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextUint16 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint16(blklen, blkpos, pos, last uint64, search uint16) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToUint16(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextUint32 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint32(blklen, blkpos, pos, last uint64, search uint32) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToUint32(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextUint64 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextUint64(blklen, blkpos, pos, last uint64, search uint64) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	return (search == mf.BytesToUint64(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasNextSubUint8 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint8 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint8 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint8) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	rshift := ((1 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint8(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasNextSubUint16 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint16 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint16 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint16) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	rshift := ((2 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint16(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasNextSubUint32 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint32 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint32 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint32) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	rshift := ((4 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint32(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasNextSubUint64 checks if the next occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindFirstUint64 to get the next elements that still satisfy the search.
// The item returned by FindFirstUint64 should be set as the "pos" parameter in this function.
// Return true if the next item is valid, false otherwise, plus the position.
func (mf TMMFile) HasNextSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, pos, last uint64, search uint64) (bool, uint64) {
	if pos >= last {
		return false, pos
	}
	pos++
	rshift := ((8 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint64(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasPrevUint8 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint8(blklen, blkpos, first, pos uint64, search uint8) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToUint8(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevUint16 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint16(blklen, blkpos, first, pos uint64, search uint16) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToUint16(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevUint32 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint32(blklen, blkpos, first, pos uint64, search uint32) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToUint32(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevUint64 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevUint64(blklen, blkpos, first, pos uint64, search uint64) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	return (search == mf.BytesToUint64(int(GetAddress(blklen, blkpos, pos)))), pos
}

// HasPrevSubUint8 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint8 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint8 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint8(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint8) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	rshift := ((1 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint8(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasPrevSubUint16 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint16 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint16 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint16(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint16) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	rshift := ((2 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint16(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasPrevSubUint32 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint32 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint32 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint32(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint32) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	rshift := ((4 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint32(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}

// HasPrevSubUint64 checks if the previous occurrence of an unsigned integer on a memory mapped
// binary file containing adjacent blocks of sorted binary data still matches the search value.
// The values in the file must encoded in big-endian format and sorted in ascending order.
// This function can be used after FindLastUint64 to get the previous elements that still satisfy the search.
// The item returned by FindLastUint64 should be set as the "pos" parameter in this function.
// Return true if the previous item is valid, false otherwise, plus the position.
func (mf TMMFile) HasPrevSubUint64(blklen, blkpos uint64, bitstart, bitend uint8, first, pos uint64, search uint64) (bool, uint64) {
	if pos <= first {
		return false, pos
	}
	pos--
	rshift := ((8 * 8) - 1 - bitend + bitstart)
	return (search == ((mf.BytesToUint64(int(GetAddress(blklen, blkpos, pos))) << bitstart) >> rshift)), pos
}
