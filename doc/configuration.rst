Configuration
=============

Since q3py is loaded as a shared library by Quake 3, you can
not pass command line options to it. Instead, q3py is configured
with environment variables.

.. envvar:: Q3PY_ENTRYPOINT

    .. versionadded:: 0.0.1

    The Python entry point which q3py shall use in the form
    of :samp:`{module}:{method}`.

    .. seealso::
        :c:macro:`Q3PY_ENV_ENTRYPOINT`
        :c:func:`init_python()`
