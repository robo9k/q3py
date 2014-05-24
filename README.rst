====
q3py
====

.. image:: https://travis-ci.org/robo9k/q3py.svg?branch=master
    :target: https://travis-ci.org/robo9k/q3py
    :alt: q3py build status on Travis CI

q3py is a Quake 3 to Python bridge.

.. image:: https://robo9k.github.io/q3py/dot_q3py-overview.svg
    :target: https://robo9k.github.io/q3py/
    :alt: Overview of the q3py architecture

Q3 allows its game modules to be loaded as a shared library. q3py
implements the required Q3 API and delegates all calls to Python. To
do so, it acts both as a Python C extension module and also embedds
libpython3.

q3py is of no use on its own, a separate Python module is needed to
implement the required game logic.
A Cython module for the *game* (server) module of Q3 is in development.

**This is a work in progress and not meant for public use (yet)**

The `q3py documentation <https://robo9k.github.io/q3py/>`_ can be found
on its GitHub pages.

License
-------
All of q3py (the code, its documentation and build scripts) is licensed
under the MIT license for now, see the LICENSE file.
