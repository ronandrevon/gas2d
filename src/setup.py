#!/usr/bin/env python

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension


extension_module = Extension('_gas',
    sources=['pygas_wrap.cxx', 'gas_simulator.cpp',
    'vector_algebra.cpp','histogram.cpp'],
                           )

setup (name = 'pygas',
       version = '0.1',
       author      = "",
       description = """ pygas""",
       ext_modules = [extension_module],
       py_modules = ["pygas"],
       )
