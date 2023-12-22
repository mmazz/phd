import sys
import matplotlib.pyplot  as plt
import numpy as np
from utils import *
'''
Graficador de los datos del experimento sobre cambiar un bit en la etapa de codificacion.
En lina de comando poner un de los siguientes 4 casos:
    0: Con optimizaciones (OpenFHE default)
    1: Sin RNS
    2: Sin NTT
    3: Sin RNS ni NTT
Si ademas agregamos un 1, esta nos pondra titulos en los graficos y nos ira mostrando por pantalla cada grafico

La cantidad de bits que tiene la encriptacion es diferente en cada caso y por esos esos factores multiplicativos
'''


dir = "../logs/log_encode/"
max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
num_bits = 64
polynomial_size = 2048
# el 2 es por que estoy comparando las encriptaciones
fileHD = ""
fileHD_RNS = ""
extra = ""
verbose = 0
non_RNS = False

fileHD = "encodeHD_multiRNS_positions_30bits.txt"
extra = "_multiRNS_positions_30bits"
RNS_size = 5
total_loops = RNS_size*num_bits*polynomial_size

num_coeff = int(2*polynomial_size*RNS_size)
non_RNS = True
RNS_proportion_limbsNotChanged = (RNS_size-1)/RNS_size
RNS_proportion_limbChanged = 1/RNS_size


if(len(sys.argv)>1):
    verbose = sys.argv[1]
with open (dir+fileHD, 'r') as f:
  # To split
  num = f.readline().split(', ')
  # To remove ' and create list
  # If u want list of string
  num_list = [x.replace('\'', '') for x in num]
print(num_list[-1])

num_list = num_list[:-1]
print(len(num_list))
print(64*polynomial_size*RNS_size*2)
print(30*polynomial_size*RNS_size*2 +34*polynomial_size*RNS_size*2)
bits0_29 = np.zeros(34*polynomial_size*RNS_size*2)
bits30_63 = np.zeros(30*polynomial_size*RNS_size*2)
count = 0
index_0 = 0
index_30 = 0

input_av = np.array(num_list, dtype=int)/total_loops


bitflip_split = np.reshape(input_av, (num_coeff, num_bits))
by_coeff = bitflip_split.sum(axis=1)
by_bits = bitflip_split.sum(axis=0)
by_coeff_av = (by_coeff/num_coeff)
by_bits_av = (by_bits/num_bits)


plt.plot(by_bits_av, color='green')
plt.xlabel('Bit changed')
plt.ylabel('HD (\%)', color='green')
plt.savefig("img/encrypr_30_60", bbox_inches='tight')
plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
plt.show()
plt.clf()


plt.plot(by_coeff_av, color='green')
plt.xlabel('Bit changed')
plt.ylabel('HD (\%)', color='green')
plt.savefig("img/encrypr_0_30", bbox_inches='tight')
plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
plt.show()
plt.clf()
