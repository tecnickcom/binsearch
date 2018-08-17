// Python binsearch Module
//
// @category   Libraries
// @author     Nicola Asuni <nicola.asuni@tecnick.com>
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/binsearch

#include <Python.h>
#include "../../c/src/binsearch.h"
#include "pybinsearch.h"

#ifndef Py_UNUSED /* This is already defined for Python 3.4 onwards */
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
    static char *kwlist[] = {"file", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "s", kwlist, &file))
        return NULL;
    mmfile_t h;
    mmap_binfile(file, &h);
    result = PyTuple_New(4);
    PyTuple_SetItem(result, 0, PyCapsule_New((void*)h.src, "src", NULL));
    PyTuple_SetItem(result, 1, Py_BuildValue("i", h.fd));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.size));
    PyTuple_SetItem(result, 3, Py_BuildValue("K", h.last));
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
    mf.src = (unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "pos", "last", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &pos, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKB", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKH", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKI", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKKKK", kwlist, &mfsrc, &blklen, &blkpos, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKB", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKH", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKI", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
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
    PyObject* mfsrc = NULL;
    static char *kwlist[] = {"mfsrc", "blklen", "blkpos", "bitstart", "bitend", "first", "pos", "search", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, keywds, "OKKBBKKK", kwlist, &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &pos, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    bool h = has_prev_sub_le_uint64_t(src, blklen, blkpos, bitstart, bitend, first, &pos, search);
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
    {NULL, NULL, 0, NULL}
};

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
    "binsearch",
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

void
initbinsearch(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("binsearch", PyBinsearchMethods);
#endif
    struct module_state *st = NULL;

    if (module == NULL)
        INITERROR;
    st = GETSTATE(module);

    st->error = PyErr_NewException("binsearch.Error", NULL, NULL);
    if (st->error == NULL)
    {
        Py_DECREF(module);
        INITERROR;
    }

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
