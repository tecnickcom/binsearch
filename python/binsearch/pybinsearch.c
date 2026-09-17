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
 * Define a converter for one of the searched types.
 * Unlike the "B", "H", "I" and "K" formats of PyArg_ParseTupleAndKeywords it
 * rejects the values that do not fit the type instead of silently truncating
 * them.
 *
 * @param T Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_convert(T) \
/** Convert a Python object to a T.
@param obj   Object to convert.
@param addr  Address of the converted value.
@return true on success, false with a Python exception set otherwise.
*/ \
static bool convert_##T(PyObject *obj, T *addr) \
{ \
    uint64_t value = 0; \
    if (!convert_uint(obj, (uint64_t)((T)-1), &value)) \
    { \
        return false; \
    } \
    *addr = (T)value; \
    return true; \
}

define_convert(uint8_t)
define_convert(uint16_t)
define_convert(uint32_t)
define_convert(uint64_t)

/**
 * Read a sequence of searched values into an array, together with the array
 * that receives their positions.
 *
 * Both arrays come out of one allocation, the positions first so that the
 * values that follow them are aligned for their type whatever it is. The
 * caller releases it by passing the positions to build_pos_list.
 *
 * The values are narrowed to the searched type here rather than in each of the
 * wrappers, so that the sequence is read in one place for all of them.
 *
 * @param obj     Sequence of values.
 * @param tsize   Size of the searched type in bytes.
 * @param pos     Receives the address of the positions, which is the allocation to release.
 * @param search  Receives the address of the values.
 *
 * @return The number of values, or -1 with a Python exception set.
 */
static Py_ssize_t take_search(PyObject *obj, size_t tsize, uint64_t **pos, void **search)
{
    PyObject *seq = PySequence_Fast(obj, "the search argument must be iterable");
    if (seq == NULL)
    {
        return -1;
    }
    const Py_ssize_t count = PySequence_Fast_GET_SIZE(seq);
    // One item more than the count, so that an empty sequence still has an
    // allocation to release and neither array is a null pointer.
    void *block = PyMem_Malloc(((size_t)count + 1) * (sizeof(uint64_t) + tsize));
    if (block == NULL) // LCOV_EXCL_BR_LINE : out of memory
    {
        Py_DECREF(seq); // LCOV_EXCL_LINE
        PyErr_NoMemory(); // LCOV_EXCL_LINE
        return -1; // LCOV_EXCL_LINE
    }
    uint64_t *positions = (uint64_t *)block;
    void *values = (void *)(positions + count + 1);
    const uint64_t max = (tsize >= sizeof(uint64_t)) ? UINT64_MAX : ((UINT64_C(1) << (tsize * 8)) - 1);
    Py_ssize_t i = 0;
    for (i = 0; i < count; i++)
    {
        PyObject *item = PySequence_Fast_GET_ITEM(seq, i); // borrowed reference
        uint64_t value = 0;
        if (!convert_uint(item, max, &value))
        {
            Py_DECREF(seq);
            PyMem_Free(block);
            return -1;
        }
        switch (tsize)
        {
        case 1:
            ((uint8_t *)values)[i] = (uint8_t)value;
            break;
        case 2:
            ((uint16_t *)values)[i] = (uint16_t)value;
            break;
        case 4:
            ((uint32_t *)values)[i] = (uint32_t)value;
            break;
        default:
            ((uint64_t *)values)[i] = value;
            break;
        }
    }
    Py_DECREF(seq);
    *pos = positions;
    *search = values;
    return count;
}

/**
 * Build the list of positions a find_many function reported and release the
 * allocation they are in.
 *
 * @param pos   Positions, and the allocation to release.
 * @param count Number of positions.
 *
 * @return The list of positions, or NULL with a Python exception set.
 */
static PyObject *build_pos_list(uint64_t *pos, Py_ssize_t count)
{
    PyObject *list = PyList_New(count);
    if (list == NULL) // LCOV_EXCL_BR_LINE : out of memory
    {
        PyMem_Free(pos); // LCOV_EXCL_LINE
        return NULL; // LCOV_EXCL_LINE
    }
    Py_ssize_t i = 0;
    for (i = 0; i < count; i++)
    {
        PyObject *item = PyLong_FromUnsignedLongLong((unsigned long long)pos[i]);
        if (item == NULL) // LCOV_EXCL_BR_LINE : out of memory
        {
            Py_DECREF(list); // LCOV_EXCL_LINE
            PyMem_Free(pos); // LCOV_EXCL_LINE
            return NULL; // LCOV_EXCL_LINE
        }
        PyList_SET_ITEM(list, i, item); // steals the reference
    }
    PyMem_Free(pos);
    return list;
}

/**
 * Sort the arguments of a METH_FASTCALL call into the order of "names".
 *
 * The functions of this module are called in a loop, so they take the arguments
 * as the vector the interpreter already has rather than as a tuple and a
 * dictionary built for the call: that alone is about half of the cost of a
 * search over data that fits in the cache. A call that passes every argument by
 * position does not touch the names at all.
 *
 * @param args     Argument vector: "nargs" positional values followed by one value per name in "kwnames".
 * @param nargs    Number of positional arguments.
 * @param kwnames  Tuple of keyword names, or NULL when there is none.
 * @param names    Names of the parameters, in positional order.
 * @param nparams  Number of parameters.
 * @param out      Array of "nparams" items that receives the value of each parameter.
 *
 * @return true on success, false with a Python exception set otherwise.
 */
static bool parse_args(PyObject *const *args, Py_ssize_t nargs, PyObject *kwnames,
                       const char *const *names, Py_ssize_t nparams, Py_ssize_t nrequired, PyObject **out)
{
    Py_ssize_t i = 0;
    if (nargs > nparams)
    {
        PyErr_Format(PyExc_TypeError, "expected at most %zd arguments, got %zd", nparams, nargs);
        return false;
    }
    for (i = 0; i < nparams; i++)
    {
        out[i] = (i < nargs) ? args[i] : NULL;
    }
    const Py_ssize_t nkw = (kwnames == NULL) ? 0 : PyTuple_GET_SIZE(kwnames);
    for (i = 0; i < nkw; i++)
    {
        PyObject *key = PyTuple_GET_ITEM(kwnames, i); // borrowed reference
        Py_ssize_t k = 0;
        while ((k < nparams) && (PyUnicode_CompareWithASCIIString(key, names[k]) != 0))
        {
            k++;
        }
        if (k == nparams)
        {
            PyErr_Format(PyExc_TypeError, "unexpected keyword argument '%S'", key);
            return false;
        }
        if (out[k] != NULL)
        {
            PyErr_Format(PyExc_TypeError, "argument '%S' given twice", key);
            return false;
        }
        out[k] = args[nargs + i];
    }
    for (i = 0; i < nrequired; i++)
    {
        if (out[i] == NULL)
        {
            PyErr_Format(PyExc_TypeError, "missing argument '%s'", names[i]);
            return false;
        }
    }
    return true;
}

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
 * The address is taken with an atomic exchange, so that only the caller that
 * finds a mapped address releases it. A free-threaded interpreter runs two
 * munmap_binfile calls on the same map at the same time, and a plain read
 * followed by a write would let both of them unmap the address and close the
 * descriptor: the second close would land on whatever descriptor number the
 * process has reused by then.
 *
 * The map is marked before the mapping is released, so that a search that runs
 * later raises instead of reading a freed address even when munmap_binfile
 * reports the failure of the close that follows the successful unmapping.
 *
 * @param map Memory map to release.
 *
 * @return 0 on success, -1 on failure, 1 when another caller released it first.
 */
static int release_mapped_src(pymap_t *map)
{
    uint8_t *src = binsearch_atomic_take(&map->src);
    if (src == (uint8_t *)MAP_FAILED)
    {
        return 1;
    }
    mmfile_t mf;
    memset(&mf, 0, sizeof(mf));
    mf.src = src;
    mf.size = map->size;
    mf.fd = map->fd;
    map->size = 0;
    map->fd = -1;
    return munmap_binfile(&mf);
}

/**
 * Release the memory map of a capsule that is being destroyed.
 *
 * @param mfsrc Capsule returned by mmap_binfile.
 */
static void free_mapped_src(PyObject *mfsrc)
{
    pymap_t *map = (pymap_t *)PyCapsule_GetPointer(mfsrc, SRC_CAPSULE_NAME);
    if (map == NULL) // LCOV_EXCL_BR_LINE : only reachable through a foreign capsule
    {
        PyErr_Clear(); // LCOV_EXCL_LINE
        return; // LCOV_EXCL_LINE
    }
    // release_mapped_src reports that it had nothing to release when the caller
    // already unmapped the file, so the state is not tested again here.
    (void)release_mapped_src(map);
    PyMem_Free(map);
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
    if ((offset > map->size) || !binsearch_check_row_range((map->size - offset), blklen, blkpos, last, tsize))
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
    if (!binsearch_check_col_offset(offset, tsize))
    {
        PyErr_SetString(PyExc_ValueError, "the offset must be a multiple of the size of the column type");
        return NULL;
    }
    if ((offset > map->size) || !binsearch_check_col_range((map->size - offset), last, tsize))
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
    if (!binsearch_check_bits(bitstart, bitend, tsize))
    {
        PyErr_SetString(PyExc_ValueError, "the bit range must satisfy: bitstart <= bitend < (8 * size of the type)");
        return false;
    }
    return true;
}

// --- FILE ---

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames)
{
    static const char *const names[] = {"file", "ctbytes"};
    PyObject *argv[2];
    if (!parse_args(argv_in, nargs, kwnames, names, 2, 2, argv))
    {
        return NULL;
    }
    const char *file = PyUnicode_AsUTF8(argv[0]);
    if (file == NULL)
    {
        return NULL;
    }
    PyObject *seq = PySequence_Fast(argv[1], "the ctbytes argument must be iterable");
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
    // The column type sizes are reported back because the BINSRC1 format reads
    // them from the file header: without them the caller cannot tell which of
    // the typed search functions to call for each column.
    PyObject *index = PyList_New((Py_ssize_t)h.ncols);
    PyObject *types = PyList_New((Py_ssize_t)h.ncols);
    if ((index == NULL) || (types == NULL)) // LCOV_EXCL_BR_LINE : out of memory
    {
        Py_XDECREF(index); // LCOV_EXCL_LINE
        Py_XDECREF(types); // LCOV_EXCL_LINE
        return NULL; // LCOV_EXCL_LINE
    }
    for (i = 0; i < h.ncols; i++)
    {
        PyObject *offset = Py_BuildValue("K", (unsigned long long)h.index[i]);
        PyObject *ctbyte = Py_BuildValue("B", h.ctbytes[i]);
        if ((offset == NULL) || (ctbyte == NULL)) // LCOV_EXCL_BR_LINE : out of memory
        {
            Py_XDECREF(offset); // LCOV_EXCL_LINE
            Py_XDECREF(ctbyte); // LCOV_EXCL_LINE
            Py_DECREF(index); // LCOV_EXCL_LINE
            Py_DECREF(types); // LCOV_EXCL_LINE
            return NULL; // LCOV_EXCL_LINE
        }
        PyList_SET_ITEM(index, i, offset); // steals the reference
        PyList_SET_ITEM(types, i, ctbyte); // steals the reference
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
        Py_DECREF(types); // LCOV_EXCL_LINE
        return PyErr_NoMemory(); // LCOV_EXCL_LINE
    }
    map->src = h.src;
    map->size = h.size;
    map->fd = h.fd;
    PyObject *capsule = PyCapsule_New((void*)map, SRC_CAPSULE_NAME, free_mapped_src);
    if (capsule == NULL) // LCOV_EXCL_BR_LINE : out of memory
    {
        // The capsule owns the map once it exists, so the mapping is released
        // here instead of being leaked with it.
        (void)release_mapped_src(map); // LCOV_EXCL_LINE
        PyMem_Free(map); // LCOV_EXCL_LINE
        Py_DECREF(index); // LCOV_EXCL_LINE
        Py_DECREF(types); // LCOV_EXCL_LINE
        return NULL; // LCOV_EXCL_LINE
    }
    return Py_BuildValue("NiKKKKBBNN",
                         capsule,
                         h.fd, h.size, h.doffset, h.dlength, h.nrows, h.ncols, h.border, index, types);
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames)
{
    static const char *const names[] = {"mfsrc", "fd", "size"};
    PyObject *argv[3];
    uint64_t size = 0;
    if (!parse_args(argv_in, nargs, kwnames, names, 3, 3, argv))
    {
        return NULL;
    }
    const int fd = (int)PyLong_AsLong(argv[1]);
    if ((fd == -1) && (PyErr_Occurred() != NULL))
    {
        return NULL;
    }
    if (!convert_uint64_t(argv[2], &size))
    {
        return NULL;
    }
    pymap_t *map = get_mapped_src(argv[0]);
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

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames)
{
    static const char *const names[] = {"blklen", "blkpos", "item"};
    PyObject *argv[3];
    uint64_t blklen = 0, blkpos = 0, item = 0;
    if (!parse_args(argv_in, nargs, kwnames, names, 3, 3, argv))
    {
        return NULL;
    }
    if (!convert_uint64_t(argv[0], &blklen)
            || !convert_uint64_t(argv[1], &blkpos)
            || !convert_uint64_t(argv[2], &item))
    {
        return NULL;
    }
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &last) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search"}; \
    PyObject *argv[9]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 9, 9, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint8_t(argv[4], &bitstart) \
            || !convert_uint8_t(argv[5], &bitend) \
            || !convert_uint64_t(argv[6], &first) \
            || !convert_uint64_t(argv[7], &last) \
            || !convert_##T(argv[8], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, pos = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint64_t(argv[4], &pos) \
            || !convert_uint64_t(argv[5], &last) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search"}; \
    PyObject *argv[9]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, pos = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 9, 9, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint8_t(argv[4], &bitstart) \
            || !convert_uint8_t(argv[5], &bitend) \
            || !convert_uint64_t(argv[6], &pos) \
            || !convert_uint64_t(argv[7], &last) \
            || !convert_##T(argv[8], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, pos = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &pos) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search"}; \
    PyObject *argv[9]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, pos = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 9, 9, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint8_t(argv[4], &bitstart) \
            || !convert_uint8_t(argv[5], &bitend) \
            || !convert_uint64_t(argv[6], &first) \
            || !convert_uint64_t(argv[7], &pos) \
            || !convert_##T(argv[8], &search)) \
    { \
        return NULL; \
    } \
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

/**
 * Define the wrapper of a row mode find_range function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_find_range(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    T search = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &last) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(argv[0], offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t n = NAME##_t(src, blklen, blkpos, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)n, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a row mode find_range_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_find_range_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search"}; \
    PyObject *argv[9]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 9, 9, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint8_t(argv[4], &bitstart) \
            || !convert_uint8_t(argv[5], &bitend) \
            || !convert_uint64_t(argv[6], &first) \
            || !convert_uint64_t(argv[7], &last) \
            || !convert_##T(argv[8], &search)) \
    { \
        return NULL; \
    } \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(argv[0], offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t n = NAME##_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)n, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Convert the optional "prefetch" argument of a find_many wrapper.
 *
 * @param obj Argument to convert, or NULL when the caller omitted it.
 * @param out Receives the mode.
 *
 * @return true on success, false with a Python exception set otherwise.
 */
static bool convert_prefetch(PyObject *obj, binsearch_prefetch_t *out)
{
    if (obj == NULL)
    {
        *out = BINSEARCH_PREFETCH_AUTO;
        return true;
    }
    uint64_t v = 0;
    if (!convert_uint64_t(obj, &v))
    {
        return false;
    }
    if (v > (uint64_t)BINSEARCH_PREFETCH_NEVER)
    {
        PyErr_Format(PyExc_ValueError, "prefetch must be PREFETCH_AUTO, PREFETCH_ALWAYS or PREFETCH_NEVER: got %llu", (unsigned long long)v);
        return false;
    }
    *out = (binsearch_prefetch_t)v;
    return true;
}

/**
 * Define the wrapper of a row mode find_many function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_find_many(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", "prefetch"}; \
    PyObject *argv[8]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 8, 7, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &last)) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(argv[0], offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    binsearch_prefetch_t prefetch = BINSEARCH_PREFETCH_AUTO; \
    if (!convert_prefetch(argv[7], &prefetch)) \
    { \
        return NULL; \
    } \
    T *search = NULL; \
    uint64_t *pos = NULL; \
    Py_ssize_t count = take_search(argv[6], sizeof(T), &pos, (void **)&search); \
    if (count < 0) \
    { \
        return NULL; \
    } \
    NAME##_t(src, blklen, blkpos, first, last, search, pos, (uint64_t)count, prefetch); \
    return build_pos_list(pos, count); \
}

/**
 * Define the wrapper of a row mode find_many_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_find_many_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", "prefetch"}; \
    PyObject *argv[10]; \
    uint64_t offset = 0, blklen = 0, blkpos = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 10, 9, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &blklen) \
            || !convert_uint64_t(argv[3], &blkpos) \
            || !convert_uint8_t(argv[4], &bitstart) \
            || !convert_uint8_t(argv[5], &bitend) \
            || !convert_uint64_t(argv[6], &first) \
            || !convert_uint64_t(argv[7], &last)) \
    { \
        return NULL; \
    } \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const uint8_t *src = get_row_src(argv[0], offset, blklen, blkpos, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    binsearch_prefetch_t prefetch = BINSEARCH_PREFETCH_AUTO; \
    if (!convert_prefetch(argv[9], &prefetch)) \
    { \
        return NULL; \
    } \
    T *search = NULL; \
    uint64_t *pos = NULL; \
    Py_ssize_t count = take_search(argv[8], sizeof(T), &pos, (void **)&search); \
    if (count < 0) \
    { \
        return NULL; \
    } \
    NAME##_t(src, blklen, blkpos, bitstart, bitend, first, last, search, pos, (uint64_t)count, prefetch); \
    return build_pos_list(pos, count); \
}

define_py_find_range(find_range_be_uint8, uint8_t)
define_py_find_range(find_range_be_uint16, uint16_t)
define_py_find_range(find_range_be_uint32, uint32_t)
define_py_find_range(find_range_be_uint64, uint64_t)
define_py_find_range(find_range_le_uint8, uint8_t)
define_py_find_range(find_range_le_uint16, uint16_t)
define_py_find_range(find_range_le_uint32, uint32_t)
define_py_find_range(find_range_le_uint64, uint64_t)

define_py_find_range_sub(find_range_sub_be_uint8, uint8_t)
define_py_find_range_sub(find_range_sub_be_uint16, uint16_t)
define_py_find_range_sub(find_range_sub_be_uint32, uint32_t)
define_py_find_range_sub(find_range_sub_be_uint64, uint64_t)
define_py_find_range_sub(find_range_sub_le_uint8, uint8_t)
define_py_find_range_sub(find_range_sub_le_uint16, uint16_t)
define_py_find_range_sub(find_range_sub_le_uint32, uint32_t)
define_py_find_range_sub(find_range_sub_le_uint64, uint64_t)

define_py_find_many(find_many_be_uint8, uint8_t)
define_py_find_many(find_many_be_uint16, uint16_t)
define_py_find_many(find_many_be_uint32, uint32_t)
define_py_find_many(find_many_be_uint64, uint64_t)
define_py_find_many(find_many_le_uint8, uint8_t)
define_py_find_many(find_many_le_uint16, uint16_t)
define_py_find_many(find_many_le_uint32, uint32_t)
define_py_find_many(find_many_le_uint64, uint64_t)

define_py_find_many_sub(find_many_sub_be_uint8, uint8_t)
define_py_find_many_sub(find_many_sub_be_uint16, uint16_t)
define_py_find_many_sub(find_many_sub_be_uint32, uint32_t)
define_py_find_many_sub(find_many_sub_be_uint64, uint64_t)
define_py_find_many_sub(find_many_sub_le_uint8, uint8_t)
define_py_find_many_sub(find_many_sub_le_uint16, uint16_t)
define_py_find_many_sub(find_many_sub_le_uint32, uint32_t)
define_py_find_many_sub(find_many_sub_le_uint64, uint64_t)

// --- COLUMN MODE ---

/**
 * Define the wrapper of a column mode col_find_first or col_find_last function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "first", "last", "search"}; \
    PyObject *argv[5]; \
    uint64_t offset = 0, first = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 5, 5, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &first) \
            || !convert_uint64_t(argv[3], &last) \
            || !convert_##T(argv[4], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint8_t(argv[2], &bitstart) \
            || !convert_uint8_t(argv[3], &bitend) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &last) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "pos", "last", "search"}; \
    PyObject *argv[5]; \
    uint64_t offset = 0, pos = 0, last = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 5, 5, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &pos) \
            || !convert_uint64_t(argv[3], &last) \
            || !convert_##T(argv[4], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "bitstart", "bitend", "pos", "last", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, pos = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint8_t(argv[2], &bitstart) \
            || !convert_uint8_t(argv[3], &bitend) \
            || !convert_uint64_t(argv[4], &pos) \
            || !convert_uint64_t(argv[5], &last) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "first", "pos", "search"}; \
    PyObject *argv[5]; \
    uint64_t offset = 0, first = 0, pos = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 5, 5, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &first) \
            || !convert_uint64_t(argv[3], &pos) \
            || !convert_##T(argv[4], &search)) \
    { \
        return NULL; \
    } \
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
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "pos", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, first = 0, pos = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    PyObject* mfsrc = NULL; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    mfsrc = argv[0]; \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint8_t(argv[2], &bitstart) \
            || !convert_uint8_t(argv[3], &bitend) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &pos) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
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

define_py_col_find(col_find_first_be_uint8, uint8_t)
define_py_col_find(col_find_first_be_uint16, uint16_t)
define_py_col_find(col_find_first_be_uint32, uint32_t)
define_py_col_find(col_find_first_be_uint64, uint64_t)
define_py_col_find(col_find_last_be_uint8, uint8_t)
define_py_col_find(col_find_last_be_uint16, uint16_t)
define_py_col_find(col_find_last_be_uint32, uint32_t)
define_py_col_find(col_find_last_be_uint64, uint64_t)

define_py_col_find_sub(col_find_first_sub_be_uint8, uint8_t)
define_py_col_find_sub(col_find_first_sub_be_uint16, uint16_t)
define_py_col_find_sub(col_find_first_sub_be_uint32, uint32_t)
define_py_col_find_sub(col_find_first_sub_be_uint64, uint64_t)
define_py_col_find_sub(col_find_last_sub_be_uint8, uint8_t)
define_py_col_find_sub(col_find_last_sub_be_uint16, uint16_t)
define_py_col_find_sub(col_find_last_sub_be_uint32, uint32_t)
define_py_col_find_sub(col_find_last_sub_be_uint64, uint64_t)

define_py_col_has_next(col_has_next_be_uint8, uint8_t)
define_py_col_has_next(col_has_next_be_uint16, uint16_t)
define_py_col_has_next(col_has_next_be_uint32, uint32_t)
define_py_col_has_next(col_has_next_be_uint64, uint64_t)

define_py_col_has_next_sub(col_has_next_sub_be_uint8, uint8_t)
define_py_col_has_next_sub(col_has_next_sub_be_uint16, uint16_t)
define_py_col_has_next_sub(col_has_next_sub_be_uint32, uint32_t)
define_py_col_has_next_sub(col_has_next_sub_be_uint64, uint64_t)

define_py_col_has_prev(col_has_prev_be_uint8, uint8_t)
define_py_col_has_prev(col_has_prev_be_uint16, uint16_t)
define_py_col_has_prev(col_has_prev_be_uint32, uint32_t)
define_py_col_has_prev(col_has_prev_be_uint64, uint64_t)

define_py_col_has_prev_sub(col_has_prev_sub_be_uint8, uint8_t)
define_py_col_has_prev_sub(col_has_prev_sub_be_uint16, uint16_t)
define_py_col_has_prev_sub(col_has_prev_sub_be_uint32, uint32_t)
define_py_col_has_prev_sub(col_has_prev_sub_be_uint64, uint64_t)

define_py_col_find(col_find_first_le_uint8, uint8_t)
define_py_col_find(col_find_first_le_uint16, uint16_t)
define_py_col_find(col_find_first_le_uint32, uint32_t)
define_py_col_find(col_find_first_le_uint64, uint64_t)
define_py_col_find(col_find_last_le_uint8, uint8_t)
define_py_col_find(col_find_last_le_uint16, uint16_t)
define_py_col_find(col_find_last_le_uint32, uint32_t)
define_py_col_find(col_find_last_le_uint64, uint64_t)

define_py_col_find_sub(col_find_first_sub_le_uint8, uint8_t)
define_py_col_find_sub(col_find_first_sub_le_uint16, uint16_t)
define_py_col_find_sub(col_find_first_sub_le_uint32, uint32_t)
define_py_col_find_sub(col_find_first_sub_le_uint64, uint64_t)
define_py_col_find_sub(col_find_last_sub_le_uint8, uint8_t)
define_py_col_find_sub(col_find_last_sub_le_uint16, uint16_t)
define_py_col_find_sub(col_find_last_sub_le_uint32, uint32_t)
define_py_col_find_sub(col_find_last_sub_le_uint64, uint64_t)

define_py_col_has_next(col_has_next_le_uint8, uint8_t)
define_py_col_has_next(col_has_next_le_uint16, uint16_t)
define_py_col_has_next(col_has_next_le_uint32, uint32_t)
define_py_col_has_next(col_has_next_le_uint64, uint64_t)

define_py_col_has_next_sub(col_has_next_sub_le_uint8, uint8_t)
define_py_col_has_next_sub(col_has_next_sub_le_uint16, uint16_t)
define_py_col_has_next_sub(col_has_next_sub_le_uint32, uint32_t)
define_py_col_has_next_sub(col_has_next_sub_le_uint64, uint64_t)

define_py_col_has_prev(col_has_prev_le_uint8, uint8_t)
define_py_col_has_prev(col_has_prev_le_uint16, uint16_t)
define_py_col_has_prev(col_has_prev_le_uint32, uint32_t)
define_py_col_has_prev(col_has_prev_le_uint64, uint64_t)

define_py_col_has_prev_sub(col_has_prev_sub_le_uint8, uint8_t)
define_py_col_has_prev_sub(col_has_prev_sub_le_uint16, uint16_t)
define_py_col_has_prev_sub(col_has_prev_sub_le_uint32, uint32_t)
define_py_col_has_prev_sub(col_has_prev_sub_le_uint64, uint64_t)

/**
 * Define the wrapper of a column mode col_find_range function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find_range(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "first", "last", "search"}; \
    PyObject *argv[5]; \
    uint64_t offset = 0, first = 0, last = 0; \
    T search = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 5, 5, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &first) \
            || !convert_uint64_t(argv[3], &last) \
            || !convert_##T(argv[4], &search)) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(argv[0], offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t n = NAME##_t(src, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)n, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a column mode col_find_range_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find_range_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search"}; \
    PyObject *argv[7]; \
    uint64_t offset = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    T search = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 7, 7, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint8_t(argv[2], &bitstart) \
            || !convert_uint8_t(argv[3], &bitend) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &last) \
            || !convert_##T(argv[6], &search)) \
    { \
        return NULL; \
    } \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(argv[0], offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    uint64_t n = NAME##_t(src, bitstart, bitend, &first, &last, search); \
    return Py_BuildValue("KKK", (unsigned long long)n, (unsigned long long)first, (unsigned long long)last); \
}

/**
 * Define the wrapper of a column mode col_find_many function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find_many(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "first", "last", "search", "prefetch"}; \
    PyObject *argv[6]; \
    uint64_t offset = 0, first = 0, last = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 6, 5, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint64_t(argv[2], &first) \
            || !convert_uint64_t(argv[3], &last)) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(argv[0], offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    binsearch_prefetch_t prefetch = BINSEARCH_PREFETCH_AUTO; \
    if (!convert_prefetch(argv[5], &prefetch)) \
    { \
        return NULL; \
    } \
    T *search = NULL; \
    uint64_t *pos = NULL; \
    Py_ssize_t count = take_search(argv[4], sizeof(T), &pos, (void **)&search); \
    if (count < 0) \
    { \
        return NULL; \
    } \
    NAME##_t(src, first, last, search, pos, (uint64_t)count, prefetch); \
    return build_pos_list(pos, count); \
}

/**
 * Define the wrapper of a column mode col_find_many_sub function.
 *
 * @param NAME Name of the wrapped function without the "_t" suffix.
 * @param T    Unsigned integer type, one of: uint8_t, uint16_t, uint32_t, uint64_t.
 */
#define define_py_col_find_many_sub(NAME, T) \
static PyObject* py_##NAME(PyObject *Py_UNUSED(ignored), PyObject *const *argv_in, Py_ssize_t nargs, PyObject *kwnames) \
{ \
    static const char *const names[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", "prefetch"}; \
    PyObject *argv[8]; \
    uint64_t offset = 0, first = 0, last = 0; \
    uint8_t bitstart = 0, bitend = 0; \
    if (!parse_args(argv_in, nargs, kwnames, names, 8, 7, argv)) \
    { \
        return NULL; \
    } \
    if (!convert_uint64_t(argv[1], &offset) \
            || !convert_uint8_t(argv[2], &bitstart) \
            || !convert_uint8_t(argv[3], &bitend) \
            || !convert_uint64_t(argv[4], &first) \
            || !convert_uint64_t(argv[5], &last)) \
    { \
        return NULL; \
    } \
    if (!check_bits(bitstart, bitend, sizeof(T))) \
    { \
        return NULL; \
    } \
    const T *src = (const T *)get_col_src(argv[0], offset, last, sizeof(T)); \
    if (src == NULL) \
    { \
        return NULL; \
    } \
    binsearch_prefetch_t prefetch = BINSEARCH_PREFETCH_AUTO; \
    if (!convert_prefetch(argv[7], &prefetch)) \
    { \
        return NULL; \
    } \
    T *search = NULL; \
    uint64_t *pos = NULL; \
    Py_ssize_t count = take_search(argv[6], sizeof(T), &pos, (void **)&search); \
    if (count < 0) \
    { \
        return NULL; \
    } \
    NAME##_t(src, bitstart, bitend, first, last, search, pos, (uint64_t)count, prefetch); \
    return build_pos_list(pos, count); \
}

define_py_col_find_range(col_find_range_be_uint8, uint8_t)
define_py_col_find_range(col_find_range_be_uint16, uint16_t)
define_py_col_find_range(col_find_range_be_uint32, uint32_t)
define_py_col_find_range(col_find_range_be_uint64, uint64_t)
define_py_col_find_range(col_find_range_le_uint8, uint8_t)
define_py_col_find_range(col_find_range_le_uint16, uint16_t)
define_py_col_find_range(col_find_range_le_uint32, uint32_t)
define_py_col_find_range(col_find_range_le_uint64, uint64_t)

define_py_col_find_range_sub(col_find_range_sub_be_uint8, uint8_t)
define_py_col_find_range_sub(col_find_range_sub_be_uint16, uint16_t)
define_py_col_find_range_sub(col_find_range_sub_be_uint32, uint32_t)
define_py_col_find_range_sub(col_find_range_sub_be_uint64, uint64_t)
define_py_col_find_range_sub(col_find_range_sub_le_uint8, uint8_t)
define_py_col_find_range_sub(col_find_range_sub_le_uint16, uint16_t)
define_py_col_find_range_sub(col_find_range_sub_le_uint32, uint32_t)
define_py_col_find_range_sub(col_find_range_sub_le_uint64, uint64_t)

define_py_col_find_many(col_find_many_be_uint8, uint8_t)
define_py_col_find_many(col_find_many_be_uint16, uint16_t)
define_py_col_find_many(col_find_many_be_uint32, uint32_t)
define_py_col_find_many(col_find_many_be_uint64, uint64_t)
define_py_col_find_many(col_find_many_le_uint8, uint8_t)
define_py_col_find_many(col_find_many_le_uint16, uint16_t)
define_py_col_find_many(col_find_many_le_uint32, uint32_t)
define_py_col_find_many(col_find_many_le_uint64, uint64_t)

define_py_col_find_many_sub(col_find_many_sub_be_uint8, uint8_t)
define_py_col_find_many_sub(col_find_many_sub_be_uint16, uint16_t)
define_py_col_find_many_sub(col_find_many_sub_be_uint32, uint32_t)
define_py_col_find_many_sub(col_find_many_sub_be_uint64, uint64_t)
define_py_col_find_many_sub(col_find_many_sub_le_uint8, uint8_t)
define_py_col_find_many_sub(col_find_many_sub_le_uint16, uint16_t)
define_py_col_find_many_sub(col_find_many_sub_le_uint32, uint32_t)
define_py_col_find_many_sub(col_find_many_sub_le_uint64, uint64_t)

// --- MODULE ---

// Add an entry of the method table of the module.
#define PY_METHOD(NAME, DOC) {#NAME, (PyCFunction)(void(*)(void))py_##NAME, METH_FASTCALL|METH_KEYWORDS, DOC},

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

    PY_METHOD(find_range_be_uint8, PYFINDRANGEUINT8_DOCSTRING)
    PY_METHOD(find_range_be_uint16, PYFINDRANGEUINT16_DOCSTRING)
    PY_METHOD(find_range_be_uint32, PYFINDRANGEUINT32_DOCSTRING)
    PY_METHOD(find_range_be_uint64, PYFINDRANGEUINT64_DOCSTRING)

    PY_METHOD(find_range_sub_be_uint8, PYFINDRANGESUBUINT8_DOCSTRING)
    PY_METHOD(find_range_sub_be_uint16, PYFINDRANGESUBUINT16_DOCSTRING)
    PY_METHOD(find_range_sub_be_uint32, PYFINDRANGESUBUINT32_DOCSTRING)
    PY_METHOD(find_range_sub_be_uint64, PYFINDRANGESUBUINT64_DOCSTRING)

    PY_METHOD(find_many_be_uint8, PYFINDMANYUINT8_DOCSTRING)
    PY_METHOD(find_many_be_uint16, PYFINDMANYUINT16_DOCSTRING)
    PY_METHOD(find_many_be_uint32, PYFINDMANYUINT32_DOCSTRING)
    PY_METHOD(find_many_be_uint64, PYFINDMANYUINT64_DOCSTRING)

    PY_METHOD(find_many_sub_be_uint8, PYFINDMANYSUBUINT8_DOCSTRING)
    PY_METHOD(find_many_sub_be_uint16, PYFINDMANYSUBUINT16_DOCSTRING)
    PY_METHOD(find_many_sub_be_uint32, PYFINDMANYSUBUINT32_DOCSTRING)
    PY_METHOD(find_many_sub_be_uint64, PYFINDMANYSUBUINT64_DOCSTRING)

    PY_METHOD(find_range_le_uint8, PYFINDRANGEUINT8_DOCSTRING)
    PY_METHOD(find_range_le_uint16, PYFINDRANGEUINT16_DOCSTRING)
    PY_METHOD(find_range_le_uint32, PYFINDRANGEUINT32_DOCSTRING)
    PY_METHOD(find_range_le_uint64, PYFINDRANGEUINT64_DOCSTRING)

    PY_METHOD(find_range_sub_le_uint8, PYFINDRANGESUBUINT8_DOCSTRING)
    PY_METHOD(find_range_sub_le_uint16, PYFINDRANGESUBUINT16_DOCSTRING)
    PY_METHOD(find_range_sub_le_uint32, PYFINDRANGESUBUINT32_DOCSTRING)
    PY_METHOD(find_range_sub_le_uint64, PYFINDRANGESUBUINT64_DOCSTRING)

    PY_METHOD(find_many_le_uint8, PYFINDMANYUINT8_DOCSTRING)
    PY_METHOD(find_many_le_uint16, PYFINDMANYUINT16_DOCSTRING)
    PY_METHOD(find_many_le_uint32, PYFINDMANYUINT32_DOCSTRING)
    PY_METHOD(find_many_le_uint64, PYFINDMANYUINT64_DOCSTRING)

    PY_METHOD(find_many_sub_le_uint8, PYFINDMANYSUBUINT8_DOCSTRING)
    PY_METHOD(find_many_sub_le_uint16, PYFINDMANYSUBUINT16_DOCSTRING)
    PY_METHOD(find_many_sub_le_uint32, PYFINDMANYSUBUINT32_DOCSTRING)
    PY_METHOD(find_many_sub_le_uint64, PYFINDMANYSUBUINT64_DOCSTRING)


    PY_METHOD(col_find_first_be_uint8, PYCOLFINDFIRSTUINT8_DOCSTRING)
    PY_METHOD(col_find_first_be_uint16, PYCOLFINDFIRSTUINT16_DOCSTRING)
    PY_METHOD(col_find_first_be_uint32, PYCOLFINDFIRSTUINT32_DOCSTRING)
    PY_METHOD(col_find_first_be_uint64, PYCOLFINDFIRSTUINT64_DOCSTRING)
    PY_METHOD(col_find_last_be_uint8, PYCOLFINDLASTUINT8_DOCSTRING)
    PY_METHOD(col_find_last_be_uint16, PYCOLFINDLASTUINT16_DOCSTRING)
    PY_METHOD(col_find_last_be_uint32, PYCOLFINDLASTUINT32_DOCSTRING)
    PY_METHOD(col_find_last_be_uint64, PYCOLFINDLASTUINT64_DOCSTRING)
    PY_METHOD(col_find_first_sub_be_uint8, PYCOLFINDFIRSTSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_first_sub_be_uint16, PYCOLFINDFIRSTSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_first_sub_be_uint32, PYCOLFINDFIRSTSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_first_sub_be_uint64, PYCOLFINDFIRSTSUBUINT64_DOCSTRING)
    PY_METHOD(col_find_last_sub_be_uint8, PYCOLFINDLASTSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_last_sub_be_uint16, PYCOLFINDLASTSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_last_sub_be_uint32, PYCOLFINDLASTSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_last_sub_be_uint64, PYCOLFINDLASTSUBUINT64_DOCSTRING)
    PY_METHOD(col_has_next_be_uint8, PYCOLHASNEXTUINT8_DOCSTRING)
    PY_METHOD(col_has_next_be_uint16, PYCOLHASNEXTUINT16_DOCSTRING)
    PY_METHOD(col_has_next_be_uint32, PYCOLHASNEXTUINT32_DOCSTRING)
    PY_METHOD(col_has_next_be_uint64, PYCOLHASNEXTUINT64_DOCSTRING)
    PY_METHOD(col_has_next_sub_be_uint8, PYCOLHASNEXTSUBUINT8_DOCSTRING)
    PY_METHOD(col_has_next_sub_be_uint16, PYCOLHASNEXTSUBUINT16_DOCSTRING)
    PY_METHOD(col_has_next_sub_be_uint32, PYCOLHASNEXTSUBUINT32_DOCSTRING)
    PY_METHOD(col_has_next_sub_be_uint64, PYCOLHASNEXTSUBUINT64_DOCSTRING)
    PY_METHOD(col_has_prev_be_uint8, PYCOLHASPREVUINT8_DOCSTRING)
    PY_METHOD(col_has_prev_be_uint16, PYCOLHASPREVUINT16_DOCSTRING)
    PY_METHOD(col_has_prev_be_uint32, PYCOLHASPREVUINT32_DOCSTRING)
    PY_METHOD(col_has_prev_be_uint64, PYCOLHASPREVUINT64_DOCSTRING)
    PY_METHOD(col_has_prev_sub_be_uint8, PYCOLHASPREVSUBUINT8_DOCSTRING)
    PY_METHOD(col_has_prev_sub_be_uint16, PYCOLHASPREVSUBUINT16_DOCSTRING)
    PY_METHOD(col_has_prev_sub_be_uint32, PYCOLHASPREVSUBUINT32_DOCSTRING)
    PY_METHOD(col_has_prev_sub_be_uint64, PYCOLHASPREVSUBUINT64_DOCSTRING)

    PY_METHOD(col_find_first_le_uint8, PYCOLFINDFIRSTUINT8_DOCSTRING)
    PY_METHOD(col_find_first_le_uint16, PYCOLFINDFIRSTUINT16_DOCSTRING)
    PY_METHOD(col_find_first_le_uint32, PYCOLFINDFIRSTUINT32_DOCSTRING)
    PY_METHOD(col_find_first_le_uint64, PYCOLFINDFIRSTUINT64_DOCSTRING)
    PY_METHOD(col_find_last_le_uint8, PYCOLFINDLASTUINT8_DOCSTRING)
    PY_METHOD(col_find_last_le_uint16, PYCOLFINDLASTUINT16_DOCSTRING)
    PY_METHOD(col_find_last_le_uint32, PYCOLFINDLASTUINT32_DOCSTRING)
    PY_METHOD(col_find_last_le_uint64, PYCOLFINDLASTUINT64_DOCSTRING)
    PY_METHOD(col_find_first_sub_le_uint8, PYCOLFINDFIRSTSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_first_sub_le_uint16, PYCOLFINDFIRSTSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_first_sub_le_uint32, PYCOLFINDFIRSTSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_first_sub_le_uint64, PYCOLFINDFIRSTSUBUINT64_DOCSTRING)
    PY_METHOD(col_find_last_sub_le_uint8, PYCOLFINDLASTSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_last_sub_le_uint16, PYCOLFINDLASTSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_last_sub_le_uint32, PYCOLFINDLASTSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_last_sub_le_uint64, PYCOLFINDLASTSUBUINT64_DOCSTRING)
    PY_METHOD(col_has_next_le_uint8, PYCOLHASNEXTUINT8_DOCSTRING)
    PY_METHOD(col_has_next_le_uint16, PYCOLHASNEXTUINT16_DOCSTRING)
    PY_METHOD(col_has_next_le_uint32, PYCOLHASNEXTUINT32_DOCSTRING)
    PY_METHOD(col_has_next_le_uint64, PYCOLHASNEXTUINT64_DOCSTRING)
    PY_METHOD(col_has_next_sub_le_uint8, PYCOLHASNEXTSUBUINT8_DOCSTRING)
    PY_METHOD(col_has_next_sub_le_uint16, PYCOLHASNEXTSUBUINT16_DOCSTRING)
    PY_METHOD(col_has_next_sub_le_uint32, PYCOLHASNEXTSUBUINT32_DOCSTRING)
    PY_METHOD(col_has_next_sub_le_uint64, PYCOLHASNEXTSUBUINT64_DOCSTRING)
    PY_METHOD(col_has_prev_le_uint8, PYCOLHASPREVUINT8_DOCSTRING)
    PY_METHOD(col_has_prev_le_uint16, PYCOLHASPREVUINT16_DOCSTRING)
    PY_METHOD(col_has_prev_le_uint32, PYCOLHASPREVUINT32_DOCSTRING)
    PY_METHOD(col_has_prev_le_uint64, PYCOLHASPREVUINT64_DOCSTRING)
    PY_METHOD(col_has_prev_sub_le_uint8, PYCOLHASPREVSUBUINT8_DOCSTRING)
    PY_METHOD(col_has_prev_sub_le_uint16, PYCOLHASPREVSUBUINT16_DOCSTRING)
    PY_METHOD(col_has_prev_sub_le_uint32, PYCOLHASPREVSUBUINT32_DOCSTRING)
    PY_METHOD(col_has_prev_sub_le_uint64, PYCOLHASPREVSUBUINT64_DOCSTRING)

    PY_METHOD(col_find_range_be_uint8, PYCOLFINDRANGEUINT8_DOCSTRING)
    PY_METHOD(col_find_range_be_uint16, PYCOLFINDRANGEUINT16_DOCSTRING)
    PY_METHOD(col_find_range_be_uint32, PYCOLFINDRANGEUINT32_DOCSTRING)
    PY_METHOD(col_find_range_be_uint64, PYCOLFINDRANGEUINT64_DOCSTRING)

    PY_METHOD(col_find_range_sub_be_uint8, PYCOLFINDRANGESUBUINT8_DOCSTRING)
    PY_METHOD(col_find_range_sub_be_uint16, PYCOLFINDRANGESUBUINT16_DOCSTRING)
    PY_METHOD(col_find_range_sub_be_uint32, PYCOLFINDRANGESUBUINT32_DOCSTRING)
    PY_METHOD(col_find_range_sub_be_uint64, PYCOLFINDRANGESUBUINT64_DOCSTRING)

    PY_METHOD(col_find_many_be_uint8, PYCOLFINDMANYUINT8_DOCSTRING)
    PY_METHOD(col_find_many_be_uint16, PYCOLFINDMANYUINT16_DOCSTRING)
    PY_METHOD(col_find_many_be_uint32, PYCOLFINDMANYUINT32_DOCSTRING)
    PY_METHOD(col_find_many_be_uint64, PYCOLFINDMANYUINT64_DOCSTRING)

    PY_METHOD(col_find_many_sub_be_uint8, PYCOLFINDMANYSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_many_sub_be_uint16, PYCOLFINDMANYSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_many_sub_be_uint32, PYCOLFINDMANYSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_many_sub_be_uint64, PYCOLFINDMANYSUBUINT64_DOCSTRING)

    PY_METHOD(col_find_range_le_uint8, PYCOLFINDRANGEUINT8_DOCSTRING)
    PY_METHOD(col_find_range_le_uint16, PYCOLFINDRANGEUINT16_DOCSTRING)
    PY_METHOD(col_find_range_le_uint32, PYCOLFINDRANGEUINT32_DOCSTRING)
    PY_METHOD(col_find_range_le_uint64, PYCOLFINDRANGEUINT64_DOCSTRING)

    PY_METHOD(col_find_range_sub_le_uint8, PYCOLFINDRANGESUBUINT8_DOCSTRING)
    PY_METHOD(col_find_range_sub_le_uint16, PYCOLFINDRANGESUBUINT16_DOCSTRING)
    PY_METHOD(col_find_range_sub_le_uint32, PYCOLFINDRANGESUBUINT32_DOCSTRING)
    PY_METHOD(col_find_range_sub_le_uint64, PYCOLFINDRANGESUBUINT64_DOCSTRING)

    PY_METHOD(col_find_many_le_uint8, PYCOLFINDMANYUINT8_DOCSTRING)
    PY_METHOD(col_find_many_le_uint16, PYCOLFINDMANYUINT16_DOCSTRING)
    PY_METHOD(col_find_many_le_uint32, PYCOLFINDMANYUINT32_DOCSTRING)
    PY_METHOD(col_find_many_le_uint64, PYCOLFINDMANYUINT64_DOCSTRING)

    PY_METHOD(col_find_many_sub_le_uint8, PYCOLFINDMANYSUBUINT8_DOCSTRING)
    PY_METHOD(col_find_many_sub_le_uint16, PYCOLFINDMANYSUBUINT16_DOCSTRING)
    PY_METHOD(col_find_many_sub_le_uint32, PYCOLFINDMANYSUBUINT32_DOCSTRING)
    PY_METHOD(col_find_many_sub_le_uint64, PYCOLFINDMANYSUBUINT64_DOCSTRING)


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
    PyObject *module = PyModule_Create(&moduledef);
    if (module != NULL) // LCOV_EXCL_BR_LINE : out of memory
    {
        // The values of binsearch_prefetch_t, for the optional "prefetch"
        // argument of the find_many functions.
        (void)PyModule_AddIntConstant(module, "PREFETCH_AUTO", (long)BINSEARCH_PREFETCH_AUTO);
        (void)PyModule_AddIntConstant(module, "PREFETCH_ALWAYS", (long)BINSEARCH_PREFETCH_ALWAYS);
        (void)PyModule_AddIntConstant(module, "PREFETCH_NEVER", (long)BINSEARCH_PREFETCH_NEVER);
    }
#ifdef Py_GIL_DISABLED
    // The module keeps no state of its own and the search functions only read
    // the mapping, so a free-threaded interpreter does not have to hold the GIL
    // for it. Unmapping a file while a search reads it is a data race, as it is
    // on an interpreter that holds the GIL.
    if (module != NULL) // LCOV_EXCL_BR_LINE : out of memory
    {
        PyUnstable_Module_SetGIL(module, Py_MOD_GIL_NOT_USED);
    }
#endif
    return module;
}
