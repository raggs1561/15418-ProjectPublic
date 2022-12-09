#!/usr/bin/env bash

# Find the optimal number of threads to use with this implementation. Tests with size 20k

for i in {1..8}
do 
  echo "Parallel with $i threads"
  export OMP_NUM_THREADS=$i
  for j in {1..3}
  do
      ./simplex-openmp 20000 20000
  done 
done
