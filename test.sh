#!/bin/sh

for i in {1..10}
do 
  for j in {1..3}
  do
    echo "Parallel with $i threads"
    export OMP_NUM_THREADS=$i 
    ./simplex-openmp
  done
done
