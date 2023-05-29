from ckks import *
import time
import numpy as np

# q0=scale=P=1024, level=2
"""
La multiplicacion con nivelacion de level funciona con
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0
"""
sparce_ternary = False
M = 8
roundCS = 2
h = 2
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0

def real_round(list, roundCS):
    res = []
    for i in range(len(list)):
        res.append(round(list[i].real,roundCS))
    return res


print("CKKS in Python")
print()
print("Parameters:")
print("N: ", int(M/2), ",    q0: ", q0, ",   Scale: ", scale, ",   Levels: ", level )
z0 = np.array([1., 1.5])
z1 = np.array([2., 2.])


st = time.time()
encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=True)
secret = encoder.keyGen(sparce_ternary)
print("Vectors:")
print("Vector 1: ", z0)
print("Vector 2: ", z1)
print()
m0, nu0 = encoder.encode(z0, scale)
m1, nu1 = encoder.encode(z1, scale)
c0 = encoder.encrypt(m0, nu0)
c1 = encoder.encrypt(m1, nu1)
print("Resultado real vs resultado CKKS")
print()
print("Suma: c1+c2")
cadd = encoder.Cadd(c0,c1)
madd = encoder.decrypt(cadd, secret)
zadd = encoder.decode(madd, scale)
zaddreal = real_round(zadd, roundCS)

print(z0[0]+z1[0], "=?", zaddreal[0], "   ,   ", z0[1]+z1[1], "=?", zaddreal[1])

print()
print("Multiplicacion por textoplano: c1*m2")
cmul_cte = encoder.Cmul_cte(c0, m1)
cmul_cte = encoder.reScale(cmul_cte)
mmul_cte = encoder.decrypt(cmul_cte, secret)
zmul_cte = encoder.decode(mmul_cte, scale)
zmulReal_cte = real_round(zmul_cte, roundCS)
z0z1 = [np.round(z0[0]*z1[0], roundCS), np.round(z0[1]*z1[1], roundCS)]
print(z0z1[0], "=?", zmulReal_cte[0], "   ,   ", z0z1[1], "=?", zmulReal_cte[1])

print()
print("Multiplicacion: c1*c2")
cmul = encoder.Cmul(c0, c1)
cmulRescale = encoder.reScale(cmul)
mmulRescale = encoder.decrypt(cmulRescale, secret)
zmulRescale = encoder.decode(mmulRescale, scale)
zmulReScaleReal = real_round(zmulRescale, roundCS)
print(z0z1[0], "=?", zmulReScaleReal[0], "   ,   ", z0z1[1], "=?", zmulReScaleReal[1])
print("error nu", cmulRescale.nu)
print("error B", cmulRescale.B)
print()
print("Multiplicacion con nivelacion de level: c1*(c1*c2)")
cmul2 = encoder.Cmul(c0, cmulRescale)
cmulRescale2 = encoder.reScale(cmul2)
mmulRescale2 = encoder.decrypt(cmulRescale2, secret)
zmulRescale2 = encoder.decode(mmulRescale2, scale)
zmulReScaleReal2 = real_round(zmulRescale2, roundCS)
print(z0[0]*z0z1[0], "=?", zmulReScaleReal2[0], "   ,   ", z0[1]*z0z1[1], "=?", zmulReScaleReal2[1])

print()
print("Multiplicacion Otro nivel: (c1*c2)*(c1*c2)")
cmul3 = encoder.Cmul(cmulRescale, cmulRescale)
cmulRescale3 = encoder.reScale(cmul3)
mmulRescale3 = encoder.decrypt(cmulRescale3, secret)
zmulRescale3 = encoder.decode(mmulRescale3, scale)
zmulReScaleReal3 = real_round(zmulRescale3, roundCS)
print(z0z1[0]*z0z1[0], "=?", zmulReScaleReal3[0], "   ,   ", z0z1[1]*z0z1[1], "=?", zmulReScaleReal3[1])

print("error nu", cmulRescale3.nu)
print("error B", cmulRescale3.B)
et = time.time()
elapsed_time = round(et - st, 5)
print('Execution time:', elapsed_time, 'seconds')

