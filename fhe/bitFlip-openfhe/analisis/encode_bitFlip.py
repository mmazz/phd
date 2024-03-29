import sys
import matplotlib.pyplot  as plt
import numpy as np
from utils import *
'''
Graficador de los datos del experimento sobre cambiar un bit en la etapa de codificacion.
En lina de comando poner un de los siguientes 6 casos:
    0: Con optimizaciones (OpenFHE default)
    1: Con optimizaciones multi RNS
    2: Sin NTT
    3: Sin NTT multi RNS
    4: Sin RNS
    5: Sin RNS ni NTT
Si ademas agregamos un 1, esta nos pondra titulos en los graficos y nos ira mostrando por pantalla cada grafico

La cantidad de bits que tiene la encriptacion es diferente en cada caso y por esos esos factores multiplicativos

Hay 3 tipos de datos para cada caso.
    a) La norma 2 entre el input/output
    b) El HD entre encriptacion original vs cambiado
        b1) Contemplando solo los limbs que se supone que no cambiaron
        b2) Contemplando solo el limb que se supone que cambio
'''


dir = "../logs/log_encode/"
fileN2 = ""
fileN2_bounded = ""
fileHD_C0_limbsNotChanged = ""
fileHD_C0_limbChanged = ""

fileHD_C1_limbsNotChanged = ""
fileHD_C1_limbChanged = ""
fileHD_positions= ""
extra = ""

max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
polynomial_size = 2048
num_bits = 64

RNS_size = 1
num_bits_coeff_encryption = 30

multiRNS = False
ejecute = True
verbose = 0

positions = False
by_bits = True
HD_plot = True

if(len(sys.argv)==1):
    print("Please pass the parameter to know wich type of file you are asking for")
    print("0 = OpenFHE with Optimizations")
    print("1 = OpenFHE with Optimizations multi RNS")
    print("2 = OpenFHE with no NTT")
    print("3 = OpenFHE with no NTT multi RNS")
    print("4 = OpenFHE with no RNS")
    print("5 = OpenFHE with no Optimizations")
    ejecute  = False

elif(sys.argv[1]==str(0)):
    print("OpenFHE with Optimizations")
    fileN2 = "encodeN2.txt"
    fileN2_bounded = "encodeN2_bounded.txt"
    fileHD_C0_limbsNotChanged = "encodeHD_C0_limbsNotChanged.txt"
    fileHD_C0_limbChanged = "encodeHD_C0_limbChanged.txt"

    fileHD_C1_limbsNotChanged = "encodeHD_C1_limbsNotChanged.txt"
    fileHD_C1_limbChanged = "encodeHD_C1_limbChanged.txt"
    fileHD_positions= "encodeHD_positions.txt"
    positions = True
    RNS_size = 2

elif(sys.argv[1]==str(1)):
    print("OpenFHE with Optimizations multi RNS")
    RNS_size = 5
    fileN2_bounded = "encodeN2_multiRNS_"+str(RNS_size)+"_bounded.txt"
    fileHD_C0_limbsNotChanged = "encodeHD_multiRNS_"+str(RNS_size)+"_C0_limbsNotChanged.txt"
    fileHD_C0_limbChanged = "encodeHD_multiRNS_"+str(RNS_size)+"_C0_limbChanged.txt"

    fileHD_C1_limbsNotChanged = "encodeHD_multiRNS_"+str(RNS_size)+"_C1_limbsNotChanged.txt"
    fileHD_C1_limbChanged = "encodeHD_multiRNS_"+str(RNS_size)+"_C1_limbChanged.txt"
    extra = "multiRNS_"
    multiRNS = True

elif(sys.argv[1]==str(2)):
    print("OpenFHE with no NTT")
    RNS_size = 2
    fileN2_bounded = "encodeN2_nonNTT_bounded.txt"
    fileHD_C0_limbsNotChanged = "encodeHD_nonNTT_C0_limbsNotChanged.txt"
    fileHD_C0_limbChanged = "encodeHD_nonNTT_C0_limbChanged.txt"

    fileHD_C1_limbsNotChanged = "encodeHD_nonNTT_C1_limbsNotChanged.txt"
    fileHD_C1_limbChanged = "encodeHD_nonNTT_C1_limbChanged.txt"
    extra = "nonNTT_"

elif(sys.argv[1]==str(3)):
    print("OpenFHE with no NTT multi RNS")
    RNS_size = 5
    fileN2_bounded = "encodeN2_nonNTT_multiRNS_"+str(RNS_size)+"_bounded.txt"
    fileHD_C0_limbsNotChanged = "encodeHD_nonNTT_multiRNS_"+str(RNS_size)+"_C0_limbsNotChanged.txt"
    fileHD_C0_limbChanged = "encodeHD_nonNTT_multiRNS_"+str(RNS_size)+"_C0_limbChanged.txt"
    fileHD_C1_limbsNotChanged = "encodeHD_nonNTT_multiRNS_"+str(RNS_size)+"_C1_limbsNotChanged.txt"
    fileHD_C1_limbChanged = "encodeHD_nonNTT_multiRNS_"+str(RNS_size)+"_C1_limbChanged.txt"
    extra = "nonNTT_multiRNS_"
    multiRNS = True


elif(sys.argv[1]==str(4)):
    print("OpenFHE with no RNS")
    fileN2_bounded = "encodeN2_nonRNS_bounded.txt"
    fileHD_C0_limbsNotChanged = "encodeHD_nonRNS_C0_limbsNotChanged.txt"
    fileHD_C0_limbChanged = "encodeHD_nonRNS_C0_limbChanged.txt"
    fileHD_C1_limbsNotChanged = "encodeHD_nonRNS_C1_limbsNotChanged.txt"
    fileHD_C1_limbChanged = "encodeHD_nonRNS_C1_limbChanged.txt"
    extra = "nonRNS_"
    num_bits_coeff_encryption = 60

elif(sys.argv[1]==str(5)):
    print("OpenFHE with no Optimizations")
    fileN2_bounded = "encodeN2_nonOps_bounded.txt"
    fileHD_C0_limbsNotChanged = "encodeHD_nonOps_C0_limbsNotChanged.txt"
    fileHD_C0_limbChanged = "encodeHD_nonOps_C0_limbChanged.txt"
    fileHD_C1_limbsNotChanged = "encodeHD_nonOps_C1_limbsNotChanged.txt"
    fileHD_C1_limbChanged = "encodeHD_nonOps_C1_limbChanged.txt"
    extra = "nonOps_"
    num_bits_coeff_encryption = 60

else:
    print("Please pass the parameter to know wich type of file you are asking for")
    print("0 = OpenFHE with Optimizations")
    print("1 = OpenFHE with Optimizations multi RNS")
    print("2 = OpenFHE with no NTT")
    print("3 = OpenFHE with no NTT multi RNS")
    print("4 = OpenFHE with no RNS")
    print("5 = OpenFHE with no Optimizations")
    ejecute  = False

if(len(sys.argv)>2):
    verbose = sys.argv[2]

if(ejecute):
    RNS_proportion_limbsNotChanged = (RNS_size-1)/RNS_size
    RNS_proportion_limbChanged = 1/RNS_size
    # Miro solo c0 o c1
    total_bits_encryption = RNS_size*num_bits_coeff_encryption*polynomial_size


    num_coeff = int(polynomial_size*RNS_size)

    encodingN2 = data_read(dir, fileN2)

    plt.plot(encodingN2)
    plt.ylabel('Norm2 (\%)', color='green')
    plt.xlabel('Bit changed')
    plt.savefig("encodingN2.png")
    encodingN2_bounded = data_read(dir, fileN2_bounded)


    N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingN2_bounded, num_coeff, num_bits)
    N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_N2_bounded(max_diff_tot, N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min)

    label = ""
    savename = extra+"bybit"
    title = "Cambio de un bit en la codificacion, comparacion entre input/output"
    ploter(N2_by_bits_av, bybits_max, bybits_min, verbose, label, False, by_bits, title, savename, False, RNS_size, polynomial_size)
    savename = extra+"bycoeff"
    ploter(N2_by_coeff_av, bycoeff_max, bycoeff_min, verbose, label, False, False, title, savename, multiRNS, RNS_size, polynomial_size)



    encodingHD_RNS = data_read(dir, fileHD_C0_limbChanged)
    encodingHD_RNS = 100*encodingHD_RNS/int(total_bits_encryption*RNS_proportion_limbChanged)
    HD_limbChanged_by_coeff_av, HD_limbChanged_by_bits_av, bycoeff_max_2, bycoeff_min_2, bybits_max_2, bybits_min_2  = data_reshape(encodingHD_RNS, num_coeff, num_bits)
    label = "limb changed"
    savename = extra+"C0_limbChanged_bybit"
    title = f"Cambio de un bit en la codificacion, comparacion entre encriptaciones C0 del limb modificado, RNS size: {RNS_size}"
    ploter(HD_limbChanged_by_bits_av, bybits_max_2, bybits_min_2, verbose, label, HD_plot, by_bits, title, savename, False, RNS_size, polynomial_size)
    savename = extra+"C0_limbChanged_bycoeff"
    ploter(HD_limbChanged_by_coeff_av, bycoeff_max_2, bycoeff_min_2, verbose, label, HD_plot, False, title, savename, False, RNS_size, polynomial_size)

    encodingHD_RNS = data_read(dir, fileHD_C1_limbChanged)
    encodingHD_RNS = 100*encodingHD_RNS/int(total_bits_encryption*RNS_proportion_limbChanged)
    HD_limbChanged_by_coeff_av, HD_limbChanged_by_bits_av, bycoeff_max_2, bycoeff_min_2, bybits_max_2, bybits_min_2  = data_reshape(encodingHD_RNS, num_coeff, num_bits)
    savename = extra+"C1_limbChanged_bybit"
    title = f"Cambio de un bit en la codificacion, comparacion entre encriptaciones C1 del limb modificado, RNS size: {RNS_size}"
    ploter(HD_limbChanged_by_bits_av, bybits_max_2, bybits_min_2, verbose, label, HD_plot, by_bits, title, savename, False, RNS_size, polynomial_size)
    savename = extra+"C1_limbChanged_bycoeff"
    ploter(HD_limbChanged_by_coeff_av, bycoeff_max_2, bycoeff_min_2, verbose, label, HD_plot, False, title, savename, False, RNS_size, polynomial_size)

    if(RNS_size > 1):
        encodingHD = data_read(dir, fileHD_C0_limbsNotChanged)
        encodingHD = 100*encodingHD/int(total_bits_encryption*RNS_proportion_limbsNotChanged)
        HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD, num_coeff, num_bits)
        label = "limbs not changed"
        savename = extra+"C0_limbsNotChanged_bybit"
        title = f"Cambio de un bit en la codificacion, comparacion entre encriptaciones C0 del limb modificado, RNS size: {RNS_size}"

        ploter(HD_by_bits_av, bybits_max, bybits_min, verbose, label, HD_plot, by_bits, title, savename, False, RNS_size, polynomial_size)
        savename = extra+"C0_limbsNotChanged_bycoeff"
        ploter(HD_by_coeff_av, bycoeff_max, bycoeff_min, verbose, label, HD_plot, False, title, savename, multiRNS, RNS_size, polynomial_size)

        encodingHD = data_read(dir, fileHD_C1_limbsNotChanged)
        encodingHD = 100*encodingHD/int(total_bits_encryption*RNS_proportion_limbsNotChanged)
        HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD, num_coeff, num_bits)
        savename = extra+"C1_limbsNotChanged_bybit"
        title = f"Cambio de un bit en la codificacion, comparacion entre encriptaciones C1 del limb modificado, RNS size: {RNS_size}"

        ploter(HD_by_bits_av, bybits_max, bybits_min, verbose, label, HD_plot, by_bits, title, savename, False, RNS_size, polynomial_size)
        savename = extra+"C1_limbsNotChanged_bycoeff"
        ploter(HD_by_coeff_av, bycoeff_max, bycoeff_min, verbose, label, HD_plot, False, title, savename, multiRNS, RNS_size, polynomial_size)

        if(positions):
            total_loops = RNS_size*num_bits*polynomial_size
            encoding_pos = data_read_pos(dir, fileHD_positions)
            encoding_pos = 100*encoding_pos/total_loops
            plt.plot(encoding_pos)
            plt.show()


