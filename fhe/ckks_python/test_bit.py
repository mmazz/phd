from ckks import *
import time
import numpy as np
from bitarray import bitarray
from copy import deepcopy
import matplotlib.pyplot as plt
# q0=scale=P=1024, level=2

diff_values = 0.2
"""
La multiplicacion con nivelacion de level funciona con
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0
"""
aritmetic = True
sparce_ternary = False
M = 8
N = 4
roundCS = 1
h = 2
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0

print("CKKS in Python")
print()
print("Parameters:")
print("N: ", int(M/2), ",    q0: ", q0, ",   Scale: ", scale, ",   Levels: ", level )
z = np.array([1., 1.5])

mask = '00000000000000000000000000000000000000000000000000000000000000'

def bit_flip(message, mask, index_bit, index):
    temp = list(mask)
    temp[index_bit] = '1'
    mask = "".join(temp)
    message[index] = int(bin(int(message[index])),2)^int(mask,2)
    return message

def bit_flip_test(m_bit, encoder, secret, scale, roundCS, z):
    m_bit_decode = encoder.decrypt(m_bit, secret)
    z_bit_decrypt = encoder.decode(m_bit_decode, scale)
    z_bit_real = [0,0]
    z_bit_real[0] = np.round(z_bit_decrypt[0].real, roundCS)
    z_bit_real[1] = np.round(z_bit_decrypt[1].real, roundCS)
    #print(z_bit_real)
    if abs(z_bit_real[0]-z[0])<diff_values and abs(z_bit_real[1] - z[1])< diff_values:
        res = 1
    else:
        res = 0
    return res


def bit_flip_encoding_exploration(m, mask, encoder, secret, scale, roundCS, z):
    res = []
    m_temp = deepcopy(m)
    for i in range(N):
        for j in range(len(mask)):
            m_temp_value = m_temp[i]
            m_temp = bit_flip(m_temp, mask, j, i)
            c_temp = encoder.encrypt(m_temp, 0)
            test = bit_flip_test(c_temp, encoder, secret, scale, roundCS, z)
            res.append(test)
            m_temp[i] = m_temp_value
    return res


def bit_flip_encryption_exploration(c, mask, encoder, secret, scale, roundCS, z):
    res_first = []
    res_second = []
    c_temp = deepcopy(c)
    for i in range(N):
        for j in range(len(mask)):
            c_temp_value = c_temp.first[i]
            c_temp.first = bit_flip(c_temp.first, mask, j, i)
            test = bit_flip_test(c_temp, encoder, secret, scale, roundCS, z)
            res_first.append(test)
            c_temp.first[i] = c_temp_value
    for i in range(N):
        for j in range(len(mask)):
            c_temp_value = c_temp.second[i]
            c_temp.second = bit_flip(c_temp.second, mask, j, i)
            test = bit_flip_test(c_temp, encoder, secret, scale, roundCS, z)
            res_second.append(test)
            c_temp.second[i] = c_temp_value
    return res_first, res_second

def statistic_encoding_one_flip(num_tests):
    list_len = len(mask)*N
    bit_flip = [0]*list_len

    for i in range(num_tests):
        encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=aritmetic, verbose=False)
        secret = encoder.keyGen(sparce_ternary)
        m, nu0 = encoder.encode(z, scale)
        bit_flip_temp = bit_flip_encoding_exploration(m, mask, encoder, secret, scale, roundCS, z)
        for j in range(len(bit_flip)):
            bit_flip[j] += bit_flip_temp[j]

    return bit_flip


bit_flip_stat= statistic_encoding_one_flip(21)


def statistic_encrpytion_one_flip(num_tests):
    list_len = len(mask)*N
    bit_flip_first = [0]*list_len
    bit_flip_second = [0]*list_len

    for i in range(num_tests):
        encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=aritmetic, verbose=False)
        secret = encoder.keyGen(sparce_ternary)
        m0, nu0 = encoder.encode(z, scale)
        c = encoder.encrypt(m0, nu0)
        bit_flip_first_temp, bit_flip_second_temp = bit_flip_encryption_exploration(c, mask, encoder, secret, scale, roundCS, z)
        for j in range(len(bit_flip_first)):
            bit_flip_first[j] += bit_flip_first_temp[j]
            bit_flip_second[j] += bit_flip_second_temp[j]

    return bit_flip_first, bit_flip_second


bit_flip_first, bit_flip_second = statistic_encrpytion_one_flip(21)

