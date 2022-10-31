#!/bin/bash

dir=$(realpath `dirname $0`)
source $dir/../.env/bin/activate

cd $dir
swig -c++ -python pygas.i
python setup.py build_ext --inplace

#install and test library
# cd ..
# pip install
cmd='python -c "import gas;\ng = gas.Simulator();"'
echo $cmd
