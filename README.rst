====
q3py
====

.. image:: https://travis-ci.org/robo9k/q3py.svg?branch=master
    :target: https://travis-ci.org/robo9k/q3py
    :alt: q3py build status on Travis CI

.. image:: https://img.shields.io/coveralls/robo9k/q3py.svg
    :target: https://coveralls.io/r/robo9k/q3py
    :alt: q3py test coverage on coveralls


``q3py`` is a `Quake 3 <https://en.wikipedia.org/wiki/Quake_III_Arena>`_
to `Python <https://www.python.org/about/>`_ bridge.

Quake 3 allows its game modules to be loaded as a shared library. q3py
implements the required Quake 3 API and delegates all calls to a Python
module (your Python module!).
To do so, it acts both as a Python C extension module and also embedds
libpython3.

.. warning::
    **This is a work in progress and not meant for public use (yet)!**

The `q3py documentation <https://q3py.readthedocs.org/>`_ can be found
online at ``readthedocs``.

License
-------
All of q3py (the code, its documentation and build scripts) is licensed
under the `MIT license <https://tldrlegal.com/license/mit-license>`_, see
the ``LICENSE`` file.
