ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = q3py.la
q3py_la_SOURCES = src/q3py.c
q3py_la_LDFLAGS = -module -avoid-version -shared -export-dynamic \
				  -export-symbols $(srcdir)/src/quake3-vm.sym \
				  $(PYTHON_LDFLAGS) $(PYTHON_EXTRA_LDFLAGS)
q3py_la_LIBADD = $(PYTHON_EXTRA_LIBS)
q3py_la_CFLAGS = $(PYTHON_CPPFLAGS) -std=c99

include_HEADERS = src/q3py.h

include $(srcdir)/doxygen.am
MOSTLYCLEANFILES = $(DX_CLEANFILES)
EXTRA_DIST = $(DX_CONFIG)
