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
total_bits_HDCompare = 2*RNS_size*30*polynomial_size
num_coeff = int(polynomial_size*RNS_size)
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
num_list = num_list[:-1]

array = np.array(num_list, dtype=int)/(polynomial_size*64*RNS_size)

print(array)
plt.plot(array, color='green')
plt.xlabel('Bit changed')
plt.ylabel('HD (\%)', color='green')
plt.savefig("encode_HD_bitFlip"+extra+"_bybit", bbox_inches='tight')
if(verbose):
    plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
    plt.show()
plt.clf()
