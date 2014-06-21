Hello world!
============

This is the classical "Hello world!" example for q3py,
:download:`q3py_hello.py <examples/q3py_hello.py>`.

.. literalinclude:: examples/q3py_hello.py
    :language: python
    :linenos:

As you can see, this is a little more involved, but to be
fair it is not the most simple way to write a hello world
program with q3py.

To run this example, take a look at the :ref:`configuration` page.
The output might look like this::

    Q3PY [INFO]: Entry point is 'q3py_hello:dll_entry'
    Python dll_entry() called
    Q3PY [INFO]: v0.0.1 initialized
    Python init_game(level_time=0, random_seed=42, restart=False)
    ********************
    ERROR: Hello, Quake 3!
    ********************
    ----- Server Shutdown (Server crashed: Hello, Quake 3!) -----
    forcefully unloading qagame vm
    ---------------------------
    ]

Hey wait, did it just say we crashed the server?

Yes, but there is a reason to this.
The purpose of a Quake 3 module is not to print "Hello world", but to
implement game logic.
We only implemented a tiny little part of the ``game`` logic (we could
have done this for ``cgame`` or ``ui`` as well).

When Quake 3 loads this Python module via q3py, it calls the ``vmMain``
function of q3py, which in turn calls our ``vm_main``, since that's what
we told q3py to do. The ``vmMain`` function acts as a generic dispatcher
for all the calls of the engine, hence the ugly arguments and argument
names.

Either way, upon initialization Quake 3 calls ``vmMain`` with the command
``GAME_INIT`` and some additional arguments specific to this call, which
are getting passed into ``init_game`` here.

In ``init_game``, we call a ``qerror`` method with the message that
we saw during the server crash.
So now in ``qerror`` we use q3py to do a "syscall" back into the Quake 3
engine and as you might have guessed, we call the ``G_ERROR`` syscall
which justs prints the message as we've seen and exits the game :)
