/**
 * \file
 * \brief Implementation of q3py.
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

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


/**
 * Tries to finalize Python, then exits.
 *
 * \param[in] status exit status code
 */
/* TODO: Mark this is noreturn for compilers to be happy */
static void q3py_exit(int status) {
	/* Try to clean up Python */
	Py_Finalize();

	/*
	 * Exit the whole process. While we are a library, there is no
	 * clean and portable way to signal Quake 3 that we have hit a fatal
	 * error, so just exit here.
	 */
	exit(status);
}

/**
 * Sets the vmMain Python callback.
 *
 * \param[in] borrowed reference of Python callable
 *
 * \return \c Py_None on success or \c NULL on error
 */
static PyObject* q3py_set_vmmain_callable(PyObject *callable) {
	if (!PyCallable_Check(callable)) {
		PyErr_SetString(PyExc_TypeError, "parameter must be callable");
		return NULL;
	}

	Py_INCREF(callable);
	Py_XDECREF(q3py_vmMain);
	q3py_vmMain = callable;

	/* For debugging purposes; */
	//PyObject_Print(q3py_vmMain, stdout, 0);

	Py_RETURN_NONE;
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
		result = q3py_set_vmmain_callable(temp);
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

	/* TODO: Python module name needs to depend on Quake 3 module name
	 * if multiple instances (game, cgame, ui) are used in the same process.
	 */
	PyObject *module = PyModule_Create(&q3pymodule);
	if (NULL == module) {
		Q3PY_LOG_ERROR("Failed to create module '"Q3PY_MODULE_NAME"'\n");
		return NULL;
	}

	static void *Q3Py_API[Q3PY_API_pointers];
	Q3Py_API[Q3PY_SYSCALL_NUM] = (void *)q3py_syscall;

	PyObject *capsule = PyCapsule_New((void *)Q3Py_API,
			Q3PY_MODULE_NAME "." Q3PY_CAPI_CAPSULE_NAME, NULL);
	if (capsule != NULL) {
		if (-1 == PyModule_AddObject(module, Q3PY_CAPI_CAPSULE_NAME, capsule)) {
			Q3PY_LOG_ERROR("Failed to add capsule '"Q3PY_CAPI_CAPSULE_NAME"' "
					"to module '"Q3PY_MODULE_NAME"'\n");
			return NULL;
		}
	}
	else {
		PyErr_Print();
		Q3PY_LOG_ERROR("Could not create capsule\n");
		return NULL;
	}

	int ok = PyModule_AddStringConstant(module, "__version__", PACKAGE_VERSION);
	if (-1 == ok) {
		Q3PY_LOG_ERROR("Could not add __version__ to '" Q3PY_MODULE_NAME "' module\n");
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
		Q3PY_LOG_ERROR("vmMain Python method has not been set\n");
		q3py_exit(EX_CONFIG);
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
			Q3PY_LOG_ERROR("vmMain result is not a long\n");
			Py_XDECREF(result);
		}
	}
	else {
		PyErr_Print();
		Q3PY_LOG_ERROR("Failed building Python value for vmMain call\n");
	}

	q3py_exit(EX_SOFTWARE);
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
		Q3PY_LOG_ERROR("Could not append Python module '"Q3PY_MODULE_NAME"'\n");
		q3py_exit(EX_SOFTWARE);
	}

	Py_Initialize();


	char *entrypoint_env = getenv(Q3PY_ENV_ENTRYPOINT);
	if (NULL == entrypoint_env) {
		Q3PY_LOG_ERROR("Entry point ("Q3PY_ENV_ENTRYPOINT") is not set\n");
		q3py_exit(EX_CONFIG);
	}

	char entrypoint[strlen(entrypoint_env) + 1];
	strncpy(entrypoint, entrypoint_env, sizeof(entrypoint));
	entrypoint[sizeof(entrypoint)] = '\0';

	char *entrypoint_separator = strchr(entrypoint, ':');
	if (NULL == entrypoint_separator) {
		Q3PY_LOG_ERROR("Entry point '%s' is not well-formed (module:method)\n", entrypoint);
		q3py_exit(EX_USAGE);
	}

	Q3PY_LOG_INFO("Entry point is '%s'\n", entrypoint);

	*entrypoint_separator = '\0';
	char *modname = entrypoint, *funcname = (entrypoint_separator + 1);


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

	if (module != NULL) {
		PyObject *function = PyObject_GetAttrString(module, funcname);
		if (function && PyCallable_Check(function)) {
			const PyObject *result = PyObject_CallObject(function, NULL);
			if (result != NULL) {
				/* TODO: Error handling */
				q3py_set_vmmain_callable(result);
				Py_DECREF(result);
			}
			else {
				Py_DECREF(function);
				Py_DECREF(module);

				PyErr_Print();
				Q3PY_LOG_ERROR("Calling init method '%s:%s' failed\n", modname, funcname);
				q3py_exit(EX_SOFTWARE);
			}
		}
		else {
			if (PyErr_Occurred()) {
				PyErr_Print();
			}
			Q3PY_LOG_ERROR("Can not find method '%s' in module '%s'\n", funcname, modname);
			q3py_exit(EX_NOINPUT);
		}

		Py_XDECREF(function);
		Py_DECREF(module);
	}
	else {
		PyErr_Print();
		Q3PY_LOG_ERROR("Failed to load module '%s'\n", modname);
		q3py_exit(EX_SOFTWARE);
	}
}

Q3_API void dllEntry(const syscallptr * const syscallptr) {
	/* A NULL pointer is the only invalid value we can check for */
	if (NULL == syscallptr) {
		Q3PY_LOG_ERROR("NULL syscall pointer\n");
		q3py_exit(EX_USAGE);
	}

	q3_syscall = syscallptr;
	Q3PY_LOG_INFO("dllEntry called with syscall %p\n", syscallptr);

	init_python();
	/* TODO: Split initialization of Python and dllEntry? */

	/* Check whether q3py_vmMain has been set within init_python() */
	check_vmMainPy();

	Q3PY_LOG_INFO("v" PACKAGE_VERSION " initialized");
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

