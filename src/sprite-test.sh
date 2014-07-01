#!/bin/bash

for i in {1..50}
do
   echo "trying $i sprites"
   sed -i bak -E "/const int num_objects = /s/[0-9]+/$i/g" main.cpp
done
