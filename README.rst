======================================
ISANA - The Isana Programming Language
======================================

Description
===========

Isana is a small lisp written in C++ with LLVM backend.

Features
========

Please check the FEATURE.rst file.

Installation and Usage
======================

Prerequisites
-------------

* Clang_ 3.0 or GCC_
* LLVM_ 3.0
* `Boost C++ Libraries`_ 1.47+
* cmake_ 2.8+

.. _Clang: http://clang.llvm.org
.. _GCC: http://gcc.gnu.org
.. _LLVM: http://llvm.org
.. _Boost C++ Libraries:  http://boost.org
.. _cmake: http://www.cmake.org

Install
-------

1. Install prerequisites before building isana.

2. Clone this repository::

    git clone git@github.com:foozea/isana.git
    cd isana

3. And build as following::

    mkdir build && cd build
    cmake ..
    make install

Then, you can find the executable binary in /usr/local/bin.

Usage
-----

You can run the REPL session with following command::

    isana

License
=======

| Copyright (C) 2012- Tetsuo Fujii and licensed under the New BSD License/Modified BSD License.
| For more details, please check the LICENSE file.

