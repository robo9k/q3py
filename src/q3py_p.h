/**
 * \file
 * \brief Private header of q3py.
 *
 * This header mostly redefines Quake 3 defines.
 * The Quake 3 headers are not really suited to be included
 * (let alone partial inclusion by a minimal library like q3py).
 */

#ifndef Q3PY_PRIVATE_H
#define Q3PY_PRIVATE_H

#include <inttypes.h>


/**
 * Quake 3 macro to use the cdecl calling convention (on Windows).
 *
 * \sa ioq3 qcommon/q_platform.h
 */
/* TODO: Actually define this properly */
#define QDECL


/**
 * Quake 3 macro to define the maximum number of arguments that
 * the syscall function in the engine parses when being called
 * by a VM.
 *
 * \sa ioq3 qcommon/vm_local.h
 */
#define MAX_VMSYSCALL_ARGS 16


/**
 * Macro adopted from Quake 3 to return the count of items in
 * an array.
 *
 * \sa ioq3 qcommon/q_shared.h
 */
#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))


/**
 * Pointer type of Quake 3's syscall function.
 */
/* TODO: This ^ is broken in doxygen */
typedef intptr_t (QDECL syscallptr)(intptr_t arg, ...);


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
/* TODO: Document the fact that this is mainly informational,
 *       as the exported symbols are controlled by quake3-vm.sym
 */
#define Q3_API
#endif

/**
 * A macro to export functions that are part of the q3py API.
 * \sa Q3_API is a variant for the Quake 3 API.
 */
/* TODO: See note about exported symbols via Q3_API. Furthermore
 *       the C API is currently only exposed via a Python capsule.
 */
#define Q3PY_API Q3_API


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
		int arg9, int arg10, int arg11);

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
Q3_API void dllEntry(const syscallptr * const syscallptr);


/**
 * Macro to log a q3py error message.
 *
 * \param[in] ... Message and possibly arguments if formatted
 *
 * \sa printf() from stdio.h
 */
#define Q3PY_LOG_ERROR(...) fprintf(stderr, "Q3PY [ERROR]: " __VA_ARGS__)

/**
 * Macro to log a q3py info message.
 *
 * \param[in] ... Message and possibly arguments if formatted
 *
 * \sa printf() from stdio.h
 */
#define Q3PY_LOG_INFO(...) fprintf(stdout, "Q3PY [INFO]: " __VA_ARGS__)


#endif /* Q3PY_PRIVATE_H */

