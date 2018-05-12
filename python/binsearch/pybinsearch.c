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

static PyObject* py_mmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    const char *file;
    if (!PyArg_ParseTuple(args, "s", &file))
        return NULL;
    mmfile_t h;
    mmap_binfile(file, &h);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, PyCapsule_New((void*)h.src, "src", NULL));
    PyTuple_SetItem(result, 1, Py_BuildValue("i", h.fd));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", h.size));
    return result;
}

static PyObject* py_munmap_binfile(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    mmfile_t mf;
    PyObject *result;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OiK", &mfsrc, &mf.fd, &mf.size))
        return NULL;
    mf.src = (unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    int h = munmap_binfile(mf);
    result = Py_BuildValue("i", h);
    return result;
}

static PyObject* py_get_address(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, item;
    if (!PyArg_ParseTuple(args, "KKK", &blklen, &blkpos, &item))
        return NULL;
    uint64_t h = get_address(blklen, blkpos, item);
    result = Py_BuildValue("K", h);
    return result;
}

static PyObject* py_find_first_uint8(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKB", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint8(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint8_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKB", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint8_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint16(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKH", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint16(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint16_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKH", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint16_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint32(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKI", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint32(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint32_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKI", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint32_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_first_uint64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKK", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_first_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyObject* py_find_last_uint64(PyObject *Py_UNUSED(ignored), PyObject *args)
{
    PyObject *result;
    uint64_t blklen, blkpos, first, last;
    uint8_t bitstart, bitend;
    uint64_t search;
    PyObject* mfsrc = NULL;
    if (!PyArg_ParseTuple(args, "OKKBBKKK", &mfsrc, &blklen, &blkpos, &bitstart, &bitend, &first, &last, &search))
        return NULL;
    const unsigned char *src = (const unsigned char *)PyCapsule_GetPointer(mfsrc, "src");
    uint64_t h = find_last_uint64_t(src, blklen, blkpos, bitstart, bitend, &first, &last, search);
    result = PyTuple_New(3);
    PyTuple_SetItem(result, 0, Py_BuildValue("K", h));
    PyTuple_SetItem(result, 1, Py_BuildValue("K", first));
    PyTuple_SetItem(result, 2, Py_BuildValue("K", last));
    return result;
}

static PyMethodDef PyBinsearchMethods[] =
{
    {"mmap_binfile", py_mmap_binfile, METH_VARARGS, PYMMAPBINFILE_DOCSTRING},
    {"munmap_binfile", py_munmap_binfile, METH_VARARGS, PYMUNMAPBINFILE_DOCSTRING},
    {"get_address", py_get_address, METH_VARARGS, PYGETADDRESS_DOCSTRING},
    {"find_first_uint8", py_find_first_uint8, METH_VARARGS, PYFINDFIRSTUINT8_DOCSTRING},
    {"find_last_uint8", py_find_last_uint8, METH_VARARGS, PYFINDLASTUINT8_DOCSTRING},
    {"find_first_uint16", py_find_first_uint16, METH_VARARGS, PYFINDFIRSTUINT16_DOCSTRING},
    {"find_last_uint16", py_find_last_uint16, METH_VARARGS, PYFINDLASTUINT16_DOCSTRING},
    {"find_first_uint32", py_find_first_uint32, METH_VARARGS, PYFINDFIRSTUINT32_DOCSTRING},
    {"find_last_uint32", py_find_last_uint32, METH_VARARGS, PYFINDLASTUINT32_DOCSTRING},
    {"find_first_uint64", py_find_first_uint64, METH_VARARGS, PYFINDFIRSTUINT64_DOCSTRING},
    {"find_last_uint64", py_find_last_uint64, METH_VARARGS, PYFINDLASTUINT64_DOCSTRING},
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
