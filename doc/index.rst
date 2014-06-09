.. q3py documentation master file, created by
   sphinx-quickstart on Mon Jun  9 18:26:51 2014.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to q3py's documentation!
================================

``q3py`` is a `Quake 3 <https://en.wikipedia.org/wiki/Quake_III_Arena>`_ to `Python <https://www.python.org/about/>`_ bridge.

.. warning::
    **This is a work in progress and not meant for public use (yet)!**

Quake 3 allows its game modules to be loaded as a shared library. q3py
implements the required Quake 3 API and delegates all calls to Python. To
do so, it acts both as a Python C extension module and also embedds
``libpython3``.

.. graphviz:: dot/q3py-overview.gv

q3py can not be run on its own, a separate Python module is needed to
implement the required game logic.

Contents:

.. toctree::
    :maxdepth: 2

    configuration


.. doxygenindex::


License
-------
All of q3py (the code, its documentation and build scripts) is licensed
under the `MIT license <https://tldrlegal.com/license/mit-license>`_, see the ``LICENSE`` file.


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

