#ifndef Q3PY_PRIVATE_H
#define Q3PY_PRIVATE_H

/**
 * Quake 3 macro to use the cdecl calling convention (on Windows).
 * TODO: Actually define this properly.
 * \va ioq3 qcommon/q_platform.h
 */
#define QDECL

/**
 * Quake 3 macro to define the maximum number of arguments that
 * the syscall function in the engine parses when being called
 * by an VM.
 * \va ioq3 qcommon/vm_local.h
 */
#define MAX_VMSYSCALL_ARGS 16

/*
 * Macro adopted from Quake 3 to return the count of items in
 * an array.
 * \va ioq3 qcommon/q_shared.h
 */
#define ARRAY_LEN(x)			(sizeof(x) / sizeof(x[0]))

/*
 * Pointer type of Quake 3's syscall function.
 */
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
#define Q3_API
#endif

/**
 * A macro to export functions that are part of the q3py API.
 * \sa Q3_API is a variant for the Quake 3 API.
 */
#define Q3PY_API Q3_API

#endif /* Q3PY_PRIVATE_H */

