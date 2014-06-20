C API
=====

q3py exposes a C API as a Python Capsule, which can be used directly
by other native Python extensions.

All of the following can be found in q3py's public
header file ``include/q3py.h``.

.. c:function:: intptr_t q3py_syscall(intptr_t number, ...)

    .. versionadded:: 0.0.1

    Invokes a Quake 3 ``syscall``.

    :param number: The number of the syscall
    :param ...: The arguments for the syscall

    :return: The result of the syscall


.. c:function:: int import_q3py()

    .. versionadded:: 0.0.1

    Imports the q3py Python capsule into :c:data:`Q3Py_API`.

    .. warning::
        This needs to be called before any other q3py
        function, otherwise you will get a nice ``SEGFAULT``.

    :return: ``0`` on success, ``-1`` on error


.. c:var:: void** Q3Py_API

    .. versionadded:: 0.0.1

    Holds a pointer to the q3py Python capsule.

    .. warning::
        Do not set this directly, use :c:func:`import_q3py` instead!
