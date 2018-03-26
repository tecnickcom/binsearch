// Python binsearch Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@tecnick.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/binsearch

#define PY_SSIZE_T_CLEAN
#include <Python.h>

static PyObject *py_mmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_munmap_binfile(PyObject *self, PyObject *args);
static PyObject *py_get_address(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint8(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint16(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint32(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint64(PyObject *self, PyObject *args);
static PyObject *py_find_first_uint128(PyObject *self, PyObject *args);
static PyObject *py_find_last_uint128(PyObject *self, PyObject *args);

PyMODINIT_FUNC initbinsearch(void);

#define PYMMAPBINFILE_DOCSTRING "Memory map the specified file."
#define PYMUNMAPBINFILE_DOCSTRING "Unmap and close the memory-mapped file."
#define PYGETADDRESS_DOCSTRING "Returns the absolute file start position of the specified item (binary block)."
#define PYFINDFIRSTUINT8_DOCSTRING "Search for the first occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT8_DOCSTRING "Search for the last occurrence of a 8 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 8 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT16_DOCSTRING "Search for the first occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT16_DOCSTRING "Search for the last occurrence of a 16 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 16 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT32_DOCSTRING "Search for the first occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT32_DOCSTRING "Search for the last occurrence of a 32 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 32 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT64_DOCSTRING "Search for the first occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT64_DOCSTRING "Search for the last occurrence of a 64 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 64 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDFIRSTUINT128_DOCSTRING "Search for the first occurrence of a 128 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 128 bit values in the file must encoded in big-endian format and sorted in ascending order."
#define PYFINDLASTUINT128_DOCSTRING "Search for the last occurrence of a 128 bit unsigned integer on a memory mapped binary file containing adjacent blocks of sorted binary data. The 128 bit values in the file must encoded in big-endian format and sorted in ascending order."

#if defined(__SUNPRO_C) || defined(__hpux) || defined(_AIX)
#define inline
#endif

#ifdef __linux
#define inline __inline
#endif
