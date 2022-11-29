#!/usr/bin/env python3

import sys
import os
import re
import numpy as np
from pysmps import smps_loader as smps


# Usage: ./checker.py python/sequential-cpp/openmp-cpp 0/1

# load pysymps
'''
TODO: Import all of netlib/lp/data to inputs
Use the decompression there to get regular MPS files

Arguments for checker.py:
- What algorithm are we using (simplex python, simplex c++, simplex openmp)
- How many inputs we are testing (fast one with small inputs, a specific large one, all of them?)

for each test case:
    run specified program on this test case
    record time taken
    compare correctness, answers are stored in lp/data online
    
print overall time taken at end
'''

def parseInput():
    test_locations = "inputs/"
    test_cases = ["testprob.mps"]

    for test_name in test_cases:
        test_location = test_locations + test_name
        res = smps.load_mps(test_location)
        print(res)

        # LP format is AX (relation) b, and minimize cX

        relation = res[5] # L, G, E (<=, >=, ==)
        c = res[6]
        A = res[7]
        rhs_names = res[8]
        assert(len(rhs_names) == 1) # Multiple right hand sides isn't supported
        b = res[9]

        rhsB = b[rhs_names[0]]
        newA = []
        newB = []

        # Convert to standard form
        for i in range(len(relation)):
            rel = relation[i]
            if rel == 'L':
                newA.append(A[i][:])
                newB.append(rhsB[i])
            elif rel == 'E':
                newA.append(A[i][:])
                newB.append(rhsB[i])
                newA.append(-1 * A[i][:])
                newB.append(-1 * rhsB[i])
            elif rel == 'G':
                newA.append(-1 * A[i][:])
                newB.append(-1 * rhsB[i])
            else: 
                raise Exception("Unsupported relation type")
        
        boundNames = res[10]
        varBounds = res[11]

        # Add variable bounds
        for bddName in boundNames:
            lowBound = varBounds[bddName]['LO']
            highBound = varBounds[bddName]['UP']
            assert(len(lowBound) == len(highBound))
            numVars = len(lowBound)
            for i in range(len(lowBound)): 
                if lowBound[i] != -np.inf:
                    newA.append(-1 * np.eye(1, numVars, i).flatten())
                    newB.append(-1 * lowBound[i])
            for i in range(len(highBound)): 
                if highBound[i] != np.inf:
                    newA.append(np.eye(1, numVars, i).flatten())
                    newB.append(highBound[i])
        
        newA = np.array(newA)
        newB = np.array(newB)
        print(newA)
        print(newB)

        np.savetxt(test_locations + test_name + "parsed_c.txt", c, newline=" ")
        np.savetxt(test_locations + test_name + "parsed_A.txt", A, newline="\n")
        np.savetxt(test_locations + test_name + "parsed_b.txt", b[rhs_names[0]], newline=" ")

parseInput()