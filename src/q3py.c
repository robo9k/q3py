/**
 * \file
 * \brief Implementation of q3py.
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "q3py_p.h"
/** 
 * Define that this is q3py itself, not another Python module
 * importing it.
 */
#define Q3PY_MODULE
#include "q3py.h"


/** Context to store the global syscall pointer */
static syscallptr *q3_syscall = NULL;

/** Context to store the Python vmMain callback */
static PyObject *q3py_vmMain = NULL;

/**
 * Sets the vmMain Python callback to be used when Quake 3 invokes
 * q3py's vmMain().
 *
 * \param[in] self Unused
 * \param[in] args Python callable
 *
 * \return \c Py_None
 */
static PyObject* q3py_set_vmmain(const PyObject *self, PyObject *args) {
	/* See https://docs.python.org/3/extending/extending.html#calling-python-functions-from-c */

	PyObject *result = NULL;
	PyObject *temp;

	if (PyArg_ParseTuple(args, "O:q3py_set_vmmain", &temp)) {
		if (!PyCallable_Check(temp)) {
			PyErr_SetString(PyExc_TypeError, "parameter must be callable");
			return NULL;
		}
		Py_XINCREF(temp);
		Py_XDECREF(q3py_vmMain);
		q3py_vmMain = temp;
		Py_INCREF(Py_None);
		result = Py_None;
	} else {
		PyErr_Print();
	}
	return result;
}

/** Method definitions for the Python module */
static PyMethodDef Q3PyMethods[] = {
	// http://bugs.python.org/issue11587
	{"set_vmmain", (PyCFunction)q3py_set_vmmain, METH_VARARGS | METH_KEYWORDS, ""},

	{NULL, NULL, 0, NULL}
};

/** Python module definition */
static struct PyModuleDef q3pymodule = {
	PyModuleDef_HEAD_INIT,
	Q3PY_MODULE_NAME,
	"", /* TODO: Write this */
	-1,
	Q3PyMethods
};

/**
 * Function to initialize the q3py Python module.
 *
 * This also creates the capsule to export q3py's C API.
 *
 * \return The q3py module
 */
PyObject* PyInit_q3py() {
	/* See https://docs.python.org/3.4/extending/extending.html#providing-a-c-api-for-an-extension-module */

	PyObject *module = PyModule_Create(&q3pymodule);
	if (NULL == module) {
		fprintf(stderr, "Failed to create module\n");
		return NULL;
	}

	static void *Q3Py_API[Q3PY_API_pointers];
	Q3Py_API[Q3PY_SYSCALL_NUM] = (void *)q3py_syscall;

	PyObject *capsule = PyCapsule_New((void *)Q3Py_API, Q3PY_MODULE_NAME "." Q3PY_CAPI_CAPSULE_NAME, NULL);
	if (capsule != NULL) {
		if (-1 == PyModule_AddObject(module, Q3PY_CAPI_CAPSULE_NAME, capsule)) {
			fprintf(stderr, "Failed to add capsule to module\n");
		}
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Could not create capsule\n");
	}

	return module;
}


/**
 * Main VM entry point for Quake 3. This is invoked by the engine whenever
 * something needs to be done.
 * The possible \p command values depend on the engine variant, e.g.
 * <em>id Quake 1.32c</em> might call this with
 * \code
 * vmMain(GAME_INIT, 0, 42, 0);
 * \endcode
 *
 * q3py just passes this onto its registered Python callback.
 *
 * \param[in] command Identifier of the VM call
 * \param[in,out] arg0,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10, arg11
 * Arguments for the VM call
 *
 * \return Result of the Python callback
 *
 * \sa The callback is set via q3py_set_vmmain().
 */
/* TODO: Check whether the params are really used as [out] */
Q3_API intptr_t vmMain(int command, int arg0, int arg1, int arg2,
		int arg3, int arg4, int arg5, int arg6, int arg7, int arg8,
		int arg9, int arg10, int arg11) {
	PyObject *args = Py_BuildValue("iiiiiiiiiiiii", command, arg0, arg1,
			arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
	if (args != NULL) {
		PyObject *result = PyObject_CallObject(q3py_vmMain, args);
		Py_DECREF(args);
		if (result != NULL && PyLong_Check(result)) {
			long retval = PyLong_AsLong(result);
			Py_DECREF(result);
			return retval;
		}
		else {
			if (PyErr_Occurred()) {
				PyErr_Print();
			}
			fprintf(stderr, "vmMain result is not a long\n");
			Py_XDECREF(result);
		}
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Failed building value for vmMain call\n");
	}

	/* TODO: Error handling */
	return -1;
}

/**
 * Initializes the embedded Python.
 *
 * Adds the q3py Python module and calls an initialization
 * method in Python land.
 */
static void init_python() {
	const int inittab = PyImport_AppendInittab(Q3PY_MODULE_NAME, &PyInit_q3py);
	if (-1 == inittab) {
		fprintf(stderr, "Could not append Python module '%s'\n", Q3PY_MODULE_NAME);
		exit(EXIT_FAILURE);
	}

	Py_Initialize();

	/* TODO: Those should be configureable (e.g. like setuptools entry points) */
	const char *funcname = "dllentry", *modname = "q3py_hello";

	/* See https://docs.python.org/3/extending/embedding.html#pure-embedding */

	/* TODO: exit() on failures */

	/*
	 * TODO: https://docs.python.org/3/c-api/unicode.html#c.PyUnicode_FromString
	 *       Does not specify possible errors. Py_DECREF()!
	 */
	PyObject *moduleName = PyUnicode_FromString(modname);

	/*
	 * TODO: https://docs.python.org/3/c-api/import.html#PyImport_Import
	 *       Does not specify possible errors.
	 */
	PyObject *module = PyImport_Import(moduleName);
	Py_DECREF(moduleName);

	if (module != NULL) {
		PyObject *function = PyObject_GetAttrString(module, funcname);
		if (function && PyCallable_Check(function)) {
			const PyObject *result = PyObject_CallObject(function, NULL);
			if (result != NULL) {
				Py_DECREF(result);
			}
			else {
				Py_DECREF(function);
				Py_DECREF(module);

				PyErr_Print();
				fprintf(stderr, "Calling init function '%s' failed\n", funcname);
			}
		}
		else {
			if (PyErr_Occurred()) {
				PyErr_Print();
			}
			fprintf(stderr, "Cannot find function '%s'\n", funcname);
		}

		Py_XDECREF(function);
		Py_DECREF(module);
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Failed to load module '%s'\n", modname);
	}
}

/**
 * VM initialization function for Quake 3. The engine calls this once upon
 * loading a VM.
 *
 * q3py initializes the embedded Python.
 *
 * \param[in] syscallptr Pointer to the engine's syscall function
 *
 * \sa The syscall function is exposed via q3py_syscall() and
 * q3py_vsyscall().
 */
/* TODO: game/map_restart? */
Q3_API void dllEntry(const syscallptr const *syscallptr) {
	q3_syscall = syscallptr;

	init_python();
	/* TODO: Split initialization of Python and dllEntry? */
	/* TODO: Check whether q3py_set_vmmain has been called (or in vmMain itself) */
}

Q3PY_API intptr_t q3py_vsyscall(intptr_t number, va_list ap) {
	intptr_t args[MAX_VMSYSCALL_ARGS - 1];
	for (size_t i = 0; i < ARRAY_LEN(args); ++i) {
		args[i] = va_arg(ap, intptr_t);
	}

	return q3_syscall(number, args[0], args[1], args[2], args[3], args[4],
			args[5], args[6], args[7], args[8], args[9], args[10],
			args[11], args[12], args[13], args[14]);
}

Q3PY_API intptr_t q3py_syscall(intptr_t number, ...) {
	va_list args;
	va_start(args, number);

	intptr_t retval = q3py_vsyscall(number, args);

	va_end(args);
	return retval;
}

