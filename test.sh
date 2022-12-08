#!/usr/bin/env bash

for i in {1..10}
do 
  echo "Parallel with $i threads"
  export OMP_NUM_THREADS=$i

  for j in {1..3}
  do
    for k in {2000 4000, 8000, 16000, 20000}
      do 
        ./simplex-openmp $k $k
      done
  done
done
