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
'''


dir = "../logs/log_encode/"
max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
RNS_size = 1
num_bits = 64
polynomial_size = 2048
# el 2 es por que estoy comparando las encriptaciones
total_bits_HDCompare = 2*RNS_size*num_bits*polynomial_size
num_coeff = int(polynomial_size*RNS_size)
fileN2 = ""
fileHD = ""
fileHD_RNS = ""
extra = ""
ejecute = True
verbose = 0
non_RNS = False

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
    fileN2 = "encodeN2_bounded.txt"
    fileHD = "encodeHD.txt"
    fileHD_RNS = "encodeHD_RNS.txt"
    RNS_size = 2
    total_bits_HDCompare = RNS_size*num_bits*polynomial_size
    num_coeff = int(polynomial_size*RNS_size)
    non_RNS = True

elif(sys.argv[1]==str(1)):
    print("OpenFHE with no RNS")
    fileN2 = "encodeN2_nonRNS_bounded.txt"
    fileHD = "encodeHD_nonRNS.txt"
    extra = "_nonRNS"

elif(sys.argv[1]==str(2)):
    print("OpenFHE with no NTT")
    fileN2 = "encodeN2_nonNTT_bounded.txt"
    fileHD = "encodeHD_nonNTT.txt"
    fileHD_RNS = "encodeHD_nonNTT_RNS.txt"
    RNS_size = 2
    total_bits_HDCompare = RNS_size*num_bits*polynomial_size
    num_coeff = int(polynomial_size*RNS_size)
    extra = "_nonNTT"
    non_RNS = True

elif(sys.argv[1]==str(3)):
    print("OpenFHE with no Optimizations")
    fileN2 = "encodeN2_nonOps_bounded.txt"
    fileHD = "encodeHD_nonOps.txt"
    extra = "_nonOps"

if(len(sys.argv)>2):
    verbose = sys.argv[2]

if(ejecute):
    bybits_max = []
    bybits_min = []
    bycoeff_max = []
    bycoeff_min = []

    encodingN2 = data_read(dir, fileN2, False, total_bits_HDCompare)
    encodingHD = data_read(dir, fileHD, True, total_bits_HDCompare)

    y_label = 'L2 norm (\%)'
    N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingN2, num_coeff, num_bits, True, max_diff_tot)

    plt.plot(N2_by_bits_av, color='green')
    plt.plot(bybits_max, 'firebrick')
    plt.plot(bybits_min, color='steelblue')
    plt.xlabel('Bit changed')
    plt.ylabel('Norm2 (\%)', color='green')
    plt.savefig("encode_N2_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en la codificacion, comparacion entre input/output")
        plt.show()
    plt.clf()


    plt.plot(N2_by_coeff_av, color='green')
    plt.plot(bycoeff_max, 'firebrick')
    plt.plot(bycoeff_min, color='steelblue')
    plt.xlabel('Coeff changed')
    plt.ylabel('Norm2 (\%)', color='green')
    plt.savefig("encode_N2_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en la codificacion, comparacion entre input/output")
        plt.show()
    plt.clf()

    HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD, num_coeff, num_bits, False, max_diff_tot)

    plt.plot(HD_by_bits_av, color='green')
    plt.plot(bybits_max, 'firebrick')
    plt.plot(bybits_min, color='steelblue')
    plt.xlabel('Bit changed')
    plt.ylabel('HD (\%)', color='green')
    plt.savefig("encode_HD_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
        plt.show()
    plt.clf()

    plt.plot(HD_by_coeff_av, color='green')
    plt.plot(bycoeff_max, 'firebrick')
    plt.plot(bycoeff_min, color='steelblue')
    plt.xlabel('Coeff changed')
    plt.ylabel('HD (\%)', color='green')
    plt.savefig("encode_HD_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
        plt.show()
    plt.clf()

    if(non_RNS):
        # Divido por 2 el total_bits, por que aca estoy mirando solo la mitad de la codificacion
        # Ya que solo me quede con una sola limb del RNS
        encodingHD_RNS = data_read(dir, fileHD_RNS, True, int(total_bits_HDCompare/2))
        HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD_RNS, num_coeff, num_bits, False, max_diff_tot)

        plt.plot(HD_by_bits_av, color='green')
        plt.plot(bybits_max, 'firebrick')
        plt.plot(bybits_min, color='steelblue')
        plt.xlabel('Bit changed')
        plt.ylabel('HD (\%)', color='green')
        plt.savefig("encode_HD_bitFlip"+extra+"_RNS_bybit", bbox_inches='tight')
        if(verbose):
            plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
            plt.show()
        plt.clf()

        plt.plot(HD_by_coeff_av, color='green')
        plt.plot(bycoeff_max, 'firebrick')
        plt.plot(bycoeff_min, color='steelblue')
        plt.xlabel('Coeff changed')
        plt.ylabel('HD (\%)', color='green')
        plt.savefig("encode_HD_bitFlip"+extra+"_RNS_bycoeff", bbox_inches='tight')
        if(verbose):
            plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
            plt.show()
        plt.clf()
