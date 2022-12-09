#!/usr/bin/env bash

# Find how performance changes as input size increases.
# Uses 8 threads for gates, 8 for m1 pro, 16 for bridges.
for i in {1..3}
do 
  echo "Parallel with 8 threads"
  export OMP_NUM_THREADS=8
  for k in {0, 2000, 4000, 8000, 16000, 20000, 32000}
    do 
      ./simplex-openmp $k $k
    done
done
