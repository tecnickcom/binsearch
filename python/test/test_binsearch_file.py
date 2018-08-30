"""Tests for binsearch module."""


import binsearch as bs
import os
from unittest import TestCase


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global src, fd, size, doffset, dlength
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../c/test/data/test_data.arrow")
        src, fd, size, doffset, dlength = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 730:
            assert False, "Unable to open the file"

    @classmethod
    def tearDownClass(cls):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_set_col_offset(self):
        self.assertEqual(doffset, 376)
        self.assertEqual(dlength, 136)
        nitems, index = bs.set_col_offset(doffset, dlength, [4, 8])
        self.assertEqual(nitems, 11)
        self.assertEqual(index[0], 376)
        self.assertEqual(index[1], 424)
