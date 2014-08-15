#!/bin/bash

USAGE="Usage: $0 DIRECTORY ..."

if [ -z "$1" ]
then
    echo "You must specifiy an input directory."
    echo "$USAGE"
    exit 1
fi

for i in $@
do
    echo "Stitching: $i"
    ./sanitiser.sh $i
    ./gen_job.sh $i $i.job
    ./stitcher.sh $i.job $i.png $i.fml
done
