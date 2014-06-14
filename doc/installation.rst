Installation
============

As of now, q3py is only available as a source code download. As such
you will need to install all the dependencies to build it.

Download
--------

.. sourcecode:: sh

    wget https://github.com/robo9k/q3py/archive/master.zip
    unzip q3py-master.zip && cd q3py-master/

You need to download the source code of q3py. The easiest way is to
grab the current master branch tarball (a zip actually) from GitHub.
After downloading, extract the source code somewhere.


Compile
-------

.. sourcecode:: sh

    sudo apt-get install build-essential autotools-dev python3-dev

To compile q3py, you need a C99 compiler such as GCC (clang works
as well). Furthermore you need the GNU autotools and the Python 3
header files and library.

.. sourcecode:: sh

    autoreconf --install && ./configure && make

You then run the autotools to create a ``Makefile`` and such, configure
q3py and finally run ``make`` to compile everything.

Install
-------

.. sourcecode:: sh

    sudo make install

To install q3py after compilation, just run the ``install`` Make target as
root, which will install q3py as a system package.

.. todo:: Install to local directory without root

Since q3py is meant to work with any Quake 3 fork, you now need to
install it for your game.

.. sourcecode:: sh

    ln -s /usr/local/lib64/q3py.so ~/.q3a/q3py/qagamex86_64.so

This creates a symbolic link from the q3py system package to a "q3py"
game directory for the Quake 3 installation of the current user.

.. note::

    You need to configure Quake 3 to use the game directory in which
    you just installed q3py (unless you use the ``BASEGAME``, e.g.
    "baseq3").
