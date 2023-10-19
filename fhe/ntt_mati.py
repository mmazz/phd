import random
import numpy as np

def mod_inv(r):
    for i in range(2,1000):
        rinv = r*i%M
        if rinv==1:
            return i
    print("error")
    return 0

n = 4 # largo del vector que voy a armar de input
k = 0
M = 0
primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97]
for i in range(1,10):
    M = i*n+1
    if M in primes:
        print(i,M)
        k = i
        break

print(M)
r = 0
for i in range(2,100):
    r = i
    if i**((M-1))%M==1:
        print(i,r)
        break

print(r)

a = np.array([[ 1, 1    , 1   , 1],
              [ 1, r    , r**2, r**3],
              [ 1, r**2 , r**4, r**6],
              [ 1, r**3 , r**6, r**9]])
input = np.array([1, 1, 3, 1])
n_ntt = a.dot(input)%M
print(n_ntt)

aneg = np.array([[ 1, 1            , 1            , 1            ],
                 [ 1, mod_inv(r)   , mod_inv(r**2), mod_inv(r**3)],
                 [ 1, mod_inv(r**2), mod_inv(r**4), mod_inv(r**6)],
                 [ 1, mod_inv(r**3), mod_inv(r**6), mod_inv(r**9)]])
print(aneg%M)
output = mod_inv(r**2)*aneg.dot(n_ntt)%M

if (output == input).all():
    print("true")
else:
    print("false")
    print(input)
    print("vs")
    print(output)

