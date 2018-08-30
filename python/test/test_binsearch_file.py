"""Tests for binsearch module - file formats."""


import binsearch as bs
import os
from unittest import TestCase


class TestFunctions(TestCase):

    def test_load_arrow(self):
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.arrow")
        src, fd, size, doffset, dlength = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 730:
            assert False, "Unable to open the file"
        self.assertEqual(doffset, 376)
        self.assertEqual(dlength, 136)
        nitems, index = bs.set_col_offset(doffset, dlength, [4, 8])
        self.assertEqual(nitems, 11)
        self.assertEqual(index[0], 376)
        self.assertEqual(index[1], 424)
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_load_feather(self):
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.feather")
        src, fd, size, doffset, dlength = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 384:
            assert False, "Unable to open the file"
        self.assertEqual(doffset, 8)
        self.assertEqual(dlength, 136)
        nitems, index = bs.set_col_offset(doffset, dlength, [4, 8])
        self.assertEqual(nitems, 11)
        self.assertEqual(index[0], 8)
        self.assertEqual(index[1], 56)
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"
