import sys
import matplotlib.pyplot  as plt
import numpy as np
from utils import *
'''
Graficador de los datos del experimento sobre cambiar un bit en la etapa de encriptacion.
En lina de comando poner un de los siguientes 4 casos:
    0: Con optimizaciones (OpenFHE default)
    1: Sin RNS
    2: Sin NTT
    3: Sin RNS ni NTT
Si ademas agregamos un 1, esta nos pondra titulos en los graficos y nos ira mostrando por pantalla cada grafico
'''



dir = "../logs/log_encrypt/"
max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
RNS_size = 1
num_bits = 64
polynomial_size = 2*2048
total_bits = RNS_size*num_bits*polynomial_size
num_coeff = int(polynomial_size*RNS_size)
fileN2 = ""
extra = ""
ejecute = True
verbose = 0


print(sys.argv[0], sys.argv[1])
if(len(sys.argv)==1):
    print("Please pass the parameter to know wich type of file you are asking for")
    print("0 = OpenFHE with Optimizations")
    print("1 = OpenFHE with no RNS")
    print("2 = OpenFHE with no NTT")
    print("3 = OpenFHE with no Optimizations")
    ejecute  = False

elif(sys.argv[1]==str(0)):
    print("OpenFHE with Optimizations")
    fileN2 = "encryptN2_bounded.txt"
    RNS_size = 2
    total_bits = 2*total_bits
    num_coeff = 2*num_coeff

elif(sys.argv[1]==str(1)):
    print("OpenFHE with no RNS")
    fileN2 = "encryptN2_nonRNS_bounded.txt"
    extra = "_nonRNS"

elif(sys.argv[1]==str(2)):
    print("OpenFHE with no NTT")
    fileN2 = "encryptN2_nonNTT_bounded.txt"
    RNS_size = 2
    total_bits = 2*total_bits
    num_coeff = 2*num_coeff
    extra = "_nonNTT"

elif(sys.argv[1]==str(3)):
    print("OpenFHE with no Optimizations")
    fileN2 = "encryptN2_nonOps_bounded.txt"
    extra = "_nonOps"

if(len(sys.argv)>2):
    verbose = sys.argv[2]

if(ejecute):
    bybits_max = []
    bybits_min = []
    bycoeff_max = []
    bycoeff_min = []

    encodingN2 = data_read(dir, fileN2, False, total_bits)

    y_label = 'L2 norm (\%)'
    N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingN2, num_coeff, num_bits, True, max_diff_tot)

    plt.plot(N2_by_bits_av, color='green')
    plt.plot(bybits_max, 'firebrick')
    plt.plot(bybits_min, color='steelblue')
    plt.xlabel('Bit changed')
    plt.ylabel('Norm2 (\%)', color='green')
    plt.savefig("encrypt_N2_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en la encriptacion, comparacion entre input/output")
        plt.show()
    plt.clf()


    plt.plot(N2_by_coeff_av, color='green')
    plt.plot(bycoeff_max, 'firebrick')
    plt.plot(bycoeff_min, color='steelblue')
    plt.xlabel('Coeff changed')
    plt.ylabel('Norm2 (\%)', color='green')
    plt.savefig("encrypt_N2_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en la encriptacion, comparacion entre input/output")
        plt.show()
    plt.clf()





