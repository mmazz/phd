from ckks import *
import time
import numpy as np
from bitarray import bitarray
from copy import deepcopy
import matplotlib.pyplot as plt
# q0=scale=P=1024, level=2

diff_values = 0.2
arithmetic = True
"""
La multiplicacion con nivelacion de level funciona con
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0
"""
sparce_ternary = False
M = 8
N = int(M/2)
roundCS = 2
h = 2
scale = 2**9
level = 3
q0 = 2**4
P = scale**level*q0

print("CKKS in Python")
print()
print("Parameters:")
print("N: ", int(M/2), ",    q0: ", q0, ",   Scale: ", scale, ",   Levels: ", level , ", bits q:", np.log2(P))
z0=np.zeros(int(N/2))
for i in range(int(N/2)):
    z0[i] = 2*i/N+2/N

encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=arithmetic, verbose=False)
secret = encoder.keyGen(sparce_ternary)
m0, nu0 = encoder.encode(z0, scale)
c = encoder.encrypt(m0, nu0)
print(c) # Todos los coeficientes son positivos, tiene que ver por como mapeo el espacio dle modulo? Creo que Hilder
# me hnabia dicho que se podia
m_bit_decode = encoder.decrypt(c, secret)
z_bit_decrypt = encoder.decode(m_bit_decode, scale)
size_m = int(N/2)
z_bit_real = [0]*size_m
print(z_bit_decrypt)
for i in range(size_m):
    z_bit_real[i] = np.round(z_bit_decrypt[i].real, roundCS)


#mask = '0b111111111111111111111111111111111111111111111111111111111111111'
mask = '000000000000000000000000000000000000000000000000000000000000000'
# Ver que onda el np.int64, se supone que llega a 2^64 en vez de 2^62

# Confirmar que en memoria la lista y/o array [a,b,c], esta en memoria con los lsd de c en la parte de lsd de la memoria
# Si saco la redefinicion de index_bit e index, da muchas partes que desencripta bien.... ver que onda
# En particular es el index el que me modifica mucho.
# creo que estaba haciendo bien en el uso de bit_flip, pero despues en el range donde lo llamo, le asignaba valores
# diferentes. Ahi puse un reverse range, asi llama la lista de ultimo a primero. Tiene sentido? Creo que no
# Supongo que en memoria estan al reves. en lsd va el elemento 0, y asi
def bit_flip(message, mask, index_bit, index):
    temp = list(mask)
    # asi es de menos significativo a mas
    index_bit = len(mask)-index_bit-1
    temp[index_bit] = '1'
    mask = "".join(temp)
    # asi es de menos significativo a mas en elementos de la lista
    #index = 4-1-index
    mask64 = np.int64(int(mask,2))

    # El encoding son flotantes, y el encriptado son enteros.
    test = np.float64(1)
    if type(message[index])==type(test):
        message[index] = np.float64(np.int64(message[index])^mask64)
    else:
        message[index] = message[index]^mask64
    return message


def bit_flip_test(m_bit, encoder, secret, scale, roundCS, z):
    size_N = len(z)
    m_bit_decode = encoder.decrypt(m_bit, secret)
    z_bit_decrypt = encoder.decode(m_bit_decode, scale)
    z_bit_real = [0]*size_N
    res = 1
    for i in range(size_N):
        z_bit_real[i] = np.round(z_bit_decrypt[i].real, roundCS)
        diff = z_bit_real[i]-z[i]
        if diff > diff_values:
            res = 0
            break

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

def statistic_encoding_one_flip(num_tests, z):
    list_len = len(mask)*N
    bit_flip = [0]*list_len

    for i in range(num_tests):
        encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=arithmetic, verbose=False)
        secret = encoder.keyGen(sparce_ternary)
        m, nu0 = encoder.encode(z, scale)
        bit_flip_temp = bit_flip_encoding_exploration(m, mask, encoder, secret, scale, roundCS, z)
        for j in range(len(bit_flip)):
            bit_flip[j] += bit_flip_temp[j]
    bit_flip = [100*x/num_tests for x in bit_flip]
    return bit_flip


bit_flip_stat= statistic_encoding_one_flip(20, z0)

# queda ver si siempre pasan en los mismos bits o no
# Bug: Por algun motivo, con alguna clave generada al realizar un cifrado, la seguinda componente
# queda con un valor menos.... Arreglado, el V aleatorio podia ser todo cero.


# realizar lo mismo pero con un barrido de diferentes inputs
def statistic_encrpytion_one_flip(num_tests, z):
    list_len = len(mask)*N
    bit_flip_first = [0]*list_len
    bit_flip_second = [0]*list_len

    for i in range(num_tests):
        encoder = CKKSScheme(M=M, h=h, scale=scale, P=P, level=level, q0=q0, arithmetic64=arithmetic, verbose=False)
        secret = encoder.keyGen(sparce_ternary)
        m0, nu0 = encoder.encode(z, scale)
        c = encoder.encrypt(m0, nu0)
        bit_flip_first_temp, bit_flip_second_temp = bit_flip_encryption_exploration(c, mask, encoder, secret, scale, roundCS, z)
        for j in range(len(bit_flip_first)):
            bit_flip_first[j] += bit_flip_first_temp[j]
            bit_flip_second[j] += bit_flip_second_temp[j]
    bit_flip_first = [100*x/num_tests for x in bit_flip_first]
    bit_flip_second = [100*x/num_tests for x in bit_flip_second]

    return bit_flip_first, bit_flip_second


bit_flip_first, bit_flip_second = statistic_encrpytion_one_flip(20, z0)


def statistic_encoding_one_flio_input(num_tests):
    list_len = len(mask)*N
    bit_flip = [0]*list_len
    bit_flip_temp = [0]*list_len
    z=np.zeros(int(N/2))
    for i in range(num_tests):
        for j in range(int(N/2)):
            z[j] = 2*i/100
        bit_flip_temp = statistic_encoding_one_flip(num_tests, z)
        bit_flip = [sum(x) for x in zip(bit_flip_temp, bit_flip)]
    bit_flip = [100*x/num_tests for x in bit_flip]
    return bit_flip
bit_flip_input = statistic_encoding_one_flio_input(20)


def statistic_encription_one_flio_input(num_tests):
    list_len = len(mask)*N
    bit_flip1 = [0]*list_len
    bit_flip_temp1 = [0]*list_len
    bit_flip2 = [0]*list_len
    bit_flip_temp2 = [0]*list_len
    z=np.zeros(int(N/2))
    for i in range(num_tests):
        for j in range(int(N/2)):
            z[j] = 2*i/100
        bit_flip_temp1, bit_flip_temp2 = statistic_encrpytion_one_flip(num_tests, z)
        bit_flip1 = [sum(x) for x in zip(bit_flip_temp1, bit_flip1)]
        bit_flip2 = [sum(x) for x in zip(bit_flip_temp2, bit_flip2)]
    bit_flip1 = [100*x/num_tests for x in bit_flip1]
    bit_flip2 = [100*x/num_tests for x in bit_flip2]
    return bit_flip1, bit_flip2
bit_flip_input1, bit_flip_input2 = statistic_encription_one_flio_input(20)

# porcentaje de desencriptacion correcta.
def plot_hist(bits, size_m):
    plt.plot(bits)
    for i in range(size_m+2):
        plt.axvline(x=62*i, color='r', ls='--')
    plt.xlabel("Bit change")
    plt.ylabel("Correct decryptions (%)")
    plt.show()


plot_hist(bit_flip_stat,int(N/2))
plot_hist(bit_flip_input,int(N/2))
plot_hist(bit_flip_input1,int(N/2))
plot_hist(bit_flip_input2,int(N/2))

