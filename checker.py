#!/usr/bin/env python3

import sys
import os
import re
import numpy
from pysmps import smps_loader as smps


# Usage: ./checker.py python/sequential-cpp/openmp-cpp 0/1

'''
version = sys.argv[1]
load_balance = '-lb' if sys.argv[2] == '1' else ''
prog = 'nbody-release-' + version
workers = [16, 128] if version == 'v1' else [16, 121]
'''

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

        # Write c, a, b to standard form
        c = res[6]
        A = res[7]
        rhs_names = res[8]
        b = res[9]

        numpy.savetxt(test_locations + test_name + "parsed_c.txt", c, newline=" ")
        numpy.savetxt(test_locations + test_name + "parsed_A.txt", A, newline="\n")
        numpy.savetxt(test_locations + test_name + "parsed_b.txt", b[rhs_names[0]], newline=" ")

parseInput()