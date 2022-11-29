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

test_locations = "inputs/"
test_cases = ["testprob.mps"]

def parseInput():
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

        print(A)
        print(b[rhs_names[0]])
        print(c)

        rhsB = b[rhs_names[0]]
        newA = []
        newB = []
        newC = c

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

        print(np.array(newA))
        print(newB)

        # Add variable bounds
        for bddName in boundNames:
            lowBound = varBounds[bddName]['LO']
            highBound = varBounds[bddName]['UP']
            assert(len(lowBound) == len(highBound))

            numRealVars = len(lowBound)
            numTotalVars = numRealVars

            for i in range(numRealVars):
                if lowBound[i] == -np.inf and highBound[i] == np.inf:
                    # replace this variable x_i with (x_i+ - x_i-)

                    newC = np.append(newC, newC[i] * -1)

                    # for all constraints with x, make the replacement
                    # Since we have (n + 1) variables now, we add a column to A
                    new_column = np.zeros((len(newA), 1))
                    newA = np.append(newA, new_column, axis=1)

                    # we still have len(relation) constraints
                    for j in range(len(newA)):
                        newA[j][-1] = -newA[j][i]
                elif lowBound[i] < 0:
                    # replace x_i >= lowBound[i] with x'i = x_i - lowBound[i] so x'_i >= 0
                    # x_i = x'_i + lowBound[i]

                    # update b accordingly
                    # a * x_i means a * x'_i + a * lowBound[i] on the left hand side,
                    # subtract b[j] by a * lowBound[i]
                    for j in range(len(newB)):
                        newB[j] -= lowBound[i] * newA[j][i]
                if lowBound[i] > 0:
                    newA.append(-1 * np.eye(1, numTotalVars, i).flatten())
                    newB.append(-1 * lowBound[i])
                
                if highBound[i] != np.inf:
                    print(i)
                    print(highBound[i])
                    print("\n\n\n")
                    newA.append(np.eye(1, numTotalVars, i).flatten())
                    newB.append(highBound[i])
        
        newA = np.array(newA)
        newB = np.array(newB)
        print(newA)
        print(newB)
        print(newC)

        print(varBounds)
        print(boundNames)


        with open(test_locations + test_name + '_parsed.txt', 'w') as outFile:
            outFile.write('%d %d\n' % (len(newA), len(c))) #m, n
            for rule in newA:
                outStr = ''
                for num in rule: 
                    outStr += "%f " % num
                outFile.write(outStr + '\n')
            
            outStr = ''
            for rule in newB:
                outStr += "%f " % rule
            outFile.write(outStr + '\n')

            outStr = ''
            for rule in newC:
                outStr += "%f " % rule
            outFile.write(outStr + '\n')

parseInput()
os.system("cat " + test_locations + test_cases[0] + '_parsed.txt' + ' | ' + "./simplex-release")