user manual {#manual}
================

This is the manual, which is suited for both users as well
as developers.


Download
--------

TODO: https://github.com/robo9k/q3py/archive/master.zip && autoreconf --install # make dist

TODO: Link to GitHub q3py releases


Compile
-------

To compile q3py you need a C99 compiler, the GNU autotools and the
Python 3 header files.

On a Debian based system you can install those dependencies like this;

    sudo apt-get install build-essential autotools-dev python3-dev

You can then run a typical autotools compile in the downloaded q3py
directory;

    ./configure && make


Tests
-----

If you want to run the optional tests, you need to install additional
dependencies, namely the `check` library;

    sudo apt-get install check

You can then run all the tests with;

    make check


Install
-------

TODO: Pre-built (.deb) packages

Assuming you have compiled the code, you can install it like this;

    sudo make install

Please note that this will install q3py as a system package and thus
requires root privileges (hence the use of `sudo`).

TODO: Install to local directory

You now have the q3py library installed, e.g. as `/usr/local/lib64/q3py.so`.
To make use of it however, you need to provide it to your Quake 3
installation;

    ln -s /usr/local/lib64/q3py.so ~/.q3a/q3py/qagamex86_64.so

The above command makes q3py available as a 64 bit native `game` VM to
Quake 3, in the `q3py` mod directory, for the current user.

TODO: Mention all module filenames

TODO: Mention Quake 3 forks (ioquake3, OpenArena, etc.)


Configure
---------

TODO: Nothing, as of yet


Run
---

When running q3py, you need to use a special configuration for Quake 3,
q3py itself and Python;

    PYTHONPATH=python_path Q3PY_MODULE=my_module Q3PY_METHOD=my_dllentry \
	ioq3ded +set fs_game q3py +set vm_game 0

Here, `PYTHONPATH` is telling Python where to look for modules. If the
Python module you want to use is in the current directory, you can just use
"." as its value, i.e. `PYTHONPATH="."`.

TODO: virtualenv, activate_this.py etc.

The other environment variables, `Q3PY_MODULE` and `Q3PY_METHOD`, tell q3py
which Python method from which module it should use for initialization.

Quake 3 needs to be told which mod directory to use, which is the folder
where `q3py.so` has been copied into.
Furthermore you need to configure Quake 3 to use a native VM for the q3py
module, which is `game` in this example.

If everything goes well, you should see output like the following;

> Try loading dll file /home/robo9k/.q3a/q3py/qagamex86_64.so
>
> Loading DLL file: /home/robo9k/.q3a/q3py/qagamex86_64.so
>
> Sys_LoadGameDll(/home/robo9k/.q3a/q3py/qagamex86_64.so) found vmMain function at 0xdeadbeef
>
> ]Q3PY: dllEntry called with syscall 0xcafebabe
>
> Q3PY: Entry point is 'my_module:my_dllentry'

