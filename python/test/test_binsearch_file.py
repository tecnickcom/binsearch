"""Tests for binsearch module - file formats."""

import os
import tempfile
from unittest import TestCase

import binsearch as bs


class TestFunctions(TestCase):
    def test_load_padded_columns(self):
        """Every column but the last is padded to an 8-byte boundary, so the
        number of rows must be derived from the padded layout: 100 rows of two
        uint8 columns occupy 100 + 4 padding + 100 bytes."""
        with tempfile.TemporaryDirectory() as tmpdir:
            inputfile = os.path.join(tmpdir, "padded.bin")
            with open(inputfile, "wb") as f:
                f.write(bytes(range(100)))
                f.write(bytes(4))
                f.write(bytes(range(100)))
            src, fd, size, doffset, dlength, nrows, ncols, border, index, _ = (
                bs.mmap_binfile(inputfile, [1, 1])
            )
            assert fd >= 0 and size == 204, "Unable to open the file"
            self.assertEqual(doffset, 0)
            self.assertEqual(dlength, 204)
            self.assertEqual(nrows, 100)
            self.assertEqual(ncols, 2)
            self.assertEqual(border, 0)
            self.assertEqual(index[0], 0)
            self.assertEqual(index[1], 104)
            item, _, _ = bs.col_find_first_le_uint8(src, index[1], 0, nrows, 42)
            self.assertEqual(item, 42)
            h = bs.munmap_binfile(src, fd, size)
            assert h == 0, "Error while closing the memory-mapped file"

    def test_load_arrow(self):
        inputfile = os.path.realpath(
            os.path.dirname(os.path.realpath(__file__))
            + "/../../c/test/data/test_data_arrow.bin"
        )
        src, fd, size, doffset, dlength, nrows, ncols, border, index, ctbytes = bs.mmap_binfile(
            inputfile, [4, 8]
        )
        assert fd >= 0 and size == 730, "Unable to open the file"
        self.assertEqual(doffset, 376)
        self.assertEqual(dlength, 136)
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)
        self.assertEqual(index[0], 376)
        self.assertEqual(index[1], 424)
        self.assertEqual(ctbytes, [4, 8])
        h = bs.munmap_binfile(src, fd, size)
        assert h == 0, "Error while closing the memory-mapped file"

    def test_load_feather(self):
        inputfile = os.path.realpath(
            os.path.dirname(os.path.realpath(__file__))
            + "/../../c/test/data/test_data_feather.bin"
        )
        src, fd, size, doffset, dlength, nrows, ncols, border, index, ctbytes = bs.mmap_binfile(
            inputfile, [4, 8]
        )
        assert fd >= 0 and size == 384, "Unable to open the file"
        self.assertEqual(doffset, 8)
        self.assertEqual(dlength, 136)
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)
        self.assertEqual(index[0], 8)
        self.assertEqual(index[1], 56)
        self.assertEqual(ctbytes, [4, 8])
        h = bs.munmap_binfile(src, fd, size)
        assert h == 0, "Error while closing the memory-mapped file"

    def test_load_binsrc(self):
        inputfile = os.path.realpath(
            os.path.dirname(os.path.realpath(__file__))
            + "/../../c/test/data/test_data_binsrc.bin"
        )
        src, fd, size, doffset, dlength, nrows, ncols, border, index, ctbytes = bs.mmap_binfile(
            inputfile, [4, 8]
        )
        assert fd >= 0 and size == 176, "Unable to open the file"
        self.assertEqual(doffset, 40)
        self.assertEqual(dlength, 136)
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)
        self.assertEqual(index[0], 40)
        self.assertEqual(index[1], 88)
        self.assertEqual(ctbytes, [4, 8])
        h = bs.munmap_binfile(src, fd, size)
        assert h == 0, "Error while closing the memory-mapped file"

    def test_load_binsrc_without_ctbytes(self):
        inputfile = os.path.realpath(
            os.path.dirname(os.path.realpath(__file__))
            + "/../../c/test/data/test_data_binsrc.bin"
        )
        # The header carries the number of columns and their type sizes, so the
        # ones given here are ignored and the real ones are reported back.
        src, fd, size, _, _, nrows, ncols, _, index, ctbytes = bs.mmap_binfile(
            inputfile, []
        )
        assert fd >= 0 and size == 176, "Unable to open the file"
        self.assertEqual(nrows, 11)
        self.assertEqual(ncols, 2)
        self.assertEqual(ctbytes, [4, 8])
        self.assertEqual(index, [40, 88])
        h = bs.munmap_binfile(src, fd, size)
        assert h == 0, "Error while closing the memory-mapped file"
