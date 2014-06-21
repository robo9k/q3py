Call graphs
===========

The following graphs give an overview of how Quake 3, q3py, Python and
finally the Python module interact with each other.

Initialization
--------------

.. graphviz:: dot/q3py-init.gv

When the Quake 3 dedicated server or client binary is launched,
it will attempt to load several game modules, i.e. ``game``, ``cgame``
and ``ui``.
This example is looking at the server and thus ``game`` module only.

Quake 3 calls ``VM_Create`` to load a game module by its name ("qagame"),
the syscall function provided by the engine (``SV_GameSystemCalls``) and
the mode in which to load the game module ("vm_game", see
:ref:`Quake 3 configuration <configuration-quake3>`).

Depending on the VM mode, Quake 3 then looks for a matching native library
file via ``Sys_LoadGameDll`` and shall find q3py.
Quake 3 invokes the exported ``dllEntry`` function of the game module
and passes the syscall pointer (``SV_GameSystemCalls``) as an argument.

Upon invocation of its ``dllEntry``, q3py stores this syscall pointer into
a global variable ``q3_syscall`` to be used lateron.
Furthermore it initializes Python (``init_python``) and its own Python
extension module (``PyInit_q3py``).

q3py then uses its ``Q3PY_ENTRYPOINT`` setting to call the given method
of your Python module and stores the result in the global variable
``q3py_vmMain`` for later use.

Once all functions and methods have returned to their caller, all of
Quake 3, q3py, Python and your Python module should have done their
minimal initialization. Depending on the Quake 3 fork, the engine
will now do an additional syscall into the game module (``GAME_INIT``) to
initialize its state.
