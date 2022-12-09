#!/usr/bin/env python3

import sys 
f = open(sys.argv[1], 'r')
a = []
b = []
for line in f.readlines():
  s = line.split()
  if s[0] == 'Time' and s[3] == 'pivot':
    p = s[10].split('[')
    a.append(float(p[0]))
  if s[0] == 'Time' and s[1] == 'difference':
    p = s[3].split('[')
    b.append(int(p[0]))

n = len(b)
i = 0
while i < n:
    print(a[i]/(1000 * b[i]))
    i += 3

#for i in range(len(b)):
# print(str(a[(int)(i/3) ]) + '\t' + str(b[i]))
#print(a[i]/(1000 * b[i]))
# print(str(b[i]))

# print(*a, sep='\n')
# print(*b, sep='\n')
