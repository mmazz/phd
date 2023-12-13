import sys
import matplotlib.pyplot  as plt
import numpy as np
from utils import *

dir = "../logs/log_encode/"
max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
RNS_size = 1
num_bits = 64
polynomial_size = 2048
total_bits = RNS_size*num_bits*polynomial_size
num_coeff = int(polynomial_size*RNS_size)
fileN2 = ""
fileHD = ""
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
    fileN2 = "encodeN2_bounded.txt"
    fileHD = "encodeHD.txt"
    RNS_size = 2
    total_bits = 2*total_bits
    num_coeff = 2*num_coeff

elif(sys.argv[1]==str(1)):
    print("OpenFHE with no RNS")
    fileN2 = "encodeN2_nonRNS_bounded.txt"
    fileHD = "encodeHD_nonRNS.txt"
    extra = "_nonRNS"

elif(sys.argv[1]==str(2)):
    print("OpenFHE with no NTT")
    fileN2 = "encodeN2_nonNTT_bounded.txt"
    fileHD = "encodeHD_nonNTT.txt"
    RNS_size = 2
    total_bits = 2*total_bits
    num_coeff = 2*num_coeff
    extra = "_nonNTT"

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

    encodingN2 = data_read(dir, fileN2, False, total_bits)
    encodingHD = data_read(dir, fileHD, True, total_bits)

    y_label = 'L2 norm (\%)'
    N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingN2, num_coeff, num_bits, True, max_diff_tot)

    plt.title("Cambio de un bit en la codificacion, comparacion entre input/output")
    plt.plot(N2_by_bits_av, color='firebrick')
    plt.plot(bybits_max, 'orange')
    plt.plot(bybits_min, color='green')
    plt.xlabel('Bit changed')
    plt.ylabel('Norm2 (\%)', color='firebrick')
    plt.savefig("encode_N2_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.show()

    plt.title("Cambio de un bit en la codificacion, comparacion entre input/output")
    plt.plot(N2_by_coeff_av, color='firebrick')
    plt.plot(bycoeff_max, 'orange')
    plt.plot(bycoeff_min, color='green')
    plt.xlabel('Coeff changed')
    plt.ylabel('Norm2 (\%)', color='firebrick')
    plt.savefig("encode_N2_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.show()

    HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD, num_coeff, num_bits, False, max_diff_tot)

    plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
    plt.plot(HD_by_bits_av, color='firebrick')
    plt.plot(bybits_max, 'orange')
    plt.plot(bybits_min, color='green')
    plt.xlabel('Bit changed')
    plt.ylabel('HD (\%)', color='firebrick')
    plt.savefig("encode_HD_bitFlip"+extra+"_bybit", bbox_inches='tight')
    if(verbose):
        plt.show()

    plt.title("Cambio de un bit en la codificacion, comparacion entre encriptaciones")
    plt.plot(HD_by_coeff_av, color='firebrick')
    plt.plot(bycoeff_max, 'orange')
    plt.plot(bycoeff_min, color='green')
    plt.xlabel('Coeff changed')
    plt.ylabel('HD (\%)', color='firebrick')
    plt.savefig("encode_HD_bitFlip"+extra+"_bycoeff", bbox_inches='tight')
    if(verbose):
        plt.show()
