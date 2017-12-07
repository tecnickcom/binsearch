"""Tests for libpybinsearch module."""


import libpybinsearch as bs
import os
import time
from unittest import TestCase

testData32 = [
    {4, 0, 99, 0x00002722, 0, 0},
    {4, 0, 99, 0x000033f5, 99, 99},
    {0, 0, 99, 0x00000001, 0, 99},
    {4, 0, 99, 0xf00027f3, 100, 100},
    {4, 0, 99, 0x000027f3, 13, 14},
    {4, 13, 99, 0x000027f3, 13, 14},
    {4, 14, 99, 0x000027f3, 14, 14},
    {4, 0, 0, 0x00000001, 1, 1},
    {4, 0, 0, 0xfffffff0, 1, 1},
    {4, 99, 99, 0x00000001, 100, 100},
    {4, 99, 99, 0xfffffff0, 100, 100},
]

testData64 = [
    {4, 0, 99, 0x000027225FB6E591, 0, 0},
    {4, 0, 99, 0x000033F522A78FD9, 99, 99},
    {0, 0, 99, 0x0000000100002722, 0, 0},
    {4, 0, 99, 0xf0000001f00027f3, 100, 100},
    {4, 0, 99, 0x000027F35FB6E591, 13, 13},
    {0, 13, 99, 0x00000001000027f3, 13, 14},
    {0, 14, 99, 0x00000001000027f3, 14, 14},
    {4, 0, 0, 0x0000000000000001, 1, 1},
    {4, 0, 0, 0xfffffffffffffff0, 1, 1},
    {4, 99, 99, 0x0000000000000001, 100, 100},
    {4, 99, 99, 0xfffffffffffffff0, 100, 100},
]


class TestFunctions(TestCase):

    @classmethod
    def setUpClass(cls):
        global src, fd, size
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/test_data.bin")
        src, fd, size = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    @classmethod
    def tearDownClass(cls):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        if h != 0:
            assert False, "Error while closing the memory-mapped file"

    def test_find_first_uint32be(self):
        for blkpos, first, last, search, foundFirst, foundLast in testData32:
            pos = bs.find_first_uint32be(src, 20, blkpos, first, last, search)
            self.assertEqual(pos, foundFirst)

    def test_find_last_uint32be(self):
        for blkpos, first, last, search, foundFirst, foundLast in testData32:
            pos = bs.find_last_uint32be(src, 20, blkpos, first, last, search)
            self.assertEqual(pos, foundLast)

    def test_find_first_uint64be(self):
        for blkpos, first, last, search, foundFirst, foundLast in testData64:
            pos = bs.find_first_uint64be(src, 20, blkpos, first, last, search)
            self.assertEqual(pos, foundFirst)

    def test_find_last_uint64be(self):
        for blkpos, first, last, search, foundFirst, foundLast in testData64:
            pos = bs.find_last_uint64be(src, 20, blkpos, first, last, search)
            self.assertEqual(pos, foundLast)


class TestBenchmark(object):

    global setup

    def setup():
        global src, fd, size
        if fd >= 0:
            pass
        bs.munmap_binfile(src, fd, size)
        inputfile = os.path.realpath(
            os.path.dirname(
                os.path.realpath(__file__)) +
            "/../../test/test_data.bin")
        src, fd, size = bs.mmap_binfile(inputfile)
        if fd < 0 or size != 2000:
            assert False, "Unable to open the file"

    def test_find_first_uint32be_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint32be,
            args=[
                src,
                20,
                4,
                0,
                99,
                0xf00027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint32be_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint32be,
            args=[
                src,
                20,
                4,
                0,
                99,
                0xf00027f3],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_first_uint64be_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_first_uint64be,
            args=[
                src,
                20,
                4,
                0,
                99,
                0x000027F35FB6E591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_find_last_uint64be_benchmark(self, benchmark):
        benchmark.pedantic(
            bs.find_last_uint64be,
            args=[
                src,
                20,
                4,
                0,
                99,
                0x000027F35FB6E591],
            setup=setup,
            iterations=1,
            rounds=10000)

    def test_tearDown(self):
        global src, fd, size
        h = bs.munmap_binfile(src, fd, size)
        fd = -1
        size = 0
        if h != 0:
            assert False, "Error while closing the memory-mapped file"
