// Python binsearch Module
//
// @category   Libraries
// @author     Nicola Asuni <info@tecnick.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/binsearch

#define MODULE_NAME "binsearch"

// PY_SSIZE_T_CLEAN must be defined before <Python.h> is included.
#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "binsearch.h"
#include "pybinsearch.h"

#define MAXNCOLS 255 //!< Largest number of columns that fits the ncols field (uint8_t)

// PyCapsule_GetPointer only checks the name, so an unqualified one would let a
// capsule built by another module (or by ctypes) be reinterpreted as a pymap_t.
#define SRC_CAPSULE_NAME "binsearch.src" //!< Name of the capsule that carries the address of the memory map

// --- ARGUMENTS ---

/**
 * Convert a Python object to an unsigned integer.
 *
 * @param obj    Object to convert.
 * @param max    Largest accepted value.
 * @param value  Address of the converted value.
 *
 * @return true on success, false with a Python exception set otherwise.
 */
static bool convert_uint(PyObject *obj, uint64_t max, uint64_t *value)
{
    unsigned long long v = PyLong_AsUnsignedLongLong(obj);
    if (PyErr_Occurred() != NULL)
    {
        return false;
    }
    if ((uint64_t)v > max)
    {
        PyErr_Format(PyExc_OverflowError, "the value must not exceed %llu", (unsigned long long)max);
        return false;
    }
    *value = (uint64_t)v;
    return true;
}

/**
 * Define a converter for the "O&" format of PyArg_ParseTupleAndKeywords.
 * Unlike the "B", "H", "I" and "K" formats it rejects the values that do not
 * fit the type instead of silently truncating them.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_convert(T) \
/** Convert a Python object to a T.
@param obj   Object to convert.
@param addr  Address of the converted value.
@return 1 on success, 0 with a Python exception set otherwise.
*/ \
static int convert_##T(PyObject *obj, void *addr) \
{ \
    uint64_t value = 0; \
    if (!convert_uint(obj, (uint64_t)((T)-1), &value)) \
    { \
        return 0; \
    } \
    *((T *)addr) = (T)value; \
    return 1; \
}

define_convert(uint8_t)
define_convert(uint16_t)
define_convert(uint32_t)
define_convert(uint64_t)

/**
 * Description of a memory map handed over to Python.
 *
 * The address, the size and the file descriptor of a mapping travel together
 * inside its capsule: the search functions check their arguments against the
 * size, and munmap_binfile releases the descriptor and the number of bytes that
 * were actually mapped instead of the ones the caller passes in.
 */
typedef struct pymap_t
{
    uint8_t *src;  //!< Address of the memory map, or MAP_FAILED once it has been released.
    uint64_t size; //!< Size of the memory map in bytes, or 0 once it has been released.
    int fd;        //!< Descriptor of the mapped file, or -1 once it has been released.
} pymap_t;

/**
 * Return the memory map carried by a capsule.
 *
 * @param mfsrc Capsule returned by mmap_binfile.
 *
 * @return The memory map, or NULL with a Python exception set.
 */
static pymap_t *get_mapped_src(PyObject *mfsrc)
{
    pymap_t *map = (pymap_t *)PyCapsule_GetPointer(mfsrc, SRC_CAPSULE_NAME);
    if (map == NULL) // the capsule is missing or has the wrong name
    {
        return NULL;
    }
    if (map->src == (uint8_t *)MAP_FAILED) // the file was never mapped or was already unmapped
    {
        PyErr_SetString(PyExc_ValueError, "the file is not memory mapped");
        return NULL;
    }
    return map;
}

/**
 * Release a memory map and mark it as unmapped.
 *
 * The map is marked before the call, so that a search that runs later raises
 * instead of reading a freed address even when munmap_binfile reports the
 * failure of the close that follows the successful unmapping.
 *
 * @param map Memory map to release.
 *
 * @return 0 on success, -1 on failure.
 */
static int release_mapped_src(pymap_t *map)
{
    mmfile_t mf;
    memset(&mf, 0, sizeof(mf));
    mf.src = map->src;
    mf.size = map->size;
    mf.fd = map->fd;
    map->src = (uint8_t *)MAP_FAILED;
    map->size = 0;
    map->fd = -1;
    return munmap_binfile(mf);
}

/**
 * Release the memory map of a capsule that is being destroyed.
 *
 * @param mfsrc Capsule returned by mmap_binfile.
 */
static void free_mapped_src(PyObject *mfsrc)
{
    pymap_t *map = (pymap_t *)PyCapsule_GetPointer(mfsrc, SRC_CAPSULE_NAME);
    if (map->src != (uint8_t *)MAP_FAILED) // the caller never called munmap_binfile
    {
        (void)release_mapped_src(map);
    }
    PyMem_Free(map);
}

/**
 * Check that the items of a row mode search are inside the memory map.
 *
 * The last item that can be read is (last - 1); the values are compared using
 * a division because the products can overflow.
 *
 * @param avail   Number of bytes available from the search offset.
 * @param blklen  Length of the binary block in bytes.
 * @param blkpos  Byte offset of the value inside a binary block.
 * @param last    Item past the last one of the search range.
 * @param tsize   Size of the searched type in bytes.
 *
 * @return true when every item of the range is inside the memory map.
 */
static bool check_row_range(uint64_t avail, uint64_t blklen, uint64_t blkpos, uint64_t last, uint64_t tsize)
{
    if (avail < tsize)
    {
        return false;
    }
    if (blkpos > (avail - tsize))
    {
        return false;
    }
    if (blklen == 0) // every item is at the same address
    {
        return true;
    }
    return ((last - 1) <= (((avail - tsize) - blkpos) / blklen));
}

/**
 * Return the address of the first block of a row mode search.
 *
 * @param mfsrc   Capsule returned by mmap_binfile.
 * @param offset  Byte offset of the first block.
 * @param blklen  Length of the binary block in bytes.
 * @param blkpos  Byte offset of the value inside a binary block.
 * @param last    Item past the last one of the search range.
 * @param tsize   Size of the searched type in bytes.
 *
 * @return The address of the first block, or NULL with a Python exception set.
 */
static const uint8_t *get_row_src(PyObject *mfsrc, uint64_t offset, uint64_t blklen, uint64_t blkpos, uint64_t last, uint64_t tsize)
{
    const pymap_t *map = get_mapped_src(mfsrc);
    if (map == NULL)
    {
        return NULL;
    }
    if ((offset > map->size) || ((last > 0) && !check_row_range((map->size - offset), blklen, blkpos, last, tsize)))
    {
        PyErr_SetString(PyExc_ValueError, "the search range is outside the memory mapped file");
        return NULL;
    }
    return (map->src + offset);
}

/**
 * Return the address of the first item of a column mode search.
 *
 * @param mfsrc   Capsule returned by mmap_binfile.
 * @param offset  Byte offset of the first item of the column.
 * @param last    Item past the last one of the search range.
 * @param tsize   Size of the searched type in bytes.
 *
 * @return The address of the first item, or NULL with a Python exception set.
 */
static const void *get_col_src(PyObject *mfsrc, uint64_t offset, uint64_t last, uint64_t tsize)
{
    const pymap_t *map = get_mapped_src(mfsrc);
    if (map == NULL)
    {
        return NULL;
    }
    if ((offset % tsize) != 0)
    {
        PyErr_SetString(PyExc_ValueError, "the offset must be a multiple of the size of the column type");
        return NULL;
    }
    if ((offset > map->size) || (last > ((map->size - offset) / tsize)))
    {
        PyErr_SetString(PyExc_ValueError, "the search range is outside the memory mapped file");
        return NULL;
    }
    return (const void *)(map->src + offset);
}

/**
 * Check that a bit range is inside the searched type.
 *
 * @param bitstart First bit to match, counted from the most significant bit.
 * @param bitend   Last bit to match, counted from the most significant bit.
 * @param tsize    Size of the searched type in bytes.
 *
 * @return true when the bit range is valid, false with a Python exception set otherwise.
 */
static bool check_bits(uint8_t bitstart, uint8_t bitend, uint64_t tsize)
{
    if ((bitstart > bitend) || (bitend >= (tsize * 8)))
    {
        PyErr_SetString(PyExc_ValueError, "the bit range must satisfy: bitstart <= bitend < (8 * size of the type)");
        return false;
    }
    return true;
}

// --- FILE ---

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    const char *file = NULL;
    PyObject* ctbytes = NULL;
    static char *kwlist[] = {"file", "ctbytes", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO", kwlist, &file, &ctbytes))
        return NULL;
    PyObject *seq = PySequence_Fast(ctbytes, "the ctbytes argument must be iterable");
    if (seq == NULL)
    {
        return NULL;
    }
    mmfile_t h;
    memset(&h, 0, sizeof(h));
    Py_ssize_t ncols = PySequence_Fast_GET_SIZE(seq);
    Py_ssize_t i = 0;
    if (ncols > MAXNCOLS)
    {
        PyErr_Format(PyExc_ValueError, "the number of columns must not exceed %d", MAXNCOLS);
    }
    else
    {
        h.ncols = (uint8_t)ncols;
        for (i = 0; i < ncols; i++)
        {
            PyObject *item = PySequence_Fast_GET_ITEM(seq, i); // borrowed reference
            // PyNumber_Index only accepts true integers: unlike PyNumber_Long
            // it does not parse a string nor truncate a float, so that a typo
            // in the column types cannot become a silently wrong layout.
            PyObject *number = PyNumber_Index(item);
            if (number == NULL)
            {
                break;
            }
            uint64_t value = 0;
            bool ok = convert_uint(number, UINT8_MAX, &value);
            Py_DECREF(number);
            if (!ok)
            {
                break;
            }
            h.ctbytes[i] = (uint8_t)value;
        }
    }
    Py_DECREF(seq);
    if (PyErr_Occurred() != NULL)
    {
        return NULL;
    }
    mmap_binfile(file, &h);
    PyObject* index = PyList_New(0);
    for (i = 0; i < h.ncols; i++)
    {
        PyObject *value = Py_BuildValue("K", (unsigned long long)h.index[i]);
        (void)PyList_Append(index, value);
        Py_DECREF(value);
    }
    // The address, the size and the descriptor of the memory map travel
    // together inside the capsule: the search functions check their arguments
    // against the size, and munmap_binfile releases what was actually mapped
    // instead of what the caller passes in. The capsule destructor releases a
    // mapping that the caller drops without unmapping it.
    pymap_t *map = (pymap_t *)PyMem_Malloc(sizeof(pymap_t));
    if (map == NULL) // LCOV_EXCL_BR_LINE : out of memory
    {
        Py_DECREF(index); // LCOV_EXCL_LINE
        return PyErr_NoMemory(); // LCOV_EXCL_LINE
    }
    map->src = h.src;
    map->size = h.size;
    map->fd = h.fd;
    PyObject *capsule = PyCapsule_New((void*)map, SRC_CAPSULE_NAME, free_mapped_src);
    return Py_BuildValue("NiKKKKBN",
                         capsule,
                         h.fd, h.size, h.doffset, h.dlength, h.nrows, h.ncols, index);
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject* mfsrc = NULL;
    int fd = -1;
    uint64_t size = 0;
    static char *kwlist[] = {"mfsrc", "fd", "size", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OiO&", kwlist, &mfsrc, &fd, convert_uint64_t, &size))
        return NULL;
    pymap_t *map = get_mapped_src(mfsrc);
    if (map == NULL)
    {
        return NULL;
    }
    // The descriptor and the size are only accepted as a confirmation: they are
    // never used to unmap, because a caller that passes the wrong ones would
    // otherwise release an arbitrary range of memory or close an unrelated
    // descriptor.
    if ((fd != map->fd) || (size != map->size))
    {
        PyErr_SetString(PyExc_ValueError, "the file descriptor and the size must be the ones returned by mmap_binfile");
        return NULL;
    }
    return Py_BuildValue("i", release_mapped_src(map));
}

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    uint64_t blklen = 0, blkpos = 0, item = 0;
    static char *kwlist[] = {"blklen", "blkpos", "item", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "O&O&O&", kwlist,
                                     convert_uint64_t, &blklen, convert_uint64_t, &blkpos, convert_uint64_t, &item))
        return NULL;
    return Py_BuildValue("K", (unsigned long long)get_address(blklen, blkpos, item));
}

// --- ROW MODE ---

/**
 * Define the wrapper of a row mode find_first or find_last function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_find(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &blklen, convert_uint64_t, &blkpos, \
                                     convert_uint64_t, &first, convert_uint64_t, &last, convert_##T, &search)) \
        return NULL; \
    const uint8_t *src = get_row_src(mfsrc, offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t h = NAME##_t(src, blklen, blkpos, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)h, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a row mode find_first_sub or find_last_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_find_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &blklen, convert_uint64_t, &blkpos, \
                                     convert_uint8_t, &bitstart, convert_uint8_t, &bitend, \
                                     convert_uint64_t, &first, convert_uint64_t, &last, convert_##T, &search)) \
        return NULL; \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(mfsrc, offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t h = NAME##_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)h, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a row mode has_next function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_has_next(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, blklen = 0, blkpos = 0, pos = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &blklen, convert_uint64_t, &blkpos, \
                                     convert_uint64_t, &pos, convert_uint64_t, &last, convert_##T, &search)) \
        return NULL; \
    const uint8_t *src = get_row_src(mfsrc, offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, blklen, blkpos, &pos, last, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

/**
 * Define the wrapper of a row mode has_next_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_has_next_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, blklen = 0, blkpos = 0, pos = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &blklen, convert_uint64_t, &blkpos, \
                                     convert_uint8_t, &bitstart, convert_uint8_t, &bitend, \
                                     convert_uint64_t, &pos, convert_uint64_t, &last, convert_##T, &search)) \
        return NULL; \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(mfsrc, offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

/**
 * Define the wrapper of a row mode has_prev function.
 * The item that can be read is the one before "pos".
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_has_prev(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, pos = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &blklen, convert_uint64_t, &blkpos, \
                                     convert_uint64_t, &first, convert_uint64_t, &pos, convert_##T, &search)) \
        return NULL; \
    const uint8_t *src = get_row_src(mfsrc, offset, blklen, blkpos, pos, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, blklen, blkpos, first, &pos, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

/**
 * Define the wrapper of a row mode has_prev_sub function.
 * The item that can be read is the one before "pos".
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_has_prev_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, pos = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &blklen, convert_uint64_t, &blkpos, \
                                     convert_uint8_t, &bitstart, convert_uint8_t, &bitend, \
                                     convert_uint64_t, &first, convert_uint64_t, &pos, convert_##T, &search)) \
        return NULL; \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(mfsrc, offset, blklen, blkpos, pos, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

define_py_find(find_first_be_uint8, uint8_t)
define_py_find(find_first_be_uint16, uint16_t)
define_py_find(find_first_be_uint32, uint32_t)
define_py_find(find_first_be_uint64, uint64_t)
define_py_find(find_last_be_uint8, uint8_t)
define_py_find(find_last_be_uint16, uint16_t)
define_py_find(find_last_be_uint32, uint32_t)
define_py_find(find_last_be_uint64, uint64_t)
define_py_find(find_first_le_uint8, uint8_t)
define_py_find(find_first_le_uint16, uint16_t)
define_py_find(find_first_le_uint32, uint32_t)
define_py_find(find_first_le_uint64, uint64_t)
define_py_find(find_last_le_uint8, uint8_t)
define_py_find(find_last_le_uint16, uint16_t)
define_py_find(find_last_le_uint32, uint32_t)
define_py_find(find_last_le_uint64, uint64_t)

define_py_find_sub(find_first_sub_be_uint8, uint8_t)
define_py_find_sub(find_first_sub_be_uint16, uint16_t)
define_py_find_sub(find_first_sub_be_uint32, uint32_t)
define_py_find_sub(find_first_sub_be_uint64, uint64_t)
define_py_find_sub(find_last_sub_be_uint8, uint8_t)
define_py_find_sub(find_last_sub_be_uint16, uint16_t)
define_py_find_sub(find_last_sub_be_uint32, uint32_t)
define_py_find_sub(find_last_sub_be_uint64, uint64_t)
define_py_find_sub(find_first_sub_le_uint8, uint8_t)
define_py_find_sub(find_first_sub_le_uint16, uint16_t)
define_py_find_sub(find_first_sub_le_uint32, uint32_t)
define_py_find_sub(find_first_sub_le_uint64, uint64_t)
define_py_find_sub(find_last_sub_le_uint8, uint8_t)
define_py_find_sub(find_last_sub_le_uint16, uint16_t)
define_py_find_sub(find_last_sub_le_uint32, uint32_t)
define_py_find_sub(find_last_sub_le_uint64, uint64_t)

define_py_has_next(has_next_be_uint8, uint8_t)
define_py_has_next(has_next_be_uint16, uint16_t)
define_py_has_next(has_next_be_uint32, uint32_t)
define_py_has_next(has_next_be_uint64, uint64_t)
define_py_has_next(has_next_le_uint8, uint8_t)
define_py_has_next(has_next_le_uint16, uint16_t)
define_py_has_next(has_next_le_uint32, uint32_t)
define_py_has_next(has_next_le_uint64, uint64_t)

define_py_has_next_sub(has_next_sub_be_uint8, uint8_t)
define_py_has_next_sub(has_next_sub_be_uint16, uint16_t)
define_py_has_next_sub(has_next_sub_be_uint32, uint32_t)
define_py_has_next_sub(has_next_sub_be_uint64, uint64_t)
define_py_has_next_sub(has_next_sub_le_uint8, uint8_t)
define_py_has_next_sub(has_next_sub_le_uint16, uint16_t)
define_py_has_next_sub(has_next_sub_le_uint32, uint32_t)
define_py_has_next_sub(has_next_sub_le_uint64, uint64_t)

define_py_has_prev(has_prev_be_uint8, uint8_t)
define_py_has_prev(has_prev_be_uint16, uint16_t)
define_py_has_prev(has_prev_be_uint32, uint32_t)
define_py_has_prev(has_prev_be_uint64, uint64_t)
define_py_has_prev(has_prev_le_uint8, uint8_t)
define_py_has_prev(has_prev_le_uint16, uint16_t)
define_py_has_prev(has_prev_le_uint32, uint32_t)
define_py_has_prev(has_prev_le_uint64, uint64_t)

define_py_has_prev_sub(has_prev_sub_be_uint8, uint8_t)
define_py_has_prev_sub(has_prev_sub_be_uint16, uint16_t)
define_py_has_prev_sub(has_prev_sub_be_uint32, uint32_t)
define_py_has_prev_sub(has_prev_sub_be_uint64, uint64_t)
define_py_has_prev_sub(has_prev_sub_le_uint8, uint8_t)
define_py_has_prev_sub(has_prev_sub_le_uint16, uint16_t)
define_py_has_prev_sub(has_prev_sub_le_uint32, uint32_t)
define_py_has_prev_sub(has_prev_sub_le_uint64, uint64_t)

// --- COLUMN MODE ---

/**
 * Define the wrapper of a column mode col_find_first or col_find_last function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, first = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &first, convert_uint64_t, &last, \
                                     convert_##T, &search)) \
        return NULL; \
    const T *src = (const T *)get_col_src(mfsrc, offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t h = NAME##_t(src, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)h, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a column mode col_find_first_sub or col_find_last_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint8_t, &bitstart, convert_uint8_t, &bitend, \
                                     convert_uint64_t, &first, convert_uint64_t, &last, convert_##T, &search)) \
        return NULL; \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(mfsrc, offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t h = NAME##_t(src, bitstart, bitend, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)h, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a column mode col_has_next function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_has_next(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, pos = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "pos", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &pos, convert_uint64_t, &last, \
                                     convert_##T, &search)) \
        return NULL; \
    const T *src = (const T *)get_col_src(mfsrc, offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, &pos, last, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

/**
 * Define the wrapper of a column mode col_has_next_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_has_next_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, pos = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "pos", "last", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint8_t, &bitstart, convert_uint8_t, &bitend, \
                                     convert_uint64_t, &pos, convert_uint64_t, &last, convert_##T, &search)) \
        return NULL; \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(mfsrc, offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, bitstart, bitend, &pos, last, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

/**
 * Define the wrapper of a column mode col_has_prev function.
 * The item that can be read is the one before "pos".
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_has_prev(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, first = 0, pos = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "first", "pos", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint64_t, &first, convert_uint64_t, &pos, \
                                     convert_##T, &search)) \
        return NULL; \
    const T *src = (const T *)get_col_src(mfsrc, offset, pos, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, first, &pos, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

/**
 * Define the wrapper of a column mode col_has_prev_sub function.
 * The item that can be read is the one before "pos".
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_has_prev_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds) \
{ \
    uint64_t offset = 0, first = 0, pos = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "pos", "search", NULL}; \
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OO&O&O&O&O&O&", kwlist, &mfsrc, \
                                     convert_uint64_t, &offset, convert_uint8_t, &bitstart, convert_uint8_t, &bitend, \
                                     convert_uint64_t, &first, convert_uint64_t, &pos, convert_##T, &search)) \
        return NULL; \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(mfsrc, offset, pos, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    bool h = NAME##_t(src, bitstart, bitend, first, &pos, search); \
    return Py_BuildValue("NK", PyBool_FromLong((long)h), (unsigned long long)pos); \
}

define_py_col_find(col_find_first_uint8, uint8_t)
define_py_col_find(col_find_first_uint16, uint16_t)
define_py_col_find(col_find_first_uint32, uint32_t)
define_py_col_find(col_find_first_uint64, uint64_t)
define_py_col_find(col_find_last_uint8, uint8_t)
define_py_col_find(col_find_last_uint16, uint16_t)
define_py_col_find(col_find_last_uint32, uint32_t)
define_py_col_find(col_find_last_uint64, uint64_t)

define_py_col_find_sub(col_find_first_sub_uint8, uint8_t)
define_py_col_find_sub(col_find_first_sub_uint16, uint16_t)
define_py_col_find_sub(col_find_first_sub_uint32, uint32_t)
define_py_col_find_sub(col_find_first_sub_uint64, uint64_t)
define_py_col_find_sub(col_find_last_sub_uint8, uint8_t)
define_py_col_find_sub(col_find_last_sub_uint16, uint16_t)
define_py_col_find_sub(col_find_last_sub_uint32, uint32_t)
define_py_col_find_sub(col_find_last_sub_uint64, uint64_t)

define_py_col_has_next(col_has_next_uint8, uint8_t)
define_py_col_has_next(col_has_next_uint16, uint16_t)
define_py_col_has_next(col_has_next_uint32, uint32_t)
define_py_col_has_next(col_has_next_uint64, uint64_t)

define_py_col_has_next_sub(col_has_next_sub_uint8, uint8_t)
define_py_col_has_next_sub(col_has_next_sub_uint16, uint16_t)
define_py_col_has_next_sub(col_has_next_sub_uint32, uint32_t)
define_py_col_has_next_sub(col_has_next_sub_uint64, uint64_t)

define_py_col_has_prev(col_has_prev_uint8, uint8_t)
define_py_col_has_prev(col_has_prev_uint16, uint16_t)
define_py_col_has_prev(col_has_prev_uint32, uint32_t)
define_py_col_has_prev(col_has_prev_uint64, uint64_t)

define_py_col_has_prev_sub(col_has_prev_sub_uint8, uint8_t)
define_py_col_has_prev_sub(col_has_prev_sub_uint16, uint16_t)
define_py_col_has_prev_sub(col_has_prev_sub_uint32, uint32_t)
define_py_col_has_prev_sub(col_has_prev_sub_uint64, uint64_t)

// --- MODULE ---

// Add an entry of the method table of the module.
#define PY_METHOD(NAME, DOC) {#NAME, (PyCFunction)(void(*)(void))py_##NAME, METH_VARARGS|METH_KEYWORDS, DOC},

static PyMethodDef PyBinsearchMethods[] =
{
    PY_METHOD(mmap_binfile, PYMMAPBINFILE_DOCSTRING)
    PY_METHOD(munmap_binfile, PYMUNMAPBINFILE_DOCSTRING)
    PY_METHOD(get_address, PYGETADDRESS_DOCSTRING)

    PY_METHOD(find_first_be_uint8, PYFINDFIRSTUINT8_DOCSTRING)
    PY_METHOD(find_first_be_uint16, PYFINDFIRSTUINT16_DOCSTRING)
    PY_METHOD(find_first_be_uint32, PYFINDFIRSTUINT32_DOCSTRING)
    PY_METHOD(find_first_be_uint64, PYFINDFIRSTUINT64_DOCSTRING)
    PY_METHOD(find_last_be_uint8, PYFINDLASTUINT8_DOCSTRING)
    PY_METHOD(find_last_be_uint16, PYFINDLASTUINT16_DOCSTRING)
    PY_METHOD(find_last_be_uint32, PYFINDLASTUINT32_DOCSTRING)
    PY_METHOD(find_last_be_uint64, PYFINDLASTUINT64_DOCSTRING)
    PY_METHOD(find_first_sub_be_uint8, PYFINDFIRSTSUBUINT8_DOCSTRING)
    PY_METHOD(find_first_sub_be_uint16, PYFINDFIRSTSUBUINT16_DOCSTRING)
    PY_METHOD(find_first_sub_be_uint32, PYFINDFIRSTSUBUINT32_DOCSTRING)
    PY_METHOD(find_first_sub_be_uint64, PYFINDFIRSTSUBUINT64_DOCSTRING)
    PY_METHOD(find_last_sub_be_uint8, PYFINDLASTSUBUINT8_DOCSTRING)
    PY_METHOD(find_last_sub_be_uint16, PYFINDLASTSUBUINT16_DOCSTRING)
    PY_METHOD(find_last_sub_be_uint32, PYFINDLASTSUBUINT32_DOCSTRING)
    PY_METHOD(find_last_sub_be_uint64, PYFINDLASTSUBUINT64_DOCSTRING)
    PY_METHOD(has_next_be_uint8, PYHASNEXTUINT8_DOCSTRING)
    PY_METHOD(has_next_be_uint16, PYHASNEXTUINT16_DOCSTRING)
    PY_METHOD(has_next_be_uint32, PYHASNEXTUINT32_DOCSTRING)
    PY_METHOD(has_next_be_uint64, PYHASNEXTUINT64_DOCSTRING)
    PY_METHOD(has_next_sub_be_uint8, PYHASNEXTSUBUINT8_DOCSTRING)
    PY_METHOD(has_next_sub_be_uint16, PYHASNEXTSUBUINT16_DOCSTRING)
    PY_METHOD(has_next_sub_be_uint32, PYHASNEXTSUBUINT32_DOCSTRING)
    PY_METHOD(has_next_sub_be_uint64, PYHASNEXTSUBUINT64_DOCSTRING)
    PY_METHOD(has_prev_be_uint8, PYHASPREVUINT8_DOCSTRING)
    PY_METHOD(has_prev_be_uint16, PYHASPREVUINT16_DOCSTRING)
    PY_METHOD(has_prev_be_uint32, PYHASPREVUINT32_DOCSTRING)
    PY_METHOD(has_prev_be_uint64, PYHASPREVUINT64_DOCSTRING)
    PY_METHOD(has_prev_sub_be_uint8, PYHASPREVSUBUINT8_DOCSTRING)
    PY_METHOD(has_prev_sub_be_uint16, PYHASPREVSUBUINT16_DOCSTRING)
    PY_METHOD(has_prev_sub_be_uint32, PYHASPREVSUBUINT32_DOCSTRING)
    PY_METHOD(has_prev_sub_be_uint64, PYHASPREVSUBUINT64_DOCSTRING)

    PY_METHOD(find_first_le_uint8, PYFINDFIRSTUINT8_DOCSTRING)
    PY_METHOD(find_first_le_uint16, PYFINDFIRSTUINT16_DOCSTRING)
    PY_METHOD(find_first_le_uint32, PYFINDFIRSTUINT32_DOCSTRING)
    PY_METHOD(find_first_le_uint64, PYFINDFIRSTUINT64_DOCSTRING)
    PY_METHOD(find_last_le_uint8, PYFINDLASTUINT8_DOCSTRING)
    PY_METHOD(find_last_le_uint16, PYFINDLASTUINT16_DOCSTRING)
    PY_METHOD(find_last_le_uint32, PYFINDLASTUINT32_DOCSTRING)
    PY_METHOD(find_last_le_uint64, PYFINDLASTUINT64_DOCSTRING)
    PY_METHOD(find_first_sub_le_uint8, PYFINDFIRSTSUBUINT8_DOCSTRING)
    PY_METHOD(find_first_sub_le_uint16, PYFINDFIRSTSUBUINT16_DOCSTRING)
    PY_METHOD(find_first_sub_le_uint32, PYFINDFIRSTSUBUINT32_DOCSTRING)
    PY_METHOD(find_first_sub_le_uint64, PYFINDFIRSTSUBUINT64_DOCSTRING)
    PY_METHOD(find_last_sub_le_uint8, PYFINDLASTSUBUINT8_DOCSTRING)
    PY_METHOD(find_last_sub_le_uint16, PYFINDLASTSUBUINT16_DOCSTRING)
    PY_METHOD(find_last_sub_le_uint32, PYFINDLASTSUBUINT32_DOCSTRING)
    PY_METHOD(find_last_sub_le_uint64, PYFINDLASTSUBUINT64_DOCSTRING)
    PY_METHOD(has_next_le_uint8, PYHASNEXTUINT8_DOCSTRING)
    PY_METHOD(has_next_le_uint16, PYHASNEXTUINT16_DOCSTRING)
    PY_METHOD(has_next_le_uint32, PYHASNEXTUINT32_DOCSTRING)
    PY_METHOD(has_next_le_uint64, PYHASNEXTUINT64_DOCSTRING)
    PY_METHOD(has_next_sub_le_uint8, PYHASNEXTSUBUINT8_DOCSTRING)
    PY_METHOD(has_next_sub_le_uint16, PYHASNEXTSUBUINT16_DOCSTRING)
    PY_METHOD(has_next_sub_le_uint32, PYHASNEXTSUBUINT32_DOCSTRING)
    PY_METHOD(has_next_sub_le_uint64, PYHASNEXTSUBUINT64_DOCSTRING)
    PY_METHOD(has_prev_le_uint8, PYHASPREVUINT8_DOCSTRING)
    PY_METHOD(has_prev_le_uint16, PYHASPREVUINT16_DOCSTRING)
    PY_METHOD(has_prev_le_uint32, PYHASPREVUINT32_DOCSTRING)
    PY_METHOD(has_prev_le_uint64, PYHASPREVUINT64_DOCSTRING)
    PY_METHOD(has_prev_sub_le_uint8, PYHASPREVSUBUINT8_DOCSTRING)
    PY_METHOD(has_prev_sub_le_uint16, PYHASPREVSUBUINT16_DOCSTRING)
    PY_METHOD(has_prev_sub_le_uint32, PYHASPREVSUBUINT32_DOCSTRING)
    PY_METHOD(has_prev_sub_le_uint64, PYHASPREVSUBUINT64_DOCSTRING)

    PY_METHOD(col_find_first_uint8, PYCOLFINDFIRSTUINT8_DOCSTRING)
    PY_METHOD(col_find_first_uint16, PYCOLFINDFIRSTUINT16_DOCSTRING)
    PY_METHOD(col_find_first_uint32, PYCOLFINDFIRSTUINT32_DOCSTRING)
    PY_METHOD(col_find_first_uint64, PYCOLFINDFIRSTUINT64_DOCSTRING)
    PY_METHOD(col_find_last_uint8, PYCOLFINDLASTUINT8_DOCSTRING)
    PY_METHOD(col_find_last_uint16, PYCOLFINDLASTUINT16_DOCSTRING)
    PY_METHOD(col_find_last_uint32, PYCOLFINDLASTUINT32_DOCSTRING)
    PY_METHOD(col_find_last_uint64, PYCOLFINDLASTUINT64_DOCSTRING)
    PY_METHOD(col_find_first_sub_uint8, PYCOLFINDFIRSTSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_first_sub_uint16, PYCOLFINDFIRSTSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_first_sub_uint32, PYCOLFINDFIRSTSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_first_sub_uint64, PYCOLFINDFIRSTSUBUINT64_DOCSTRING)
    PY_METHOD(col_find_last_sub_uint8, PYCOLFINDLASTSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_last_sub_uint16, PYCOLFINDLASTSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_last_sub_uint32, PYCOLFINDLASTSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_last_sub_uint64, PYCOLFINDLASTSUBUINT64_DOCSTRING)
    PY_METHOD(col_has_next_uint8, PYCOLHASNEXTUINT8_DOCSTRING)
    PY_METHOD(col_has_next_uint16, PYCOLHASNEXTUINT16_DOCSTRING)
    PY_METHOD(col_has_next_uint32, PYCOLHASNEXTUINT32_DOCSTRING)
    PY_METHOD(col_has_next_uint64, PYCOLHASNEXTUINT64_DOCSTRING)
    PY_METHOD(col_has_next_sub_uint8, PYCOLHASNEXTSUBUINT8_DOCSTRING)
    PY_METHOD(col_has_next_sub_uint16, PYCOLHASNEXTSUBUINT16_DOCSTRING)
    PY_METHOD(col_has_next_sub_uint32, PYCOLHASNEXTSUBUINT32_DOCSTRING)
    PY_METHOD(col_has_next_sub_uint64, PYCOLHASNEXTSUBUINT64_DOCSTRING)
    PY_METHOD(col_has_prev_uint8, PYCOLHASPREVUINT8_DOCSTRING)
    PY_METHOD(col_has_prev_uint16, PYCOLHASPREVUINT16_DOCSTRING)
    PY_METHOD(col_has_prev_uint32, PYCOLHASPREVUINT32_DOCSTRING)
    PY_METHOD(col_has_prev_uint64, PYCOLHASPREVUINT64_DOCSTRING)
    PY_METHOD(col_has_prev_sub_uint8, PYCOLHASPREVSUBUINT8_DOCSTRING)
    PY_METHOD(col_has_prev_sub_uint16, PYCOLHASPREVSUBUINT16_DOCSTRING)
    PY_METHOD(col_has_prev_sub_uint32, PYCOLHASPREVSUBUINT32_DOCSTRING)
    PY_METHOD(col_has_prev_sub_uint64, PYCOLHASPREVSUBUINT64_DOCSTRING)

    {
        NULL, NULL, 0, NULL
    }
};

static const char modulename[] = MODULE_NAME;

// The module keeps no per-interpreter state, so m_size is 0.
static struct PyModuleDef moduledef =
{
    PyModuleDef_HEAD_INIT,
    modulename,
    PYBINSEARCH_DOCSTRING,
    0,
    PyBinsearchMethods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_binsearch(void)
{
    return PyModule_Create(&moduledef);
}
