Python API
==========

q3py acts as a C extension module to the embedded Python.
This allows the Python module to call back into Quake 3
and configure q3py.


.. py:module:: q3py
    :synopsis: Bridge to use Python for Quake 3 game modules


.. py:function:: set_vmmain(callback)

    Sets the ``vmMain`` callable that q3py invokes when
    being called by Quake 3.

    :param callable callback: The Python callable to use
    :raises: TypeError: if ``callable`` is not a ``callable()``


.. py:function:: syscall(num[, args])

    Invokes a Quake 3 ``syscall``.

    :param int num: The number of the syscall
    :param int args: The arguments for the syscall

    :return: The result of the syscall
    :rtype: int


.. py:attribute:: __version__

    Contains the version of q3py as a read-only string of
    the form :samp:`{MAJOR}.{MINOR}.{PATCH}`, e.g. "|version|".


.. py:attribute:: _C_API

    Contains a read-only ``PyCapsule`` of q3py's C API.
