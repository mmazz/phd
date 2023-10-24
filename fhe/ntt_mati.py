import numpy as np
import random

def vandermonde(n,w):
    A = np.ones((n,n), dtype=int)
    for i in range(0,n):
        for j in range(0,n):
            A[i][j] = w**((j*i)%n)
    return A

def vandermonde_cyclic(n,q,w):
    A = np.ones((n,n), dtype=int)
    for i in range(0,n):
        for j in range(0,n):
            A[i][j] =  w**((j*i)%n)%q
    return A


def vandermonde_negacyclic(n,q,w):
    A = np.ones((n,n), dtype=int)
    for i in range(0,n):
        for j in range(0,n):
            A[i][j] = w**((2*j*i+j)%(2*n))%q
    return A

def vandermonde_Inegacyclic(n,q,w):
    A = np.ones((n,n), dtype=int)
    for i in range(0,n):
        for j in range(0,n):
            A[i][j] = w**((2*j*i+i)%(2*n))%q
    return A

def ntt(input, A, q):
    return (A@input)%q

def intt(input_ntt, Ainv, q):
    return ninv*(Ainv@input_ntt)%q


n = 4
ninv= 5761
q = 7681
input = [1, 2, 3, 4]


w = 3383
winv = 4298
A = vandermonde(n,w)%q
Ainv = vandermonde(n,winv)
input = [random.randint(0, q-1) for i in range(n)]
input_ntt = ntt(input, A, q)
output = intt(input_ntt, Ainv, q)

if (output == input).all():
    print("Linear true")
else:
    print("Linear false")
    print(input)
    print("vs")
    print(output)


A = vandermonde_cyclic(n,q,w)
Ainv = vandermonde_cyclic(n,q,winv)
input = [random.randint(0, q-1) for i in range(n)]
input = [1, 2, 3, 4]
input_ntt = ntt(input, A, q)
output = intt(input_ntt, Ainv, q)
if (output == input).all():
    print("Cyclic true")
else:
    print("Cyclic false")
    print(input)
    print("vs")
    print(output)



w = 1925
winv = 1213
A = vandermonde_negacyclic(n,q,w)
Ainv = vandermonde_Inegacyclic(n,q,winv)
input = [random.randint(0, q-1) for i in range(n)]
input = [1, 2, 3, 4]
input_ntt = ntt(input, A, q)
output = intt(input_ntt, Ainv, q)

if (output == input).all():
    print("NegaCyclic true")
else:
    print("BegaCylcic false")
    print(input)
    print("vs")
    print(output)
