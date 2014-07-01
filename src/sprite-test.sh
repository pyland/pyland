#!/bin/bash

for i in {1..50}
do
   echo "trying $i sprites"
   sed -i bak -E "/const int num_objects = /s/[0-9]+/$i/g" main.cpp
   sed -i bak -E "/const int num_objects = /s/[0-9]+/$i/g" python_embed/interpreter.cpp
   COMPILER=g++-4.7 PYTHON_VERSION=3.2 LIBBOOST_PYTHON=boost_python-py32 make
   timeout -k 20 20 ./main.bin 
done
