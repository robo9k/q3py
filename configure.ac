AC_INIT([q3py], [0.0.1], [robo@9k.lv], [q3py], [https://github.com/robo9k/q3py])
AC_CONFIG_SRCDIR([src/q3py.c])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CC_C99

DX_INIT_DOXYGEN([$PACKAGE_TARNAME], [doc/Doxyfile], [doxygen-doc])

AX_PYTHON_DEVEL([>= '3.2'])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])


AC_ARG_ENABLE([gcov],
  [AS_HELP_STRING([--enable-gcov],
    [use Gcov instrumentation for test coverage])],
    [],
    [enable_gcov=no])
AM_CONDITIONAL([COND_GCOV],[test '!' "$enable_gcov" = no])

# Disabling inlining of functions works different in GCC, clang, etc.
CFLAGS_DISABLE_INLINING=
AX_CHECK_COMPILE_FLAG([-mllvm -inline-threshold=0], [CFLAGS_DISABLE_INLINING='-mllvm -inline-threshold=0'])
AX_CHECK_COMPILE_FLAG([--no-inline], [CFLAGS_DISABLE_INLINING=--no-inline])
AC_SUBST([CFLAGS_DISABLE_INLINING])


AC_OUTPUT([Makefile])
