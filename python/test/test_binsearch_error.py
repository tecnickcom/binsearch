"""Tests for binsearch module - argument validation and error paths."""

import gc
import os
from unittest import TestCase

import binsearch as bs

TEST_FILE = os.path.realpath(
    os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/test_data.bin"
)

TEST_FILE_SIZE = 4016

# Every search function of the module: the file mapping functions and
# get_address take different arguments and are tested separately.
SEARCH_FUNCTIONS = sorted(
    name
    for name in dir(bs)
    if name.startswith(("find_", "has_", "col_")) and callable(getattr(bs, name))
)

SUB_FUNCTIONS = [name for name in SEARCH_FUNCTIONS if "_sub_" in name]

# One function per searched type, used to check the argument converters.
TYPE_FUNCTIONS = {
    8: "find_first_be_uint8",
    16: "find_first_be_uint16",
    32: "find_first_be_uint32",
    64: "find_first_be_uint64",
}


def search_kwargs(name, mfsrc, **overrides):
    """Return a valid set of arguments for the given search function.

    Only the overrides that the function accepts are applied, so that the same
    override can be passed to functions that take "last" and to functions that
    take "pos".
    """
    kwargs = {"mfsrc": mfsrc, "offset": 0}
    if not name.startswith("col_"):
        kwargs["blklen"] = 16
        kwargs["blkpos"] = 0
    if "_sub_" in name:
        kwargs["bitstart"] = 0
        kwargs["bitend"] = 0
    if "has_next" in name:
        kwargs["pos"] = 0
        kwargs["last"] = 1
    elif "has_prev" in name:
        kwargs["first"] = 0
        kwargs["pos"] = 1
    else:
        kwargs["first"] = 0
        kwargs["last"] = 1
    # The find_many functions search a sequence of values rather than one.
    kwargs["search"] = [0] if "_many_" in name else 0
    kwargs.update({k: v for k, v in overrides.items() if k in kwargs})
    if ("_many_" in name) and not isinstance(kwargs["search"], list):
        kwargs["search"] = [kwargs["search"]]
    return kwargs


class TestMmap(TestCase):
    def test_mmap_binfile_missing_file(self):
        src, fd, size, doffset, dlength, nrows, ncols, border, index, _ = bs.mmap_binfile(
            "missing_file.bin", [4, 8]
        )
        self.assertEqual(fd, -1)
        self.assertEqual(size, 0)
        self.assertEqual(nrows, 0)
        self.assertEqual(index, [0, 0])
        self.assertIsNotNone(src)
        self.assertEqual(doffset, 0)
        self.assertEqual(dlength, 0)
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)

    def test_mmap_binfile_file_not_a_string(self):
        with self.assertRaises(TypeError):
            bs.mmap_binfile(12, [4])

    def test_munmap_binfile_fd_not_a_number(self):
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        with self.assertRaises(TypeError):
            bs.munmap_binfile(src, "12", size)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_munmap_binfile_size_not_a_number(self):
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        with self.assertRaises(TypeError):
            bs.munmap_binfile(src, fd, None)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_munmap_binfile_fd_mismatch(self):
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        with self.assertRaises(ValueError):
            bs.munmap_binfile(src, -1, size)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_get_address_blklen_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.get_address(None, 0, 0)

    def test_get_address_blkpos_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.get_address(0, None, 0)

    def test_get_address_item_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.get_address(0, 0, None)

    def test_mmap_binfile_ctbytes_not_iterable(self):
        with self.assertRaises(TypeError):
            bs.mmap_binfile(TEST_FILE, 4)

    def test_mmap_binfile_too_many_columns(self):
        with self.assertRaises(ValueError):
            bs.mmap_binfile(TEST_FILE, [1] * 256)

    def test_mmap_binfile_ctbytes_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.mmap_binfile(TEST_FILE, [4, None])

    def test_mmap_binfile_ctbytes_negative(self):
        with self.assertRaises(OverflowError):
            bs.mmap_binfile(TEST_FILE, [4, -8])

    def test_mmap_binfile_ctbytes_too_large(self):
        """A column type size that does not fit a byte is rejected instead of
        being silently truncated."""
        with self.assertRaises(OverflowError):
            bs.mmap_binfile(TEST_FILE, [4, 300])

    def test_mmap_binfile_ctbytes_float(self):
        """A column type size that is not an integer is rejected instead of
        being truncated to a silently wrong layout."""
        with self.assertRaises(TypeError):
            bs.mmap_binfile(TEST_FILE, [12.0])

    def test_mmap_binfile_ctbytes_string(self):
        """A column type size given as a string is rejected instead of being
        parsed as a number."""
        with self.assertRaises(TypeError):
            bs.mmap_binfile(TEST_FILE, ["12"])

    def test_get_address(self):
        self.assertEqual(bs.get_address(3, 5, 7), 26)

    def test_get_address_negative(self):
        with self.assertRaises(OverflowError):
            bs.get_address(3, 5, -7)

    def test_mmap_binfile_ctbytes_tuple(self):
        src, fd, size, doffset, dlength, nrows, ncols, border, index, _ = bs.mmap_binfile(
            TEST_FILE, (4, 8)
        )
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)
        self.assertEqual(size, TEST_FILE_SIZE)
        self.assertEqual(doffset, 0)
        self.assertEqual(dlength, TEST_FILE_SIZE)
        self.assertEqual(nrows, 334)
        self.assertEqual(index, [0, 1336])
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_mmap_binfile_ctbytes_iterator(self):
        """A generator is converted to a temporary list that is released."""
        src, fd, size, doffset, dlength, nrows, ncols, border, index, _ = bs.mmap_binfile(
            TEST_FILE, (v for v in [4, 8])
        )
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)
        self.assertEqual(nrows, 334)
        self.assertEqual(index, [0, 1336])
        self.assertEqual(size, TEST_FILE_SIZE)
        self.assertEqual(doffset, 0)
        self.assertEqual(dlength, TEST_FILE_SIZE)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_mmap_binfile_iterator_too_many_columns(self):
        with self.assertRaises(ValueError):
            bs.mmap_binfile(TEST_FILE, (1 for _ in range(256)))

    def test_mmap_binfile_iterator_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.mmap_binfile(TEST_FILE, (v for v in [4, None]))

    def test_mmap_binfile_iterator_negative(self):
        with self.assertRaises(OverflowError):
            bs.mmap_binfile(TEST_FILE, (v for v in [4, -8]))

    def test_mmap_binfile_ctbytes_empty_tuple(self):
        src, fd, size, doffset, dlength, nrows, ncols, border, index, _ = bs.mmap_binfile(
            TEST_FILE, ()
        )
        self.assertEqual(ncols, 0)
        self.assertEqual(border, 0)
        self.assertEqual(nrows, 0)
        self.assertEqual(index, [])
        self.assertEqual(size, TEST_FILE_SIZE)
        self.assertEqual(doffset, 0)
        self.assertEqual(dlength, TEST_FILE_SIZE)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_mmap_binfile_ctbytes_list_subclass(self):
        class ColumnList(list):
            pass

        src, fd, size, doffset, dlength, nrows, ncols, border, index, _ = bs.mmap_binfile(
            TEST_FILE, ColumnList([4, 8])
        )
        self.assertEqual(ncols, 2)
        self.assertEqual(border, 0)
        self.assertEqual(nrows, 334)
        self.assertEqual(index, [0, 1336])
        self.assertEqual(size, TEST_FILE_SIZE)
        self.assertEqual(doffset, 0)
        self.assertEqual(dlength, TEST_FILE_SIZE)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_munmap_binfile_not_a_capsule(self):
        with self.assertRaises(ValueError):
            bs.munmap_binfile(None, 3, 10)

    def test_munmap_binfile_wrong_size(self):
        """A size that is not the mapped one is rejected and leaves the mapping
        usable, instead of releasing an arbitrary range of memory."""
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        with self.assertRaises(ValueError):
            bs.munmap_binfile(src, fd, 0)
        self.assertEqual(bs.find_first_be_uint8(src, 0, 16, 0, 0, 251, 0x00)[0], 0)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_munmap_binfile_wrong_fd(self):
        """A descriptor that is not the mapped one is rejected and leaves the
        mapping usable, instead of closing an unrelated descriptor."""
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        victim = os.open(TEST_FILE, os.O_RDONLY)
        try:
            with self.assertRaises(ValueError):
                bs.munmap_binfile(src, victim, size)
            self.assertEqual(bs.find_first_be_uint8(src, 0, 16, 0, 0, 251, 0x00)[0], 0)
            os.read(victim, 1)  # the descriptor was not closed
        finally:
            os.close(victim)
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)

    def test_munmap_binfile_twice(self):
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)
        with self.assertRaises(ValueError):
            bs.munmap_binfile(src, fd, size)

    def test_dropped_mapping_is_released(self):
        """A mapping that is dropped without being unmapped is released by the
        capsule destructor instead of leaking its descriptor and its map."""
        before = len(os.listdir("/proc/self/fd"))
        for _ in range(32):
            bs.mmap_binfile(TEST_FILE, [12])
        gc.collect()
        self.assertLessEqual(len(os.listdir("/proc/self/fd")), before + 1)

    def test_dropped_mapping_after_unmap(self):
        """The capsule destructor of an already unmapped file does nothing."""
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)
        del src
        gc.collect()

    def test_missing_arguments(self):
        """Every module function rejects a call without arguments."""
        names = [
            name
            for name in dir(bs)
            if not name.startswith("_") and callable(getattr(bs, name))
        ]
        self.assertGreater(len(names), 90)
        for name in names:
            with self.subTest(name=name), self.assertRaises(TypeError):
                getattr(bs, name)()


class TestSearchArguments(TestCase):
    """Every search function checks its arguments against the memory map."""

    def setUp(self):
        self.src, self.fd, self.size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        self.assertEqual(self.size, TEST_FILE_SIZE)

    def tearDown(self):
        bs.munmap_binfile(self.src, self.fd, self.size)

    def test_function_list(self):
        self.assertEqual(len(SEARCH_FUNCTIONS), 192)
        self.assertEqual(len(SUB_FUNCTIONS), 96)

    def test_argument_not_a_number(self):
        """Every numeric argument of every search function is converted, so a
        value that is not a number is refused rather than silently accepted."""
        for name in SEARCH_FUNCTIONS:
            kwargs = search_kwargs(name, self.src)
            for key in kwargs:
                if key == "mfsrc":
                    continue
                with self.subTest(name=name, argument=key), self.assertRaises(
                    TypeError
                ):
                    getattr(bs, name)(**{**kwargs, key: None})

    def test_searched_value_not_a_number(self):
        """The find_many functions convert every value of the sequence."""
        for name in SEARCH_FUNCTIONS:
            if "_many_" not in name:
                continue
            kwargs = search_kwargs(name, self.src)
            with self.subTest(name=name), self.assertRaises(TypeError):
                getattr(bs, name)(**{**kwargs, "search": [None]})

    def test_not_a_capsule(self):
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(ValueError):
                getattr(bs, name)(**search_kwargs(name, None))

    def test_unmapped_file(self):
        src, fd, size, *_ = bs.mmap_binfile(TEST_FILE, [12])
        self.assertEqual(bs.munmap_binfile(src, fd, size), 0)
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(ValueError):
                getattr(bs, name)(**search_kwargs(name, src))

    def test_offset_outside_file(self):
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(ValueError):
                getattr(bs, name)(**search_kwargs(name, self.src, offset=self.size + 8))

    def test_range_outside_file(self):
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(ValueError):
                getattr(bs, name)(
                    **search_kwargs(name, self.src, last=2**40, pos=2**40)
                )

    def test_empty_range(self):
        """An empty range reads nothing, so it is accepted at the end of the file."""
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name):
                getattr(bs, name)(
                    **search_kwargs(name, self.src, offset=self.size, last=0, pos=0)
                )

    def test_no_room_for_one_item(self):
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(ValueError):
                getattr(bs, name)(**search_kwargs(name, self.src, offset=self.size))

    def test_block_position_outside_file(self):
        name = "find_first_be_uint8"
        with self.assertRaises(ValueError):
            getattr(bs, name)(**search_kwargs(name, self.src, blkpos=2**40))

    def test_null_block_length(self):
        """A block length of zero puts every item at the same address."""
        result, first, last = bs.find_first_be_uint8(self.src, 0, 0, 0, 0, 251, 0x00)
        self.assertEqual(result, 0)
        self.assertEqual(first, 0)
        self.assertEqual(last, 0)

    def test_column_offset_not_aligned(self):
        for name in SEARCH_FUNCTIONS:
            if name.startswith("col_") and not name.endswith("uint8"):
                with self.subTest(name=name), self.assertRaises(ValueError):
                    getattr(bs, name)(**search_kwargs(name, self.src, offset=1))

    def test_bit_range_outside_type(self):
        for name in SUB_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(ValueError):
                getattr(bs, name)(**search_kwargs(name, self.src, bitend=200))

    def test_bit_range_reversed(self):
        name = "find_first_sub_be_uint64"
        with self.assertRaises(ValueError):
            getattr(bs, name)(**search_kwargs(name, self.src, bitstart=5, bitend=1))

    def test_negative_value(self):
        for name in SEARCH_FUNCTIONS:
            with self.subTest(name=name), self.assertRaises(OverflowError):
                getattr(bs, name)(**search_kwargs(name, self.src, search=-1))

    def test_value_too_large_for_the_type(self):
        for bits, name in TYPE_FUNCTIONS.items():
            if bits == 64:
                continue
            with self.subTest(name=name), self.assertRaises(OverflowError):
                getattr(bs, name)(**search_kwargs(name, self.src, search=(2**bits)))

    def test_value_too_large_for_uint64(self):
        name = TYPE_FUNCTIONS[64]
        with self.assertRaises(OverflowError):
            getattr(bs, name)(**search_kwargs(name, self.src, search=(2**64)))

    def test_value_not_a_number(self):
        name = TYPE_FUNCTIONS[32]
        with self.assertRaises(TypeError):
            getattr(bs, name)(**search_kwargs(name, self.src, search="1"))
