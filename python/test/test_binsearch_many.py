"""Tests for binsearch module - find_range and find_many."""

import os
import tempfile
from unittest import TestCase

import binsearch as bs

# The column-mode fixture, the same one the column tests use.
COL_FILE = os.path.realpath(
    os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/test_data_col.bin"
)
COL_CTBYTES = [1, 2, 4, 8]
COL_ROWS = 251

# The row-mode fixture, the same one the row tests use.
ROW_FILE = os.path.realpath(
    os.path.dirname(os.path.realpath(__file__)) + "/../../c/test/data/test_data.bin"
)
ROW_CTBYTES = [12]
ROW_BLKLEN = 16
ROW_ROWS = 251

# Size in bytes of each searched type, and the width of its bit range.
TYPES = [8, 16, 32, 64]


def swap_columns(data, rows, offsets):
    """Returns a copy of the data with the bytes of every column value reversed."""
    out = bytearray(data)
    for col, tsize in enumerate(COL_CTBYTES):
        for row in range(rows):
            pos = offsets[col] + (row * tsize)
            out[pos : pos + tsize] = out[pos : pos + tsize][::-1]
    return bytes(out)


class Fixtures(TestCase):
    """The memory mapped fixtures shared by the tests below, in both byte
    orders: the files are little-endian, so a byte-swapped copy of the column
    one exercises the be functions on any host."""

    @classmethod
    def setUpClass(cls):
        cls.tmpdir = tempfile.TemporaryDirectory()
        (
            cls.colsrc,
            cls.colfd,
            cls.colsize,
            _,
            _,
            _,
            _,
            border,
            cls.colindex,
            _,
        ) = bs.mmap_binfile(COL_FILE, COL_CTBYTES)
        assert cls.colfd >= 0, "Unable to open the column fixture"
        assert border == 0, "The fixture must be little-endian"
        with open(COL_FILE, "rb") as f:
            data = f.read()
        befile = os.path.join(cls.tmpdir.name, "test_data_col_be.bin")
        with open(befile, "wb") as f:
            f.write(swap_columns(data, COL_ROWS, cls.colindex))
        (
            cls.becolsrc,
            cls.becolfd,
            cls.becolsize,
            _,
            _,
            _,
            _,
            _,
            cls.becolindex,
            _,
        ) = bs.mmap_binfile(befile, COL_CTBYTES)
        assert cls.becolfd >= 0, "Unable to open the swapped column fixture"
        (
            cls.rowsrc,
            cls.rowfd,
            cls.rowsize,
            cls.rowoffset,
            *_,
        ) = bs.mmap_binfile(ROW_FILE, ROW_CTBYTES)
        assert cls.rowfd >= 0, "Unable to open the row fixture"

    @classmethod
    def tearDownClass(cls):
        bs.munmap_binfile(cls.colsrc, cls.colfd, cls.colsize)
        bs.munmap_binfile(cls.becolsrc, cls.becolfd, cls.becolsize)
        bs.munmap_binfile(cls.rowsrc, cls.rowfd, cls.rowsize)
        cls.tmpdir.cleanup()

    def col_cases(self):
        """Yields one case per column, byte order and type."""
        for col, bits in enumerate(TYPES):
            yield "le", self.colsrc, self.colindex[col], bits
            yield "be", self.becolsrc, self.becolindex[col], bits


class TestFindRange(Fixtures):
    """find_range reports the half-open range that find_first and find_last
    delimit, in one call, for every layout, byte order and type."""

    def check_range(self, name, rng, first, last, search, **extra):
        """Checks a find_range result against the find_first and find_last it
        replaces, and returns the reported number of items."""
        find_first = getattr(bs, name.replace("find_range", "find_first"))
        find_last = getattr(bs, name.replace("find_range", "find_last"))
        hit, _, wfirst = find_first(first=first, last=last, search=search, **extra)
        if hit == last:
            wcount, wlast = 0, wfirst
        else:
            _, _, wlast = find_last(first=first, last=last, search=search, **extra)
            wcount = wlast - wfirst
        count, gfirst, glast = getattr(bs, name)(
            first=first, last=last, search=search, **extra
        )
        self.assertEqual(count, wcount, f"{name} search {search} count")
        if count > 0:
            self.assertEqual((gfirst, glast), (wfirst, wlast), f"{name} search {search}")
        return count

    def test_col_find_range(self):
        for order, src, offset, bits in self.col_cases():
            name = f"col_find_range_{order}_uint{bits}"
            with self.subTest(name=name):
                total = 0
                for search in range(0, 256, 7):
                    total += self.check_range(
                        name, None, 0, COL_ROWS, search, mfsrc=src, offset=offset
                    )
                self.assertGreater(total, 0, "no item was ever found")
                # Sub-ranges, including the empty one and the one that ends
                # where the data does.
                for first, last in ((0, 0), (5, 5), (3, 4), (0, 1), (100, 200), (0, COL_ROWS)):
                    for search in (0, 1, 0x70, 0xFF):
                        self.check_range(
                            name, None, first, last, search, mfsrc=src, offset=offset
                        )

    def test_col_find_range_sub(self):
        for order, src, offset, bits in self.col_cases():
            name = f"col_find_range_sub_{order}_uint{bits}"
            plain = f"col_find_range_{order}_uint{bits}"
            with self.subTest(name=name):
                # The whole value selects the same items as the plain search.
                for search in range(0, 256, 7):
                    want = getattr(bs, plain)(src, offset, 0, COL_ROWS, search)
                    got = getattr(bs, name)(
                        src, offset, 0, bits - 1, 0, COL_ROWS, search
                    )
                    self.assertEqual(got, want, f"{name} search {search}")
                for first, last in ((0, 0), (3, 4), (0, COL_ROWS)):
                    want = getattr(bs, plain)(src, offset, first, last, 0x70)
                    got = getattr(bs, name)(src, offset, 0, bits - 1, first, last, 0x70)
                    self.assertEqual(got, want)

    def test_find_range(self):
        for order in ("be", "le"):
            for bits in TYPES:
                name = f"find_range_{order}_uint{bits}"
                with self.subTest(name=name):
                    extra = {
                        "mfsrc": self.rowsrc,
                        "offset": self.rowoffset,
                        "blklen": ROW_BLKLEN,
                        "blkpos": 0,
                    }
                    for search in range(0, 256, 7):
                        self.check_range(name, None, 0, ROW_ROWS, search, **extra)
                    for first, last in ((0, 0), (3, 4), (0, 1), (0, ROW_ROWS)):
                        for search in (0, 0x70, 0xFF):
                            self.check_range(name, None, first, last, search, **extra)

    def test_find_range_sub(self):
        for order in ("be", "le"):
            for bits in TYPES:
                name = f"find_range_sub_{order}_uint{bits}"
                plain = f"find_range_{order}_uint{bits}"
                with self.subTest(name=name):
                    for search in range(0, 256, 7):
                        want = getattr(bs, plain)(
                            self.rowsrc, self.rowoffset, ROW_BLKLEN, 0, 0, ROW_ROWS, search
                        )
                        got = getattr(bs, name)(
                            self.rowsrc, self.rowoffset, ROW_BLKLEN, 0, 0, bits - 1, 0, ROW_ROWS, search
                        )
                        self.assertEqual(got, want, f"{name} search {search}")
                    for first, last in ((0, 0), (3, 4), (0, ROW_ROWS)):
                        want = getattr(bs, plain)(
                            self.rowsrc, self.rowoffset, ROW_BLKLEN, 0, first, last, 0x70
                        )
                        got = getattr(bs, name)(
                            self.rowsrc, self.rowoffset, ROW_BLKLEN, 0, 0, bits - 1, first, last, 0x70
                        )
                        self.assertEqual(got, want)


class TestFindMany(Fixtures):
    """find_many reports for every searched value what find_first reports for
    it, for any number of values from none to more than two batches."""

    # The number of searched values on each side of a batch boundary, which is
    # eight values wide.
    COUNTS = (0, 1, 7, 8, 9, 15, 16, 17, 20)

    def check_many(self, name, first, last, searches, **extra):
        want = [
            getattr(bs, name.replace("find_many", "find_first"))(
                first=first, last=last, search=s, **extra
            )[0]
            for s in searches
        ]
        got = getattr(bs, name)(first=first, last=last, search=searches, **extra)
        self.assertEqual(got, want, f"{name} over {len(searches)} values")

    def test_col_find_many(self):
        for order, src, offset, bits in self.col_cases():
            name = f"col_find_many_{order}_uint{bits}"
            with self.subTest(name=name):
                values = [(v * 13) % 256 for v in range(max(self.COUNTS))]
                for count in self.COUNTS:
                    self.check_many(
                        name, 0, COL_ROWS, values[:count], mfsrc=src, offset=offset
                    )
                for first, last in ((0, 0), (3, 4), (100, 200)):
                    self.check_many(
                        name, first, last, values[:9], mfsrc=src, offset=offset
                    )
                # A tuple is accepted as well as a list.
                self.check_many(
                    name, 0, COL_ROWS, tuple(values[:9]), mfsrc=src, offset=offset
                )

    def test_col_find_many_sub(self):
        for order, src, offset, bits in self.col_cases():
            name = f"col_find_many_sub_{order}_uint{bits}"
            plain = f"col_find_many_{order}_uint{bits}"
            with self.subTest(name=name):
                values = [(v * 13) % 256 for v in range(20)]
                for count in self.COUNTS:
                    want = getattr(bs, plain)(src, offset, 0, COL_ROWS, values[:count])
                    got = getattr(bs, name)(
                        src, offset, 0, bits - 1, 0, COL_ROWS, values[:count]
                    )
                    self.assertEqual(got, want)

    def test_find_many(self):
        for order in ("be", "le"):
            for bits in TYPES:
                name = f"find_many_{order}_uint{bits}"
                with self.subTest(name=name):
                    extra = {
                        "mfsrc": self.rowsrc,
                        "offset": self.rowoffset,
                        "blklen": ROW_BLKLEN,
                        "blkpos": 0,
                    }
                    values = [(v * 13) % 256 for v in range(max(self.COUNTS))]
                    for count in self.COUNTS:
                        self.check_many(name, 0, ROW_ROWS, values[:count], **extra)
                    for first, last in ((0, 0), (3, 4)):
                        self.check_many(name, first, last, values[:9], **extra)

    def test_find_many_sub(self):
        for order in ("be", "le"):
            for bits in TYPES:
                name = f"find_many_sub_{order}_uint{bits}"
                plain = f"find_many_{order}_uint{bits}"
                with self.subTest(name=name):
                    values = [(v * 13) % 256 for v in range(20)]
                    for count in self.COUNTS:
                        want = getattr(bs, plain)(
                            self.rowsrc, self.rowoffset, ROW_BLKLEN, 0, 0, ROW_ROWS, values[:count]
                        )
                        got = getattr(bs, name)(
                            self.rowsrc,
                            self.rowoffset,
                            ROW_BLKLEN,
                            0,
                            0,
                            bits - 1,
                            0,
                            ROW_ROWS,
                            values[:count],
                        )
                        self.assertEqual(got, want)

    def test_search_not_iterable(self):
        with self.assertRaises(TypeError):
            bs.col_find_many_le_uint32(self.colsrc, self.colindex[2], 0, COL_ROWS, 4)

    def test_search_item_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.col_find_many_le_uint32(
                self.colsrc, self.colindex[2], 0, COL_ROWS, [1, None, 3]
            )

    def test_search_item_out_of_range(self):
        with self.assertRaises(OverflowError):
            bs.col_find_many_le_uint8(
                self.colsrc, self.colindex[0], 0, COL_ROWS, [1, 256]
            )

    def test_search_item_negative(self):
        with self.assertRaises(OverflowError):
            bs.col_find_many_le_uint32(
                self.colsrc, self.colindex[2], 0, COL_ROWS, [1, -1]
            )

    def test_prefetch_modes(self):
        """Every mode reports what the default one reports."""
        values = [(v * 13) % 256 for v in range(20)]
        want = bs.col_find_many_le_uint32(
            self.colsrc, self.colindex[2], 0, COL_ROWS, values
        )
        for mode in (bs.PREFETCH_AUTO, bs.PREFETCH_ALWAYS, bs.PREFETCH_NEVER):
            with self.subTest(prefetch=mode):
                self.assertEqual(
                    bs.col_find_many_le_uint32(
                        self.colsrc, self.colindex[2], 0, COL_ROWS, values, mode
                    ),
                    want,
                )
                self.assertEqual(
                    bs.col_find_many_le_uint32(
                        self.colsrc,
                        self.colindex[2],
                        0,
                        COL_ROWS,
                        values,
                        prefetch=mode,
                    ),
                    want,
                )
        # The other three families take it too.
        self.assertEqual(
            len(
                bs.col_find_many_sub_le_uint32(
                    self.colsrc,
                    self.colindex[2],
                    0,
                    31,
                    0,
                    COL_ROWS,
                    values,
                    bs.PREFETCH_ALWAYS,
                )
            ),
            len(values),
        )
        self.assertEqual(
            len(
                bs.find_many_le_uint32(
                    self.rowsrc, 0, ROW_BLKLEN, 0, 0, ROW_ROWS, values, bs.PREFETCH_ALWAYS
                )
            ),
            len(values),
        )
        self.assertEqual(
            len(
                bs.find_many_sub_le_uint32(
                    self.rowsrc,
                    0,
                    ROW_BLKLEN,
                    0,
                    0,
                    31,
                    0,
                    ROW_ROWS,
                    values,
                    bs.PREFETCH_ALWAYS,
                )
            ),
            len(values),
        )

    def test_prefetch_out_of_range(self):
        with self.assertRaises(ValueError):
            bs.col_find_many_le_uint32(
                self.colsrc, self.colindex[2], 0, COL_ROWS, [1, 2], 3
            )

    def test_prefetch_out_of_range_every_function(self):
        """Every find_many function rejects a mode that is not one of the three."""
        bits_index = {8: 0, 16: 1, 32: 2, 64: 3}
        for order in ("be", "le"):
            for bits, ci in bits_index.items():
                cases = (
                    (
                        f"col_find_many_{order}_uint{bits}",
                        (self.colsrc, self.colindex[ci], 0, COL_ROWS, [1, 2]),
                    ),
                    (
                        f"col_find_many_sub_{order}_uint{bits}",
                        (
                            self.colsrc,
                            self.colindex[ci],
                            0,
                            bits - 1,
                            0,
                            COL_ROWS,
                            [1, 2],
                        ),
                    ),
                    (
                        f"find_many_{order}_uint{bits}",
                        (self.rowsrc, 0, ROW_BLKLEN, 0, 0, ROW_ROWS, [1, 2]),
                    ),
                    (
                        f"find_many_sub_{order}_uint{bits}",
                        (
                            self.rowsrc,
                            0,
                            ROW_BLKLEN,
                            0,
                            0,
                            bits - 1,
                            0,
                            ROW_ROWS,
                            [1, 2],
                        ),
                    ),
                )
                for name, args in cases:
                    with self.subTest(name=name), self.assertRaises(ValueError):
                        getattr(bs, name)(*args, 3)

    def test_prefetch_not_a_number(self):
        with self.assertRaises(TypeError):
            bs.col_find_many_le_uint32(
                self.colsrc, self.colindex[2], 0, COL_ROWS, [1, 2], None
            )


class TestCallingConvention(TestCase):
    """The search functions take their arguments by position or by name."""

    def setUp(self):
        self.src, self.fd, self.size, _, _, _, _, _, self.index, _ = bs.mmap_binfile(
            COL_FILE, COL_CTBYTES
        )

    def tearDown(self):
        bs.munmap_binfile(self.src, self.fd, self.size)

    def test_all_positional(self):
        self.assertEqual(
            bs.col_find_first_le_uint8(self.src, self.index[0], 0, COL_ROWS, 0x70),
            bs.col_find_first_le_uint8(
                mfsrc=self.src, offset=self.index[0], first=0, last=COL_ROWS, search=0x70
            ),
        )

    def test_mixed(self):
        self.assertEqual(
            bs.col_find_first_le_uint8(self.src, self.index[0], 0, COL_ROWS, 0x70),
            bs.col_find_first_le_uint8(
                self.src, self.index[0], 0, last=COL_ROWS, search=0x70
            ),
        )

    def test_too_many_arguments(self):
        with self.assertRaises(TypeError):
            bs.col_find_first_le_uint8(self.src, self.index[0], 0, COL_ROWS, 0x70, 1)

    def test_unknown_keyword(self):
        with self.assertRaises(TypeError):
            bs.col_find_first_le_uint8(
                self.src, self.index[0], 0, COL_ROWS, 0x70, nosuch=1
            )

    def test_duplicate_argument(self):
        with self.assertRaises(TypeError):
            bs.col_find_first_le_uint8(
                self.src, self.index[0], 0, COL_ROWS, 0x70, search=1
            )

    def test_missing_argument(self):
        with self.assertRaises(TypeError):
            bs.col_find_first_le_uint8(self.src, self.index[0], 0, COL_ROWS)
