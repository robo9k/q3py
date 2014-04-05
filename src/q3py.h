/**
 * \file
 * \brief Header for the public C API of q3py.
 *
 * q3py provides functions to call back into the associated Quake 3
 * instance via syscalls.
 *
 * The syscall numbers depend on the context in which this module
 * is running and the engine variant in use.
 * An example might be running as \e cgame in <em>id Quake 3 1.32c</em>
 * and calling \c CL_AddReliableCommand() with
 * \code
 * q3py_syscall(CG_SENDCLIENTCOMMAND, "score");
 * \endcode
 *
 * \warning
 * While the syscall functions appear to support an indefinite number
 * of arguments, they do not!
 * \warning
 * Due to difficulties with reliably passing variadic arguments
 * from the VM to the engine and vice versa, even Quake 3 only supports
 * a fixed maximum number of arguments internally. The exact number is
 * an engine implementation detail.
 * \warning
 * This magic number should be equal to the one used by q3py and
 * is as such sufficient to invoke any of the syscalls.
 * \warning \n
 * This is nothing that could be fixed by q3py as the syscall mechanism
 * in Quake 3 is doomed by design (pun intended).
 */

#ifndef Q3PY_H
#define Q3PY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdarg.h>


/**
 * Invokes a Quake 3 system call.
 *
 * \param[in] number Identifier of the system call.
 * \param[in] args List of arguments for the system call.
 *
 * \return Result of the syscall.
 *
 * \sa q3py_syscall() is a variadic variant.
 */
intptr_t q3py_vsyscall(intptr_t number, va_list args);


/**
 * Index of q3py_syscall() in the C API capsule.
 */
#define Q3PY_SYSCALL_NUM 0
/**
 * Return type of q3py_syscall().
 */
#define Q3PY_SYSCALL_RETURN intptr_t
/**
 * Function prototype of q3py_syscall().
 */
#define Q3PY_SYSCALL_PROTO (intptr_t number, ...)

/* TODO: Using the Q3PY_SYSCALL_ defines above would be DRY, but
 *       it confuses doxygen (as well as human readers of this file).
 */
/**
 * Invokes a Quake 3 system call.
 *
 * \param[in] number Identifier of the system call.
 * \param[in] ... Arguments for the system call.
 *
 * \return Result of the syscall.
 *
 * \sa q3py_vsyscall() is a variant with argument list.
 */
intptr_t q3py_syscall(intptr_t number, ...);


/**
 * Name of the Python module under which q3py will register itself.
 */
#define Q3PY_MODULE_NAME "q3py"
/**
 * Name of the Python capsulse which exposes q3py's C API.
 */
#define Q3PY_CAPI_CAPSULE_NAME "_C_API"
/**
 * Number of function pointers in the C API capsule.
 */
#define Q3PY_API_pointers 1


/**
 * If this header is included while building another Python
 * extension module, expose q3py's C API via a capsule.
 */
#ifndef Q3PY_MODULE

/**
 * Pointer to q3py's C API capsule. Initialized via import_q3py().
 */
static void **Q3Py_API;


/**
 * Redefine q3py_syscall() to use the capsule.
 *
 * \warning import_q3py() needs to be called before invoking
 * this q3py_syscall() definition!
 */
#define q3py_syscall \
	(*(Q3PY_SYSCALL_RETURN (*)Q3PY_SYSCALL_PROTO) Q3Py_API[Q3PY_SYSCALL_NUM])


/**
 * Function to import q3py's Python capsule, which is a
 * prerequisite to call any of q3py's C API functions.
 *
 * \return \c 0 on success, \c -1 on failure.
 */
static int import_q3py() {
	Q3Py_API = (void **)PyCapsule_Import(Q3PY_MODULE_NAME "." Q3PY_CAPI_CAPSULE_NAME, 0);
	return (Q3Py_API != NULL) ? 0 : -1;
}

#endif /* Q3PY_MODULE */


#ifdef __cplusplus
}
#endif

#endif /* Q3PY_H */

