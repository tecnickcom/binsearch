// Python binsearch Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@tecnick.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/binsearch

#define MODULE_NAME "binsearch"

#include <Python.h>
#include "../../c/src/binsearch/binsearch.h"
#include "pybinsearch.h"

#ifndef Py_UNUSED // This is already defined for Python 3.4 onwards
#ifdef __GNUC__
#define Py_UNUSED(name) _unused_ ## name __attribute__((unused))
#else
#define Py_UNUSED(name) _unused_ ## name
#endif
#endif

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    const char *file;
    PyObject* ctbytes = NULL;
    static char *kwlist[] = {"file", "ctbytes", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "sO", kwlist, &file, &ctbytes))
        return NULL;
    mmfile_t h;
    ctbytes = PySequence_Fast(ctbytes, "argument must be iterable");
    if (!ctbytes)
    {
        return 0;
    }
    h.ncols = (uint8_t)PySequence_Fast_GET_SIZE(ctbytes);
    int i;
    for(i = 0; i < h.ncols; i++)
    {
        PyObject *fitem;
        PyObject *item = PySequence_Fast_GET_ITEM(ctbytes, i);
        if (!item)
        {
            Py_DECREF(ctbytes);
            return 0;
        }
        fitem = PyNumber_Long(item);
        if (!fitem)
        {
            Py_DECREF(ctbytes);
            return 0;
        }
        h.ctbytes[i] = (uint8_t)PyLong_AsUnsignedLong(item);
        Py_DECREF(fitem);
    }
    Py_DECREF(ctbytes);
    mmap_binfile(file, &h);
    PyObject* index = PyList_New(0);
    for (i = 0; i < h.ncols; i++)
    {
        PyList_Append(index, Py_BuildValue("K", h.index[i]));
    }
    result = PyTuple_New(9);
    PyTuple_SetItem(result, 0, PyCapsule_New((void*)h.src, "src", NULL));
    PyTuple_SetItem(result, 1, Py_BuildValue("i", h.fd));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.size));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", h.doffset));
    PyTuple_SetItem(result, 4, Py_BuildValue("K", h.dlength));
    PyTuple_SetItem(result, 5, Py_BuildValue("K", h.nrows));
    PyTuple_SetItem(result, 6, Py_BuildValue("B", h.ncols));
    PyTuple_SetItem(result, 7, index);
    PyTuple_SetItem(result, 8, PyCapsule_New((void*)h.index, "index", NULL));
    return result;
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    mmfile_t mf;
    PyObject *result;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "fd", "size", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OiK", kwlist, &mfsrc, &mf.fd, &mf.size))
        return NULL;
    mf.src = (uint8_t *)PyCapsule_GetPointer(mfsrc, "src");
    int h = munmap_binfile(mf);
    result = Py_BuildValue("i", h);
    return result;
}

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, item;
    static char *kwlist[] = {"blklen", "blkpos", "item", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "KKK", kwlist, &blklen, &blkpos, &item))
        return NULL;
    uint64_t h = get_address(blklen, blkpos, item);
    result = Py_BuildValue("K", h);
    return result;
}

// ----------

static PyObject* py_find_first_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_be_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_be_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_be_uint32_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_be_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_last_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_be_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_be_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_be_uint32_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_be_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_first_sub_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_be_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_be_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_be_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_be_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_last_sub_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_be_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_be_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_be_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_be_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_has_next_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_be_uint8_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_be_uint16_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_be_uint32_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_be_uint64_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_next_sub_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_be_uint8_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_be_uint16_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_be_uint32_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_be_uint64_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_prev_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_be_uint8_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_be_uint16_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_be_uint32_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_be_uint64_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_prev_sub_be_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_be_uint8_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_be_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_be_uint16_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_be_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_be_uint32_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_be_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_be_uint64_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

// ----------

static PyObject* py_find_first_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_le_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_le_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_le_uint32_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_le_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_last_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_le_uint8_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_le_uint16_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_le_uint32_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_le_uint64_t(src, blklen, blkpos, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_first_sub_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_le_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_le_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_le_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_sub_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_first_sub_le_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_find_last_sub_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_le_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_le_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_le_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_sub_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = find_last_sub_le_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_has_next_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_le_uint8_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_le_uint16_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_le_uint32_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_le_uint64_t(src, blklen, blkpos, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_next_sub_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_le_uint8_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_le_uint16_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_le_uint32_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_next_sub_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, pos, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_next_sub_le_uint64_t(src, blklen, blkpos, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_prev_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_le_uint8_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_le_uint16_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_le_uint32_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_le_uint64_t(src, blklen, blkpos, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_has_prev_sub_le_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKB", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_le_uint8_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_le_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKH", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_le_uint16_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_le_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKI", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_le_uint32_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_has_prev_sub_le_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, pos;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKBBKKK", kwlist, &mfsrc, &offset, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = has_prev_sub_le_uint64_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

// --- COLUMN ---

static PyObject* py_col_find_first_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKB", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_uint8_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_first_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKH", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_uint16_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_first_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKI", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_uint32_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_first_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKK", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_uint64_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_col_find_last_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKB", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_uint8_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_last_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKH", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_uint16_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_last_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKI", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_uint32_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_last_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKK", kwlist, &mfsrc, &offset, &first, &last, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_uint64_t(src, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_col_find_first_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKB", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_sub_uint8_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_first_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKH", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_sub_uint16_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_first_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKI", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_sub_uint32_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_first_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKK", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_first_sub_uint64_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_col_find_last_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKB", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_sub_uint8_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_last_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKH", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_sub_uint16_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_last_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKI", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_sub_uint32_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_col_find_last_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKK", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    uint64_t h = col_find_last_sub_uint64_t(src, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

// ----------

static PyObject* py_col_has_next_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKB", kwlist, &mfsrc, &offset, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_uint8_t(src, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_next_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKH", kwlist, &mfsrc, &offset, &pos, &last, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_uint16_t(src, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_next_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKI", kwlist, &mfsrc, &offset, &pos, &last, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_uint32_t(src, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_next_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKK", kwlist, &mfsrc, &offset, &pos, &last, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_uint64_t(src, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_col_has_next_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKB", kwlist, &mfsrc, &offset, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_sub_uint8_t(src, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_next_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKH", kwlist, &mfsrc, &offset, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_sub_uint16_t(src, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_next_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKI", kwlist, &mfsrc, &offset, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_sub_uint32_t(src, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_next_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t pos, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKK", kwlist, &mfsrc, &offset, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_next_sub_uint64_t(src, bitstart, bitend, &pos, last, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_col_has_prev_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKB", kwlist, &mfsrc, &offset, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_uint8_t(src, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_prev_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKH", kwlist, &mfsrc, &offset, &first, &pos, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_uint16_t(src, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_prev_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKI", kwlist, &mfsrc, &offset, &first, &pos, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_uint32_t(src, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_prev_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKK", kwlist, &mfsrc, &offset, &first, &pos, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_uint64_t(src, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyObject* py_col_has_prev_sub_uint8(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint8_t bitstart, bitend;
    uint8_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKB", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint8_t *src = (const uint8_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_sub_uint8_t(src, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_prev_sub_uint16(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint8_t bitstart, bitend;
    uint16_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKH", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint16_t *src = (const uint16_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_sub_uint16_t(src, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_prev_sub_uint32(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint8_t bitstart, bitend;
    uint32_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKI", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint32_t *src = (const uint32_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_sub_uint32_t(src, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

static PyObject* py_col_has_prev_sub_uint64(PyObject *Py_UNUSED(ignored), PyObject *args, PyObject *keywds)
{
    PyObject *result;
    uint64_t first, pos;
    uint8_t bitstart, bitend;
    uint64_t search;
    uint64_t offset;
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "offset", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKBBKKK", kwlist, &mfsrc, &offset, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const uint64_t *src = (const uint64_t *)((const uint8_t *)PyCapsule_GetPointer(mfsrc, "src") + offset);
    bool h = col_has_prev_sub_uint64_t(src, bitstart, bitend, first, &pos, search);
    result = PyTuple_New(2);
    PyTuple_SetItem(result, 0, Py_BuildValue("i", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", pos));
    return result;
}

// ----------

static PyMethodDef PyBinsearchMethods[] =
{
    {"mmap_binfile", (PyCFunction)py_mmap_binfile, METH_VARARGS|METH_KEYWORDS, PYMMAPBINFILE_DOCSTRING},
    {"munmap_binfile", (PyCFunction)py_munmap_binfile, METH_VARARGS|METH_KEYWORDS, PYMUNMAPBINFILE_DOCSTRING},
    {"get_address", (PyCFunction)py_get_address, METH_VARARGS|METH_KEYWORDS, PYGETADDRESS_DOCSTRING},
    {"find_first_be_uint8", (PyCFunction)py_find_first_be_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT8_DOCSTRING},
    {"find_first_be_uint16", (PyCFunction)py_find_first_be_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT16_DOCSTRING},
    {"find_first_be_uint32", (PyCFunction)py_find_first_be_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT32_DOCSTRING},
    {"find_first_be_uint64", (PyCFunction)py_find_first_be_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT64_DOCSTRING},
    {"find_last_be_uint8", (PyCFunction)py_find_last_be_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT8_DOCSTRING},
    {"find_last_be_uint16", (PyCFunction)py_find_last_be_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT16_DOCSTRING},
    {"find_last_be_uint32", (PyCFunction)py_find_last_be_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT32_DOCSTRING},
    {"find_last_be_uint64", (PyCFunction)py_find_last_be_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT64_DOCSTRING},
    {"find_first_sub_be_uint8", (PyCFunction)py_find_first_sub_be_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT8_DOCSTRING},
    {"find_first_sub_be_uint16", (PyCFunction)py_find_first_sub_be_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT16_DOCSTRING},
    {"find_first_sub_be_uint32", (PyCFunction)py_find_first_sub_be_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT32_DOCSTRING},
    {"find_first_sub_be_uint64", (PyCFunction)py_find_first_sub_be_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT64_DOCSTRING},
    {"find_last_sub_be_uint8", (PyCFunction)py_find_last_sub_be_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT8_DOCSTRING},
    {"find_last_sub_be_uint16", (PyCFunction)py_find_last_sub_be_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT16_DOCSTRING},
    {"find_last_sub_be_uint32", (PyCFunction)py_find_last_sub_be_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT32_DOCSTRING},
    {"find_last_sub_be_uint64", (PyCFunction)py_find_last_sub_be_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT64_DOCSTRING},
    {"has_next_be_uint8", (PyCFunction)py_has_next_be_uint8, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT8_DOCSTRING},
    {"has_next_be_uint16", (PyCFunction)py_has_next_be_uint16, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT16_DOCSTRING},
    {"has_next_be_uint32", (PyCFunction)py_has_next_be_uint32, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT32_DOCSTRING},
    {"has_next_be_uint64", (PyCFunction)py_has_next_be_uint64, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT64_DOCSTRING},
    {"has_next_sub_be_uint8", (PyCFunction)py_has_next_sub_be_uint8, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT8_DOCSTRING},
    {"has_next_sub_be_uint16", (PyCFunction)py_has_next_sub_be_uint16, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT16_DOCSTRING},
    {"has_next_sub_be_uint32", (PyCFunction)py_has_next_sub_be_uint32, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT32_DOCSTRING},
    {"has_next_sub_be_uint64", (PyCFunction)py_has_next_sub_be_uint64, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT64_DOCSTRING},
    {"has_prev_be_uint8", (PyCFunction)py_has_prev_be_uint8, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT8_DOCSTRING},
    {"has_prev_be_uint16", (PyCFunction)py_has_prev_be_uint16, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT16_DOCSTRING},
    {"has_prev_be_uint32", (PyCFunction)py_has_prev_be_uint32, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT32_DOCSTRING},
    {"has_prev_be_uint64", (PyCFunction)py_has_prev_be_uint64, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT64_DOCSTRING},
    {"has_prev_sub_be_uint8", (PyCFunction)py_has_prev_sub_be_uint8, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT8_DOCSTRING},
    {"has_prev_sub_be_uint16", (PyCFunction)py_has_prev_sub_be_uint16, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT16_DOCSTRING},
    {"has_prev_sub_be_uint32", (PyCFunction)py_has_prev_sub_be_uint32, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT32_DOCSTRING},
    {"has_prev_sub_be_uint64", (PyCFunction)py_has_prev_sub_be_uint64, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT64_DOCSTRING},
    {"find_first_le_uint8", (PyCFunction)py_find_first_le_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT8_DOCSTRING},
    {"find_first_le_uint16", (PyCFunction)py_find_first_le_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT16_DOCSTRING},
    {"find_first_le_uint32", (PyCFunction)py_find_first_le_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT32_DOCSTRING},
    {"find_first_le_uint64", (PyCFunction)py_find_first_le_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTUINT64_DOCSTRING},
    {"find_last_le_uint8", (PyCFunction)py_find_last_le_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT8_DOCSTRING},
    {"find_last_le_uint16", (PyCFunction)py_find_last_le_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT16_DOCSTRING},
    {"find_last_le_uint32", (PyCFunction)py_find_last_le_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT32_DOCSTRING},
    {"find_last_le_uint64", (PyCFunction)py_find_last_le_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDLASTUINT64_DOCSTRING},
    {"find_first_sub_le_uint8", (PyCFunction)py_find_first_sub_le_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT8_DOCSTRING},
    {"find_first_sub_le_uint16", (PyCFunction)py_find_first_sub_le_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT16_DOCSTRING},
    {"find_first_sub_le_uint32", (PyCFunction)py_find_first_sub_le_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT32_DOCSTRING},
    {"find_first_sub_le_uint64", (PyCFunction)py_find_first_sub_le_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDFIRSTSUBUINT64_DOCSTRING},
    {"find_last_sub_le_uint8", (PyCFunction)py_find_last_sub_le_uint8, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT8_DOCSTRING},
    {"find_last_sub_le_uint16", (PyCFunction)py_find_last_sub_le_uint16, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT16_DOCSTRING},
    {"find_last_sub_le_uint32", (PyCFunction)py_find_last_sub_le_uint32, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT32_DOCSTRING},
    {"find_last_sub_le_uint64", (PyCFunction)py_find_last_sub_le_uint64, METH_VARARGS|METH_KEYWORDS, PYFINDLASTSUBUINT64_DOCSTRING},
    {"has_next_le_uint8", (PyCFunction)py_has_next_le_uint8, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT8_DOCSTRING},
    {"has_next_le_uint16", (PyCFunction)py_has_next_le_uint16, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT16_DOCSTRING},
    {"has_next_le_uint32", (PyCFunction)py_has_next_le_uint32, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT32_DOCSTRING},
    {"has_next_le_uint64", (PyCFunction)py_has_next_le_uint64, METH_VARARGS|METH_KEYWORDS, PYHASNEXTUINT64_DOCSTRING},
    {"has_next_sub_le_uint8", (PyCFunction)py_has_next_sub_le_uint8, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT8_DOCSTRING},
    {"has_next_sub_le_uint16", (PyCFunction)py_has_next_sub_le_uint16, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT16_DOCSTRING},
    {"has_next_sub_le_uint32", (PyCFunction)py_has_next_sub_le_uint32, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT32_DOCSTRING},
    {"has_next_sub_le_uint64", (PyCFunction)py_has_next_sub_le_uint64, METH_VARARGS|METH_KEYWORDS, PYHASNEXTSUBUINT64_DOCSTRING},
    {"has_prev_le_uint8", (PyCFunction)py_has_prev_le_uint8, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT8_DOCSTRING},
    {"has_prev_le_uint16", (PyCFunction)py_has_prev_le_uint16, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT16_DOCSTRING},
    {"has_prev_le_uint32", (PyCFunction)py_has_prev_le_uint32, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT32_DOCSTRING},
    {"has_prev_le_uint64", (PyCFunction)py_has_prev_le_uint64, METH_VARARGS|METH_KEYWORDS, PYHASPREVUINT64_DOCSTRING},
    {"has_prev_sub_le_uint8", (PyCFunction)py_has_prev_sub_le_uint8, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT8_DOCSTRING},
    {"has_prev_sub_le_uint16", (PyCFunction)py_has_prev_sub_le_uint16, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT16_DOCSTRING},
    {"has_prev_sub_le_uint32", (PyCFunction)py_has_prev_sub_le_uint32, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT32_DOCSTRING},
    {"has_prev_sub_le_uint64", (PyCFunction)py_has_prev_sub_le_uint64, METH_VARARGS|METH_KEYWORDS, PYHASPREVSUBUINT64_DOCSTRING},
    {"col_find_first_uint8", (PyCFunction)py_col_find_first_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTUINT8_DOCSTRING},
    {"col_find_first_uint16", (PyCFunction)py_col_find_first_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTUINT16_DOCSTRING},
    {"col_find_first_uint32", (PyCFunction)py_col_find_first_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTUINT32_DOCSTRING},
    {"col_find_first_uint64", (PyCFunction)py_col_find_first_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTUINT64_DOCSTRING},
    {"col_find_last_uint8", (PyCFunction)py_col_find_last_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTUINT8_DOCSTRING},
    {"col_find_last_uint16", (PyCFunction)py_col_find_last_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTUINT16_DOCSTRING},
    {"col_find_last_uint32", (PyCFunction)py_col_find_last_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTUINT32_DOCSTRING},
    {"col_find_last_uint64", (PyCFunction)py_col_find_last_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTUINT64_DOCSTRING},
    {"col_find_first_sub_uint8", (PyCFunction)py_col_find_first_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTSUBUINT8_DOCSTRING},
    {"col_find_first_sub_uint16", (PyCFunction)py_col_find_first_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTSUBUINT16_DOCSTRING},
    {"col_find_first_sub_uint32", (PyCFunction)py_col_find_first_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTSUBUINT32_DOCSTRING},
    {"col_find_first_sub_uint64", (PyCFunction)py_col_find_first_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLFINDFIRSTSUBUINT64_DOCSTRING},
    {"col_find_last_sub_uint8", (PyCFunction)py_col_find_last_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTSUBUINT8_DOCSTRING},
    {"col_find_last_sub_uint16", (PyCFunction)py_col_find_last_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTSUBUINT16_DOCSTRING},
    {"col_find_last_sub_uint32", (PyCFunction)py_col_find_last_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTSUBUINT32_DOCSTRING},
    {"col_find_last_sub_uint64", (PyCFunction)py_col_find_last_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLFINDLASTSUBUINT64_DOCSTRING},
    {"col_has_next_uint8", (PyCFunction)py_col_has_next_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTUINT8_DOCSTRING},
    {"col_has_next_uint16", (PyCFunction)py_col_has_next_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTUINT16_DOCSTRING},
    {"col_has_next_uint32", (PyCFunction)py_col_has_next_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTUINT32_DOCSTRING},
    {"col_has_next_uint64", (PyCFunction)py_col_has_next_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTUINT64_DOCSTRING},
    {"col_has_next_sub_uint8", (PyCFunction)py_col_has_next_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTSUBUINT8_DOCSTRING},
    {"col_has_next_sub_uint16", (PyCFunction)py_col_has_next_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTSUBUINT16_DOCSTRING},
    {"col_has_next_sub_uint32", (PyCFunction)py_col_has_next_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTSUBUINT32_DOCSTRING},
    {"col_has_next_sub_uint64", (PyCFunction)py_col_has_next_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLHASNEXTSUBUINT64_DOCSTRING},
    {"col_has_prev_uint8", (PyCFunction)py_col_has_prev_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVUINT8_DOCSTRING},
    {"col_has_prev_uint16", (PyCFunction)py_col_has_prev_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVUINT16_DOCSTRING},
    {"col_has_prev_uint32", (PyCFunction)py_col_has_prev_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVUINT32_DOCSTRING},
    {"col_has_prev_uint64", (PyCFunction)py_col_has_prev_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVUINT64_DOCSTRING},
    {"col_has_prev_sub_uint8", (PyCFunction)py_col_has_prev_sub_uint8, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVSUBUINT8_DOCSTRING},
    {"col_has_prev_sub_uint16", (PyCFunction)py_col_has_prev_sub_uint16, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVSUBUINT16_DOCSTRING},
    {"col_has_prev_sub_uint32", (PyCFunction)py_col_has_prev_sub_uint32, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVSUBUINT32_DOCSTRING},
    {"col_has_prev_sub_uint64", (PyCFunction)py_col_has_prev_sub_uint64, METH_VARARGS|METH_KEYWORDS, PYCOLHASPREVSUBUINT64_DOCSTRING},
    {NULL, NULL, 0, NULL}
};

static const char modulename[] = MODULE_NAME;

struct module_state
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#if PY_MAJOR_VERSION >= 3
static int myextension_traverse(PyObject *m, visitproc visit, void *arg)
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int myextension_clear(PyObject *m)
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef =
{
    PyModuleDef_HEAD_INIT,
    modulename,
    NULL,
    sizeof(struct module_state),
    PyBinsearchMethods,
    NULL,
    myextension_traverse,
    myextension_clear,
    NULL
};

#define INITERROR return NULL

PyObject* PyInit_binsearch(void)
#else
#define INITERROR return

void initbinsearch(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule(modulename, PyBinsearchMethods);
#endif
    struct module_state *st = NULL;
    if (module == NULL)
    {
        INITERROR;
    }
    st = GETSTATE(module);
    st->error = PyErr_NewException(MODULE_NAME ".Error", NULL, NULL);
    if (st->error == NULL)
    {
        Py_DECREF(module);
        INITERROR;
    }
#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
