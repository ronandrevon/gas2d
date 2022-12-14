%module gas
%{
#define SWIG_FILE_WITH_INIT
#include "../src/simulator.hpp"
%}
%include "std_vector.i"
%include "numpy.i"

%init %{
  import_array();
%}
%apply (double* IN_ARRAY1, int DIM1) {(double* seq, int n)};

%include "typemaps.i"

%typemap(out) double [ANY] {
  int i;
  $result = PyList_New($1_dim0);
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PyFloat_FromDouble((double) $1[i]);
    PyList_SetItem($result,i,o);
  }
}

%include "../src/simulator.hpp"
