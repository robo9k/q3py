ACLOCAL_AMFLAGS = -I m4

if COND_GCOV
   MAYBE_COVERAGE=--coverage $(CFLAGS_DISABLE_INLINING)
endif

## libq3py is a convenience library to build both
## the q3py module as well as link it into the check programs
noinst_LTLIBRARIES = libq3py.la
libq3py_la_SOURCES = src/q3py.c src/q3py_p.h
libq3py_la_LDFLAGS = $(PYTHON_LDFLAGS) $(PYTHON_EXTRA_LDFLAGS)
libq3py_la_LIBADD = $(PYTHON_EXTRA_LIBS)
libq3py_la_CFLAGS = $(PYTHON_CPPFLAGS) -I$(top_srcdir)/include $(MAYBE_COVERAGE)

## q3py is the module that is being installed
lib_LTLIBRARIES = q3py.la
q3py_la_SOURCES =
q3py_la_LDFLAGS = -module -avoid-version -shared -export-dynamic \
				  -export-symbols $(srcdir)/src/quake3-vm.sym
q3py_la_LIBADD = libq3py.la


include_HEADERS = include/q3py.h

include $(srcdir)/doxygen.am
MOSTLYCLEANFILES = $(DX_CLEANFILES) *.gcda *.gcno
EXTRA_DIST = README.rst LICENSE doc src/quake3-vm.sym $(DX_CONFIG)

TESTS = check_q3py
check_PROGRAMS = check_q3py
check_q3py_SOURCES = tests/check_q3py.c
check_q3py_CFLAGS = @CHECK_CFLAGS@ $(MAYBE_COVERAGE)
check_q3py_LDADD = $(top_builddir)/libq3py.la @CHECK_LIBS@

sphinx-dirhtml: doxygen-doc
	sphinx-build -b dirhtml \
		-D breathe_projects.q3py=$(builddir)/$(DX_DOCDIR)/xml \
		$(srcdir)/doc $(builddir)/sphinx-dirhtml
