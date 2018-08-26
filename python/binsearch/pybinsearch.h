// Python binsearch Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@tecnick.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/binsearch

#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *py_mmap_binfile(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_get_address(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_first_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_last_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_first_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_last_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_next_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_next_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_prev_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_prev_sub_be_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_be_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_first_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_last_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_first_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_first_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_find_last_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_find_last_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_next_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_next_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_next_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_prev_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_has_prev_sub_le_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_has_prev_sub_le_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_find_first_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_find_last_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_find_first_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_first_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_find_last_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_find_last_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_has_next_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_has_next_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_next_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_has_prev_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_uint64(PyObject *self, PyObject *args, PyObject *keywds);

static PyObject *py_col_has_prev_sub_uint8(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint16(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint32(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject *py_col_has_prev_sub_uint64(PyObject *self, PyObject *args, PyObject *keywds);

PyMODINIT_FUNC initbinsearch(void);

// ----------

#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"file : str\n"\
"    Path to the file to map.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Pointer to the memory map.\n"\
"    - File descriptor.\n"\
"    - File size in bytes.\n"\
"    - Index of the last element (if set as last 4 bytes) or it can be used as index size."

#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"fd : int\n"\
"    File descriptor.\n"\
"size : int\n"\
"    File size.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    On success returns 0, on failure -1."

#define PYGETADDRESS_DOCSTRING "Returns the absolute file address position of the specified item (binary block).\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"item : int\n"\
"    Item number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"int:\n"\
"    First byte position of the specified item number."

// ----------

#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYHASNEXTUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYHASNEXTSUBUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTSUBUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTSUBUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYHASNEXTSUBUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYHASPREVUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

#define PYHASPREVSUBUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVSUBUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVSUBUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYHASPREVSUBUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"blklen : int\n"\
"    Length of the binary block in bytes.\n"\
"blkpos : int\n"\
"    Indicates the position of the value to search inside a binary block.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

// ----------

#define PYCOLFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLFINDFIRSTSUBUINT8_DOCSTRING "Search for the first occurrence of a bit set contained in a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTSUBUINT16_DOCSTRING "Search for the first occurrence of a bit set contained in a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTSUBUINT32_DOCSTRING "Search for the first occurrence of a bit set contained in a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDFIRSTSUBUINT64_DOCSTRING "Search for the first occurrence of a bit set contained in a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLFINDLASTSUBUINT8_DOCSTRING "Search for the last occurrence of a bit set contained in a 8 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 8 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 7).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTSUBUINT16_DOCSTRING "Search for the last occurrence of a bit set contained in a 16 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 16 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 15).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTSUBUINT32_DOCSTRING "Search for the last occurrence of a bit set contained in a 32 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 32 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 31).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

#define PYCOLFINDLASTSUBUINT64_DOCSTRING "Search for the last occurrence of a bit set contained in a 64 bit unsigned integer on a memory buffer containing contiguos blocks of sorted binary data. The values must be encoded in Little-Endian format and sorted in ascending order. The 64 bit values in the file must be sorted in ascending order.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - Item number if found or (last + 1) if not found.\n"\
"    - Position of the 'first' iterator.\n"\
"    - Position of the 'last' iterator."

// ----------

#define PYCOLHASNEXTUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYCOLHASNEXTSUBUINT8_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTSUBUINT16_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTSUBUINT32_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

#define PYCOLHASNEXTSUBUINT64_DOCSTRING "Check if the next occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"pos : int\n"\
"    Current item position.\n"\
"last : int\n"\
"    Last element of the range to search (max value = nitems - 1).\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Next position."

// ----------

#define PYCOLHASPREVUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

#define PYCOLHASPREVSUBUINT8_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 8 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVSUBUINT16_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 16 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVSUBUINT32_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 32 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

#define PYCOLHASPREVSUBUINT64_DOCSTRING "Check if the previous occurrence of an unsigned integer on a memory buffer containing contiguos blocks of sorted binary data still matches the search value.\n"\
"\n"\
"Parameters\n"\
"----------\n"\
"mfsrc : obj\n"\
"    Pointer to the memory mapped file.\n"\
"offset : int\n"\
"    Byte offset from where the first item starts.\n"\
"bitstart : int\n"\
"    First bit position to consider (min 0).\n"\
"bitend : int\n"\
"    Last bit position to consider (max 63).\n"\
"first : int\n"\
"    First element of the range to search (min value = 0).\n"\
"pos : int\n"\
"    Current item position.\n"\
"search : int\n"\
"    Unsigned 64 bit number to search.\n"\
"\n"\
"Returns\n"\
"-------\n"\
"tuple :\n"\
"    - 1 if the next item is valid, 0 otherwise.\n"\
"    - Previous position."

// ----------

#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
