#!/usr/bin/env python3

import sys
import os
import re
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

res = smps.load_mps("inputs/afiro.mps")

# add a slack variable to each inequality to get an equality