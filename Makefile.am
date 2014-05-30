ACLOCAL_AMFLAGS = -I m4

## libq3py is a convenience library to build both
## the q3py module as well as link it into the check programs
noinst_LTLIBRARIES = libq3py.la
libq3py_la_SOURCES = src/q3py.c src/q3py_p.h src/q3py.h
libq3py_la_LDFLAGS = $(PYTHON_LDFLAGS) $(PYTHON_EXTRA_LDFLAGS)
libq3py_la_LIBADD = $(PYTHON_EXTRA_LIBS)
libq3py_la_CFLAGS = $(PYTHON_CPPFLAGS)

## q3py is the module that is being installed
lib_LTLIBRARIES = q3py.la
q3py_la_SOURCES =
q3py_la_LDFLAGS = -module -avoid-version -shared -export-dynamic \
				  -export-symbols $(srcdir)/src/quake3-vm.sym
q3py_la_LIBADD = libq3py.la


include_HEADERS = src/q3py.h

include $(srcdir)/doxygen.am
MOSTLYCLEANFILES = $(DX_CLEANFILES)
EXTRA_DIST = README.rst LICENSE doc src/quake3-vm.sym $(DX_CONFIG)

TESTS = check_q3py
check_PROGRAMS = check_q3py
check_q3py_SOURCES = tests/check_q3py.c $(top_builddir)/src/q3py.h
check_q3py_CFLAGS = @CHECK_CFLAGS@
check_q3py_LDADD = $(top_builddir)/libq3py.la @CHECK_LIBS@
