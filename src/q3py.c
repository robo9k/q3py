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
#include <q3py.h>


/** Context to store the global syscall pointer */
static syscallptr *q3_syscall = NULL;

/** Context to store the Python vmMain callback */
static PyObject *q3py_vmMain = NULL;


static void q3py_log(const char* message) {
	fprintf(stdout, "Q3PY: %s\n", message);
}

static void q3py_error(const char* message) {
	fprintf(stderr, "Q3PY: %s\n", message);
}


/**
 * Tries to finalize Python, then exits.
 */
/* TODO: Mark this is noreturn for compilers to be happy */
static void q3py_exit() {
	/* Try to clean up Python */
	Py_Finalize();

	/*
	 * Exit the whole process. While we are a library, there is no
	 * clean and portable way to signal Quake 3 that we have hit a fatal
	 * error, so just exit here.
	 */
	exit(EXIT_FAILURE);
}

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
	{"set_vmmain",
		(PyCFunction)q3py_set_vmmain,
		METH_VARARGS | METH_KEYWORDS,
		""},

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
		q3py_error("Failed to create module '" Q3PY_MODULE_NAME "'");
		return NULL;
	}

	static void *Q3Py_API[Q3PY_API_pointers];
	Q3Py_API[Q3PY_SYSCALL_NUM] = (void *)q3py_syscall;

	PyObject *capsule = PyCapsule_New((void *)Q3Py_API,
			Q3PY_MODULE_NAME "." Q3PY_CAPI_CAPSULE_NAME, NULL);
	if (capsule != NULL) {
		if (-1 == PyModule_AddObject(module, Q3PY_CAPI_CAPSULE_NAME, capsule)) {
			q3py_error("Failed to add capsule '" Q3PY_CAPI_CAPSULE_NAME "' "
			           "to module '" Q3PY_MODULE_NAME "'");
			return NULL;
		}
	}
	else {
		PyErr_Print();
		q3py_error("Could not create capsule");
		return NULL;
	}

	return module;
}

/**
 * Checks whether q3py_vmMain has been set and exits with an error if not.
 */
/*
 * TODO: Return a bool instead, so the caller can provide a more precise
 * error message (and call q3py_exit() himself).
 */
void check_vmMainPy() {
	if (NULL == q3py_vmMain) {
		q3py_error("vmMain Python method has not been set");
		q3py_exit();
	}
}

Q3_API intptr_t vmMain(int command, int arg0, int arg1, int arg2,
		int arg3, int arg4, int arg5, int arg6, int arg7, int arg8,
		int arg9, int arg10, int arg11) {

	/*
	 * Check whether q3py_vmMain has been setup correctly.
	 * This is also checked in dllEntry(), but we might be invoked without
	 * the expected initialization order.
	 * Ideally we would not check this on every invocation, as it imposes
	 * some overhead.
	 */
	check_vmMainPy();

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
			q3py_error("vmMain result is not a long");
			Py_XDECREF(result);
		}
	}
	else {
		PyErr_Print();
		q3py_error("Failed building value for vmMain call");
	}

	q3py_exit();
	return -1;
}


/**
 * Initializes the embedded Python.
 *
 * Adds the q3py Python module and calls an initialization
 * method in Python land.
 */
static void init_python() {
	const int inittab = PyImport_AppendInittab(Q3PY_MODULE_NAME,
			&PyInit_q3py);
	if (-1 == inittab) {
		q3py_error("Could not append Python module '" Q3PY_MODULE_NAME "'");
		q3py_exit();
	}

	Py_Initialize();


	/*
	 * NOTE: Having one env var (e.g. "Q3PY_ENTRYPOINT") with value
	 * "module:method" might be easier for the user to configure, but
	 * C's string handling is meh.
	 */
	char *modname = getenv(Q3PY_ENV_ENTRYPOINT_MODULE);
	if (NULL == modname) {
		q3py_error("Entry point module (" Q3PY_ENV_ENTRYPOINT_MODULE ") "
				"is not set");
		q3py_exit();
	}

	char *funcname = getenv(Q3PY_ENV_ENTRYPOINT_METHOD);
	if (NULL == modname) {
		q3py_error("Entry point method (" Q3PY_ENV_ENTRYPOINT_METHOD ") "
				"is not set");
		q3py_exit();
	}

	char entrypoint_buffer[128];
	snprintf(entrypoint_buffer, sizeof(entrypoint_buffer),
			"Entry point is '%s:%s'", modname, funcname);
	q3py_log(entrypoint_buffer);


	/* See https://docs.python.org/3/extending/embedding.html#pure-embedding */

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

	/* Buffer for formatted error messages, size chosen arbitrarily */
	char error_message_buffer[128];

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
				snprintf(error_message_buffer, sizeof(error_message_buffer),
						"Calling init method '%s' failed", funcname);
				q3py_error(error_message_buffer);
				q3py_exit();
			}
		}
		else {
			if (PyErr_Occurred()) {
				PyErr_Print();
			}
			snprintf(error_message_buffer, sizeof(error_message_buffer),
					"Can not find method '%s' in module '%s'",
					funcname, modname);
			q3py_error(error_message_buffer);
			q3py_exit();
		}

		Py_XDECREF(function);
		Py_DECREF(module);
	}
	else {
		PyErr_Print();
		snprintf(error_message_buffer, sizeof(error_message_buffer),
				"Failed to load module '%s'", modname);
		q3py_error(error_message_buffer);
		q3py_exit();
	}
}

Q3_API void dllEntry(const syscallptr * const syscallptr) {
	/* A NULL pointer is the only invalid value we can check for */
	if (NULL == syscallptr) {
		q3py_error("NULL syscall pointer");
		q3py_exit();
	}

	q3_syscall = syscallptr;
	char message_buffer[128];
	snprintf(message_buffer, sizeof(message_buffer),
			"dllEntry called with syscall %p", syscallptr);
	q3py_log(message_buffer);

	init_python();
	/* TODO: Split initialization of Python and dllEntry? */

	/* Check whether q3py_vmMain has been set within init_python() */
	check_vmMainPy();
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

