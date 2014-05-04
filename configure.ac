AC_INIT([q3py], [0.0.1], [robo@9k.lv], [q3py], [https://github.com/robo9k/q3py])
AC_CONFIG_SRCDIR([src/q3py.c])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign])
LT_INIT

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC

DX_INIT_DOXYGEN([$PACKAGE_TARNAME], [doc/Doxyfile], [doxygen-doc])

AX_PYTHON_DEVEL([>= '3.2'])

AM_PATH_CHECK

AC_OUTPUT([Makefile])
