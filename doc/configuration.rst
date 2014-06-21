.. _configuration:

Configuration
=============

To run q3py, you need to configure the components that it
consists of; i.e. q3py itself, Quake 3 and the embedded Python.

As an example, you can run q3py like this;

.. sourcecode:: sh
    :linenos:

    Q3PY_ENTRYPOINT="myq3py:init" \
    PYTHONPATH="~/my-q3py-module/" \
    ioq3ded +set vm_game 0 \
    +set fs_game "q3py" +map "q3dm6"

.. note::
    This example assumes that you installed q3py in a "q3py" game
    directory (the ``fs_game`` option for Quake 3).

    Furthermore it starts the dedicated server of ioquake3 and loads
    the map "q3dm6".

The output from running this example might look like this::

    Try loading dll file /home/robo9k/.q3a/q3py/qagamex86_64.so
    Loading DLL file: /home/robo9k/.q3a/q3py/qagamex86_64.so
    Sys_LoadGameDll(/home/robo9k/.q3a/q3py/qagamex86_64.so) found vmMain function at 0xdeadbeef
    ]Q3PY [INFO]: dllEntry called with syscall 0xcafebabe
    Q3PY [INFO]: Entry point is 'myq3py:init'
    Q3PY [INFO]: v0.0.1 initialized


q3py
----

Since q3py is loaded as a shared library by Quake 3, you can
not pass command line options to it. Instead, q3py is configured
with environment variables.

.. envvar:: Q3PY_ENTRYPOINT

    .. versionadded:: 0.0.1

    The Python entry point which q3py shall use, in the form
    of :samp:`{module}:{method}`.

    .. warning::
        This is a required setting. Without it, q3py could not do
        anything and thus exits with an error.

    .. seealso::
        :c:macro:`Q3PY_ENV_ENTRYPOINT`
        :c:func:`init_python()`


Quake 3
-------

In its default configuration Quake 3 does not load shared libraries (such
as q3py) for its game modules. Therefor you need to configure the
:ref:`ioquake3 cvar vm_game <ioquake3:cvar-vm_game>` to use q3py as the
``game`` module, ``vm_cgame`` for the ``cgame`` module and ``vm_ui`` for
the ``ui`` module respectively.

You can do so either via a ``+set vm_game 0`` command line option at
Quake 3 startup or by adding ``set vm_game 0`` to your Quake 3 ``.cfg``.

Python
------

In order for Python to find the module you specified as your q3py
entry point above, you need to tell it where to look at.

You do so by adding the folder that contains the Python module to
the :envvar:`PYTHONPATH` environment variable.

.. todo:: virtualenv, venv, activate_this.py
