Call graphs
===========

The following graphs give an overview of how Quake 3, q3py, Python and
finally your Python module interact with each other.

Initialization
--------------

.. graphviz:: dot/q3py-init.gv

When the Quake 3 dedicated server or client binary is launched,
it will attempt to load several game modules, i.e. ``game``, ``cgame``
and ``ui``.
This example is looking at the server and thus ``game`` module only.

Quake 3 calls :c:func:`VM_Create()` to load a game module by its name
("qagame"), the syscall function provided by the engine
(:c:func:`SV_GameSystemCalls()`) and the mode in which to load the game
module ("vm_game", see :ref:`Quake 3 configuration <configuration-quake3>`).

Depending on the VM mode, Quake 3 then looks for a matching native library
file via :c:func:`Sys_LoadGameDll()` and shall find q3py.
Quake 3 invokes the exported ``dllEntry`` function of the game module
and passes the syscall pointer (``SV_GameSystemCalls``) as an argument.

Upon invocation of its ``dllEntry``, q3py stores this syscall pointer into
a global variable :c:data:`q3_syscall` to be used lateron.
Furthermore it initializes Python (:c:func:`init_python()`) and its own
Python extension module (:c:func:`PyInit_q3py()`).

q3py then uses its :envvar:`Q3PY_ENTRYPOINT` setting to call the given
method of your Python module and stores the result in the global variable
:c:data:`q3py_vmMain` for later use.

Once all functions and methods have returned to their caller, all of
Quake 3, q3py, Python and your Python module should have done their
minimal initialization. Depending on the Quake 3 fork, the engine
will now do an additional syscall into the game module (``GAME_INIT``) to
initialize its state.

.. note::
    Quake 3 calls ``dllEntry`` only when a map is loaded, not when one
    is restarted (it calls ``GAME_INIT`` in both cases and passes a
    ``restart`` argument).

    Furthermore there is no inverse operation such as ``dllExit``.
    Quake 3 invokes syscalls such as ``GAME_SHUTDOWN`` and unloads
    the shared library.
    q3py does not implement ``_fini`` or such (see ``man dlclose(3)``).


.. todo::
    Investigate whether we need ``_fini`` or such to shutdown Python
    or cleanup any other shared state.


Quake 3 calling q3py
--------------------

.. graphviz:: dot/quake3-calling-q3py.gv

.. todo::
    Graphviz layout with subgraph labels is unreadable.

Quake 3 calls into the game modules via syscalls, for example to initialize
the game state with ``GAME_INIT``.
Do to so, the engine calls :c:func:`VM_Call()` on a VM it created previously
and passes the syscall command (``GAME_INIT``) and arguments depending
on the syscall (in the case of ``GAME_INIT`` those are ``levelTime``,
``randomSeed`` and ``restart``).

With a shared library as game module, its exported ``vmMain`` function
is invoked with the syscall command and arguments. q3py looks up the
configured Python callable in its global :c:data:`q3py_vmMain` variable and
calls it with the syscall command and arguments. q3py then passes the
return value of the Python callable back to Quake 3.


script.py calling q3py
----------------------

.. graphviz:: dot/py-calling-q3py.gv

The game modules call back into Quake 3 via syscalls, for example to
indicate a critical error.

Your Python module can either use the q3py C or Python API. In both cases
the :c:func:`q3py_syscall()` function is invoked with the syscall command
and arguments.
q3py passes those to its :c:data:`q3_syscall` function pointer which it
obtained during initialization earlier on.

This function pointer aims at :c:func:`VM_DllSyscall()`, a compability
function which invokes the engine system call dispatcher,
e.g. :c:func:`SV_GameSystemCalls()`, with the syscall command and arguments.
The dispatcher then calls a function which matches the syscall command,
e.g. :c:func:`Com_Error()` for ``G_ERROR``.
