
import sys
import matplotlib.pyplot  as plt
import numpy as np
from utils import *
'''
Graficador de los datos del experimento sobre cambiar un bit en el input.
En lina de comando poner un de los siguientes 4 casos:
    0: Con optimizaciones (OpenFHE default)
    1: Sin RNS
Si ademas agregamos un 1, esta nos pondra titulos en los graficos y nos ira mostrando por pantalla cada grafico
'''

dir = "../logs/log_input/"
max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
RNS_size = 1
num_bits = 64
polynomial_size = 2048
total_bits = num_bits*input_size
total_bits_encode = RNS_size*num_bits*polynomial_size
total_bits_encrypt = 2*RNS_size*num_bits*polynomial_size
num_coeff = int(input_size)
fileN2 = ""
fileHD_encode = ""
fileHD_encrypt = ""
extra = ""
ejecute = True
verbose = 0



print(sys.argv[0], sys.argv[1])
if(len(sys.argv)==1):
    print("Please pass the parameter to know wich type of file you are asking for")
    print("0 = OpenFHE with Optimizations")
    print("1 = OpenFHE with no RNS")
    ejecute  = False

elif(sys.argv[1]==str(0)):
    print("OpenFHE with Optimizations")
    fileN2 = "inputN2_bounded.txt"
    fileHD_encode = "inputHD.txt"
    fileHD_encrypt = "inputHD_encrypt.txt"
    RNS_size = 2
    total_bits_encode = RNS_size*num_bits*polynomial_size
    total_bits_encrypt = 2*RNS_size*num_bits*polynomial_size

elif(sys.argv[1]==str(1)):
    print("OpenFHE with no RNS")
    fileN2 = "inputN2_nonRNS_bounded.txt"
    fileHD_encode = "inputHD_nonRNS.txt"
    fileHD_encrypt = "inputHD_encrypt_nonRNS.txt"
    extra = "_nonRNS"

if(len(sys.argv)>2):
    verbose = sys.argv[2]

if(ejecute):
    bybits_max = []
    bybits_min = []
    bycoeff_max = []
    bycoeff_min = []

    outputN2 = data_read(dir, fileN2, False, total_bits)
    encodingHD = data_read(dir, fileHD_encode, True, total_bits_encode)
    encryptHD = data_read(dir, fileHD_encrypt, True, total_bits_encrypt)

    y_label = 'L2 norm (\%)'
    N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(outputN2, num_coeff, num_bits, True, max_diff_tot)

    plt.plot(N2_by_bits_av, color='green')
    plt.plot(bybits_max, 'firebrick')
    plt.plot(bybits_min, color='steelblue')
    plt.xlabel('Bit changed')
    plt.ylabel('Norm2 (\%)', color='green')
    plt.savefig("input_N2_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en el input, comparacion entre input/output")
        plt.show()
    plt.clf()


    plt.plot(N2_by_coeff_av, color='green')
    plt.plot(bycoeff_max, 'firebrick')
    plt.plot(bycoeff_min, color='steelblue')
    plt.xlabel('Coeff changed')
    plt.ylabel('Norm2 (\%)', color='green')
    plt.savefig("input_N2_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en el input, comparacion entre input/output")
        plt.show()
    plt.clf()

    HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD, num_coeff, num_bits, False, max_diff_tot)

    plt.plot(HD_by_bits_av, color='green')
    plt.plot(bybits_max, 'firebrick')
    plt.plot(bybits_min, color='steelblue')
    plt.xlabel('Bit changed')
    plt.ylabel('HD (\%)', color='green')
    plt.savefig("input_HD_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en el input, comparacion entre codificaciones")
        plt.show()
    plt.clf()

    plt.plot(HD_by_coeff_av, color='green')
    plt.plot(bycoeff_max, 'firebrick')
    plt.plot(bycoeff_min, color='steelblue')
    plt.xlabel('Coeff changed')
    plt.ylabel('HD (\%)', color='green')
    plt.savefig("input_HD_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en el input, comparacion entre codificaciones")
        plt.show()
    plt.clf()


    encryptHD_by_coeff_av, encryptHD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encryptHD, num_coeff, num_bits, False, max_diff_tot)

    plt.plot(encryptHD_by_bits_av, color='green')
    plt.plot(bybits_max, 'firebrick')
    plt.plot(bybits_min, color='steelblue')
    plt.xlabel('Bit changed')
    plt.ylabel('HD (\%)', color='green')
    plt.savefig("input_HDencrypt_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en el input, comparacion entre encriptaciones")
        plt.show()
    plt.clf()

    plt.plot(encryptHD_by_coeff_av, color='green')
    plt.plot(bycoeff_max, 'firebrick')
    plt.plot(bycoeff_min, color='steelblue')
    plt.xlabel('Coeff changed')
    plt.ylabel('HD (\%)', color='green')
    plt.savefig("input_HDencrypt_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en el input, comparacion entre encriptaciones")
        plt.show()
    plt.clf()
