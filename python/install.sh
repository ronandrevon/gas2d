#!/bin/bash

dir=$(realpath `dirname $0`)

cd $dir
swig -c++ -python pygas.i
python setup.py build_ext --inplace

#install and test library
# cd ..
# pip install
# source $dir/../.env/bin/activate
cmd='python -c "import gas;\ng = gas.Simulator();"'
echo $cmd
