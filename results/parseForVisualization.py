#!/usr/bin/env python3

import sys 
f = open(sys.argv[1], 'r')
a = []
b = []
for line in f.readlines():
  s = line.split()
  if s[0] == 'Parallel':
    a.append(int(s[2]))
    # print(s[2])
  if s[0] == 'Time' and s[1] == 'difference':
    p = s[3].split('[')
    b.append(int(p[0]))

res = []

n = len(b)
i = 0
while i < n:
    avg = (b[i] + b[i + 1] + b[i + 2])/3
    res.append((a[(int)(i/3)], avg) )
    i += 3
  
print(res)

#for threadCount in resDict:
#  resDict[threadCount] /= (len(b) / len(a))





# print(*a, sep='\n')
# print(*b, sep='\n')
