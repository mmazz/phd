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
RNS_proportion_limbsNotChanged = 1/2
RNS_proportion_limbChanged = 1/2


print(sys.argv[0], sys.argv[1])
if(len(sys.argv)==1):
    print("Please pass the parameter to know wich type of file you are asking for")
    print("0 = OpenFHE with Optimizations")
    print("1 = OpenFHE with no RNS")
    print("2 = OpenFHE with no NTT")
    print("3 = OpenFHE with no Optimizations")
    print("4 = OpenFHE with Optimizations multi RNS")
    ejecute  = False

elif(sys.argv[1]==str(0)):
    print("OpenFHE with Optimizations")
    fileN2 = "encodeN2_bounded.txt"
    fileHD = "encodeHD_limbsNotChanged.txt"
    fileHD_RNS = "encodeHD_limbChanged.txt"
    RNS_size = 2
    total_bits_HDCompare = 2*RNS_size*num_bits*polynomial_size
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
    total_bits_HDCompare = 2*RNS_size*num_bits*polynomial_size
    num_coeff = int(polynomial_size*RNS_size)
    extra = "_nonNTT"
    non_RNS = True


elif(sys.argv[1]==str(3)):
    print("OpenFHE with no Optimizations")
    fileN2 = "encodeN2_nonOps_bounded.txt"
    fileHD = "encodeHD_nonOps.txt"
    extra = "_nonOps"


elif(sys.argv[1]==str(4)):
    print("OpenFHE with Optimizations multi RNS")
    fileN2 = "encodeN2_multiRNS_bounded.txt"
    fileHD = "encodeHD_multiRNS_limbsNotChanged.txt"
    fileHD_RNS = "encodeHD_multiRNS_limbChanged.txt"
    extra = "_multiRNS"
    RNS_size = 5
    total_bits_HDCompare = 2*RNS_size*num_bits*polynomial_size
    num_coeff = int(polynomial_size*RNS_size)
    non_RNS = True
    RNS_proportion_limbsNotChanged = (RNS_size-1)/RNS_size
    RNS_proportion_limbChanged = 1/RNS_size

elif(sys.argv[1]==str(5)):
    print("OpenFHE with Optimizations multi RNS")
    fileN2 = "encodeN2_multiRNS_bounded_30bits.txt"
    fileHD = "encodeHD_multiRNS_limbsNotChanged_30bits.txt"
    fileHD_RNS = "encodeHD_multiRNS_limbChanged_30bits.txt"
    extra = "_multiRNS_30bits"
    RNS_size = 5
    total_bits_HDCompare = 2*RNS_size*30*polynomial_size
    num_coeff = int(polynomial_size*RNS_size)
    non_RNS = True
    RNS_proportion_limbsNotChanged = (RNS_size-1)/RNS_size
    RNS_proportion_limbChanged = 1/RNS_size


if(len(sys.argv)>2):
    verbose = sys.argv[2]

if(ejecute):
    bybits_max = []
    bybits_min = []
    bycoeff_max = []
    bycoeff_min = []

    encodingN2 = data_read(dir, fileN2, False, total_bits_HDCompare)
    encodingHD = data_read(dir, fileHD, True, int(total_bits_HDCompare*RNS_proportion_limbsNotChanged))

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
    plt.axvline(0, ls='--', lw=2, label="Limb separation")
    for i in range(RNS_size):
        plt.axvline(polynomial_size*(i+1), ls='--', lw=2)
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
        encodingHD_RNS = data_read(dir, fileHD_RNS, True, int(total_bits_HDCompare*RNS_proportion_limbChanged))
        HD_limbChanged_by_coeff_av, HD_limbChanged_by_bits_av, bycoeff_max_2, bycoeff_min_2, bybits_max_2, bybits_min_2  = data_reshape(encodingHD_RNS, num_coeff, num_bits, False, max_diff_tot)
        plt.plot(HD_limbChanged_by_bits_av, color='firebrick', label="limb changed")
        plt.plot(HD_by_bits_av, color='steelblue', label="limbs not changed")

        plt.plot(bybits_max_2, 'firebrick', ls='--')
        plt.plot(bybits_min_2, color='steelblue', ls='--')
        plt.plot(bybits_max, 'firebrick')
        plt.plot(bybits_min, color='steelblue')
        plt.xlabel('Bit changed')
        plt.ylabel('HD (\%)', color='green')
        plt.legend()
        plt.savefig("encode_HD_bitFlip"+extra+"_RNS_bybit", bbox_inches='tight')
        if(verbose):
            plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones del limb modificado")
            plt.show()
        plt.clf()

        plt.plot(HD_limbChanged_by_coeff_av, color='firebrick', label="limb changed")
        plt.plot(HD_by_coeff_av, color='steelblue', label="limbs not changed")
        plt.axvline(0, ls='--', lw=2, label="Limb separation")
        for i in range(RNS_size):
            plt.axvline(polynomial_size*(i+1), ls='--', lw=2)

        plt.plot(bycoeff_max_2, 'firebrick', ls="--")
        plt.plot(bycoeff_min_2, color='steelblue', ls="--")
        plt.plot(bycoeff_max, 'firebrick')
        plt.plot(bycoeff_min, color='steelblue')
        plt.xlabel('Coeff changed')
        plt.ylabel('HD (\%)', color='green')
        plt.legend()
        plt.savefig("encode_HD_bitFlip"+extra+"_RNS_bycoeff", bbox_inches='tight')
        if(verbose):
            plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones del limb modificado")
            plt.show()
        plt.clf()


        if(sys.argv[1]==str(4)):

            HD_limbChanged_by_coeff_av[:polynomial_size] = HD_limbChanged_by_coeff_av[:polynomial_size]/2
            HD_by_coeff_av[polynomial_size:] = HD_by_coeff_av[polynomial_size:]*4/5
            plt.plot(HD_limbChanged_by_coeff_av, color='firebrick', label="limb changed")
            plt.plot(HD_by_coeff_av, color='steelblue', label="limbs not changed")
            plt.axvline(0, ls='--', lw=2, label="Limb separation")
            for i in range(RNS_size):
                plt.axvline(polynomial_size*(i+1), ls='--', lw=2)
           # plt.plot(bycoeff_max, 'firebrick')
            #plt.plot(bycoeff_min, color='steelblue')
            plt.xlabel('Coeff changed')
            plt.ylabel('HD (\%)', color='green')
            plt.legend()
            plt.savefig("encode_HD_bitFlip"+extra+"_RNS_bycoeff2", bbox_inches='tight')
            if(verbose):
                plt.title("PRUEBA")
                plt.show()
            plt.clf()
