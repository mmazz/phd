from ckks import *
import time
import numpy as np
import mpmath as mp
arithmetic64 = False
# q0=scale=P=1024, level=2
"""
La multiplicacion con nivelacion de level funciona con
scale = 2**9
level = 4
q0 = 2**4
P = scale**level*q0
"""

M = 2**3
roundCS = 2
h = 2
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0

print("CKKS in Python")
print()
print("Parameters:")
print("N: ", int(M/2), ",    q0: ", q0, ",   Scale: ", scale, ",   Levels: ", level )
z0 = np.array([1., 1.5])
z1 = np.array([2., 2.])

def realRounding(c):
    res = [0]*(len(c))
    for i in range(len(c)):
        res[i] = np.round(float(mp.re(c[i])), roundCS)
    return res

st = time.time()
encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=arithmetic64)
secret = encoder.keyGen(True)
print("Vectors:")
print("Vector 1: ", z0)
print("Vector 2: ", z1)
print()
print("Approx Bits:", np.log2(z1[1]*scale))
print()
m0, nu0 = encoder.encode(z0, scale)
m1, nu1 = encoder.encode(z1, scale)
c0 = encoder.encrypt(m0, nu0)
c1 = encoder.encrypt(m1, nu1)
print()

print("leveling mod")
c0l = encoder.cypher_leveling_mod(c0, level-2)
m0l = encoder.decrypt(c0l, secret)
z0l = encoder.decode(m0l, scale)
z0lReal = realRounding(z0l)
print(z0[0], z0lReal[0])
print(z0[1], z0lReal[1])

print("leveling 2 cyphertexts")
print(c0l.level, "vs", c1.level)
c0l2, c1l = encoder.leveling_mod(c0l, c1)
m1l = encoder.decrypt(c1l, secret)
z1l = encoder.decode(m1l, scale)
z1lReal = realRounding(z1l)
print(z1[0], "=?", z1lReal[0])
print(z1[1],"=?", z1lReal[1])
m0l = encoder.decrypt(c0l2, secret)
z0l = encoder.decode(m0l, scale)
z0lReal = realRounding(z0l)
print(z0[0], "=?", z0lReal[0])
print(z0[1],"=?", z0lReal[1])

print("leveling 2 cyphertexts")
print(c1.level, "vs", c0l.level)
c1l, c0l22 = encoder.leveling_mod(c1, c0l)
m1l = encoder.decrypt(c1l, secret)
z1l = encoder.decode(m1l, scale)
z1lReal = realRounding(z1l)
print(z1[0], "=?", z1lReal[0])
print(z1[1],"=?", z1lReal[1])
m2l = encoder.decrypt(c0l22, secret)
z2l = encoder.decode(m2l, scale)
z2lReal = realRounding(z2l)
print(z0[0], "=?", z2lReal[0])
print(z0[1], "=?", z2lReal[1])

print("add leveling 2 cyphertexts")
print(c0l.level, "vs", c1.level)
cadd = encoder.Cadd(c0l, c1)
madd = encoder.decrypt(cadd, secret)
zadd = encoder.decode(madd, scale)
zaddReal = realRounding(zadd)
print(z0[0]+z1[0], "=?", zaddReal[0])
print(z0[1]+z1[1],"=?", zaddReal[1])

print()
print("leveling rescale")
c0l = encoder.reScale(c0)
m0l = encoder.decrypt(c0l, secret)
z0l = encoder.decode(m0l, scale)
z0lReal = realRounding(z0l)
print(z0[0], z0lReal[0])
print(z0[1], z0lReal[1])
print("leveling 2 cyphertexts")
print(c0l.level, "vs", c1.level)
c0l2, c1l = encoder.leveling_rescale(c0l, c1)
m1l = encoder.decrypt(c1l, secret)
z1l = encoder.decode(m1l, scale)
z1lReal = realRounding(z1l)
print(z1[0], "=?", z1lReal[0])
print(z1[1],"=?", z1lReal[1])
m0l = encoder.decrypt(c0l2, secret)
z0l = encoder.decode(m0l, scale)
z0lReal = realRounding(z0l)
print(z0[0], "=?", z0lReal[0])
print(z0[1],"=?", z0lReal[1])

print("leveling 2 cyphertexts")
print(c1.level, "vs", c0l.level)
c1l, c0l22 = encoder.leveling_rescale(c1, c0l)
m1l = encoder.decrypt(c1l, secret)
z1l = encoder.decode(m1l, scale)
z1lReal = realRounding(z1l)
print(z1[0], "=?", z1lReal[0])
print(z1[1],"=?", z1lReal[1])
m2l = encoder.decrypt(c0l22, secret)
z2l = encoder.decode(m2l, scale)
z2lReal = realRounding(z2l)
print(z0[0], "=?", z2lReal[0])
print(z0[1], "=?", z2lReal[1])

print("add leveling 2 cyphertexts")
print(c0l.level, "vs", c1.level)
cadd = encoder.Cadd(c0l, c1)
madd = encoder.decrypt(cadd, secret)
zadd = encoder.decode(madd, scale)
zaddReal = realRounding(zadd)
print(z0[0]+z1[0], "=?", zaddReal[0])
print(z0[1]+z1[1],"=?", zaddReal[1])
et = time.time()
elapsed_time = round(et - st,5)
print('Execution time:', elapsed_time, 'seconds')
