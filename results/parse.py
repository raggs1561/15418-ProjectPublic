#!/usr/bin/env python3

import sys 
f = open(sys.argv[1], 'r')
a = []
b = []
for line in f.readlines():
  s = line.split()
  if s[0] == 'Parallel':
    # a.append(int(s[2]))
    print(s[2])
  if s[0] == 'Time' and s[1] == 'difference':
    p = s[3].split('[')
    b.append(int(p[0]))


for i in range(len(b)):
  # print(str(a[(int)(i/3) ]) + '\t' + str(b[i]))
  print(str(b[i]))

# print(*a, sep='\n')
# print(*b, sep='\n')
