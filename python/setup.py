#!/usr/bin/env python

"""
setup.py file for SWIG example
"""

from distutils.core import setup, Extension

src_dir ='../src'
src = ['gas_simulator.cpp','vector_algebra.cpp','histogram.cpp']
sources = ['pygas_wrap.cxx']+['%s/%s' %(src_dir,s) for s in src]

extension_module = Extension('_gas',sources=sources,)
setup (name = 'pygas',
       version = '0.1',
       author      = "",
       description = """ pygas""",
       ext_modules = [extension_module],
       py_modules  = ["pygas"],
       )
