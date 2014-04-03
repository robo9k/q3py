/**
 * \file
 * \brief Header for the public API of q3py.
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

#if (defined _MSC_VER)
#define Q3_API __declspec(dllexport)
#elif (defined __SUNPRO_C)
#define Q3_API __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
#define Q3_API __attribute__((visibility("default")))
#else
/**
 * A macro to export functions that are part of the Quake 3 API.
 * \sa Q3PY_API is a variant for the q3py API.
 */
/* TODO: Move Q3_API into private header */
#define Q3_API
#endif

/**
 * A macro to export functions that are part of the q3py API.
 * \sa Q3_API is a variant for the Quake 3 API.
 */
#define Q3PY_API Q3_API

/**
 * Invokes a Quake 3 system call.
 *
 * \warning
 * Only a magic number of arguments is supported, any
 * extra arguments will be ignored.
 *
 * \param[in] number Identifier of the system call.
 * \param[in] args List of arguments for the system call.
 *
 * \return The return value of the syscall.
 *
 * \sa q3py_syscall() is a variadic variant.
 */
Q3PY_API intptr_t q3py_vsyscall(intptr_t number, va_list args);

/**
 * Invokes a Quake 3 system call.
 *
 * \warning
 * Only a magic number of arguments is supported, any
 * extra arguments will be ignored.
 *
 * \param[in] number Identifier of the system call.
 * \param[in] ... Arguments for the system call.
 *
 * \return The return value of the syscall.
 *
 * \va q3py_vsyscall() is a variant with argument list.
 */
Q3PY_API intptr_t q3py_syscall(intptr_t number, ...);

#define Q3PY_MODULE_NAME "q3py"
#define Q3PY_CAPI_CAPSULE_NAME "_C_API"

#define Q3PY_SYSCALL_NUM 1
#define Q3PY_SYSCALL_RETURN intptr_t
#define Q3PY_SYSCALL_PROTO (intptr_t number, ...)

/* TODO: Add some version function at index 0 */
#define Q3PY_API_pointers 2

#ifndef Q3PY_MODULE
static void **Q3Py_API;
#define q3py_syscall \
	(*(Q3PY_SYSCALL_RETURN (*)Q3PY_SYSCALL_PROTO) Q3Py_API[Q3PY_SYSCALL_NUM])

static int import_q3py() {
	Q3Py_API = (void **)PyCapsule_Import(Q3PY_MODULE_NAME "." Q3PY_CAPI_CAPSULE_NAME, 0);
	return (Q3Py_API != NULL) ? 0 : -1;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* Q3PY_H */

