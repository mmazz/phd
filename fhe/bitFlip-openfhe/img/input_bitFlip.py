import matplotlib.pyplot  as plt
import numpy as np
import matplotlib
import pandas as pd

plt.rcParams.update({
    "text.usetex": True,
})
matplotlib.rcParams.update({'font.size': 20})
plt.rc('xtick',labelsize=16)
plt.rc('ytick',labelsize=16)


bounded = True
max_diff = 255
input_size = 28*28
max_diff_tot = np.sqrt(max_diff**2 * input_size)
RNS_size = 2
num_bits = 64
polynomial_size = 2048
total_bits = RNS_size*num_bits*polynomial_size
total_bits_input = num_bits*input_size
num_coeff = int(polynomial_size*RNS_size)

dir = "../logs/log_input/"
fileHD = "inputHD.txt"
fileHD_encrypt = "inputHD_encrypt.txt"
fileHD_output= "inputHD_output.txt"
fileN2 =  "inputN2.txt"
fileN2_bounded =  "inputN2_bounded.txt"

print(f"Total number of bits: {total_bits}")
# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
# Ademas me guardo los maximos y minimos de cada fila columna para poder tener una especie de desviacion estanrd pero mas abrupta
def data_reshape(encoding):
    bitflip_split = np.reshape(encoding, (input_size, num_bits))
    bycoeff_max = []
    bycoeff_min = []
    for row in bitflip_split:
        bycoeff_max.append(max(row))
        bycoeff_min.append(min(row))

    bybits_max = []
    bybits_min = []
    for column in bitflip_split.T:
        bybits_max.append(max(column))
        bybits_min.append(min(column))

    by_coeff = bitflip_split.sum(axis=1)
    by_bits = bitflip_split.sum(axis=0)
    by_coeff_av = (by_coeff/num_bits)
    by_bits_av = (by_bits/input_size)
    return by_coeff_av, by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min
def data_reshape(encoding, num_rows, num_cols, bounded):
    if (bounded):
        encoding = encoding*100/max_diff_tot
    bitflip_split = np.reshape(encoding, (num_rows, num_cols))
    bycoeff_max = []
    bycoeff_min = []
    for row in bitflip_split:
        bycoeff_max.append(max(row))
        bycoeff_min.append(min(row))

    bybits_max = []
    bybits_min = []
    for column in bitflip_split.T:
        bybits_max.append(max(column))
        bybits_min.append(min(column))

    by_coeff = bitflip_split.sum(axis=1)
    by_bits = bitflip_split.sum(axis=0)
    by_coeff_av = (by_coeff/num_cols)
    by_bits_av = (by_bits/num_rows)
    return by_coeff_av, by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min


def data_read(dir, file, HD, total_bits):
    df = pd.read_csv(dir+file, header=None,  skip_blank_lines=False)
    df = df.iloc[1:,:]
    input = df[df.columns[0]].to_numpy(dtype='float')
    if (HD):
        input = input/(total_bits)*100
    print(f"{fileHD}: {input.mean()}")
    return input
bybits_max = []
bybits_min = []
bycoeff_max = []
bycoeff_min = []


inputHD = data_read(dir, fileHD, True, total_bits)
HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min = data_reshape(inputHD, input_size, num_bits, False)

plt.title("Input cambia 1 bit: HD entre encodings")
plt.plot(HD_by_bits_av, color='firebrick')
plt.plot(bybits_max, 'orange')
plt.plot(bybits_min, color='green')
plt.xlabel('Bit changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_bitFlip_bybit", bbox_inches='tight')
plt.show()

plt.title("Input cambia 1 bit: HD encodings")
plt.plot(HD_by_coeff_av, color='firebrick')
plt.plot(bycoeff_max, 'orange')
plt.plot(bycoeff_min, color='green')
plt.xlabel('Coeff changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_bitFlip_bycoeff", bbox_inches='tight')
plt.show()

inputHD_encrypt = data_read(dir, fileHD_encrypt, True, 2*total_bits)
HD_by_coeff_av, HD_by_bits_av , bycoeff_max, bycoeff_min, bybits_max, bybits_min= data_reshape(inputHD_encrypt, input_size, num_bits, False)

plt.title("Input cambia 1 bit: HD entre encriptados")
plt.plot(HD_by_bits_av, color='firebrick')
plt.plot(bybits_max, 'orange')
plt.plot(bybits_min, color='green')
plt.xlabel('Bit changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_encrpyt_bitFlip_bybit", bbox_inches='tight')
plt.show()

plt.title("Input cambia 1 bit: HD entre encriptados")
plt.plot(HD_by_coeff_av, color='firebrick')
plt.plot(bycoeff_max, 'orange')
plt.plot(bycoeff_min, color='green')
plt.xlabel('Coeff changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_encrypt_bitFlip_bycoeff", bbox_inches='tight')
plt.show()


inputN2 = data_read(dir, fileN2_bounded, False, total_bits)
N2_by_coeff_av, N2_by_bits_av , bycoeff_max, bycoeff_min, bybits_max, bybits_min= data_reshape(inputN2, input_size, num_bits, True)


y_label = 'L2 norm'
if (bounded):
    y_label = y_label + ' (\%)'

plt.title("Input cambia 1 bit: N2 entre outputs")
plt.plot(N2_by_bits_av, color='firebrick')
plt.plot(bybits_max, 'orange')
plt.plot(bybits_min, color='green')
plt.xlabel('Bit changed')
plt.ylabel('Norm2 (\%)', color='firebrick')
plt.savefig("input_N2_bitFlip_bybit", bbox_inches='tight')
plt.show()

plt.title("Input cambia 1 bit: N2 entre outputs")
plt.plot(N2_by_coeff_av, color='firebrick')
plt.plot(bycoeff_max, 'orange')
plt.plot(bycoeff_min, color='green')
plt.xlabel('Coeff changed')
plt.ylabel('Norm2 (\%)', color='firebrick')
plt.savefig("input_N2_bitFlip_bycoeff", bbox_inches='tight')
plt.show()


# Estoy haciendo ahora con uint8_t por eso el divido 4
inputHD_output = data_read(dir, fileHD_output, True, total_bits/4)
HD_by_coeff_av, HD_by_bits_av , bycoeff_max, bycoeff_min, bybits_max, bybits_min= data_reshape(inputHD_output, input_size, num_bits, False)

plt.title("Input cambia 1 bit: HD entre outputs")
plt.plot(HD_by_bits_av, color='firebrick')
plt.plot(bybits_max, 'orange')
plt.plot(bybits_min, color='green')
plt.xlabel('Bit changed')
plt.ylabel('HD (\%)', color='firebrick')
plt.savefig("input_HD_bitFlip_bybit", bbox_inches='tight')
plt.show()

plt.title("Input cambia 1 bit: HD entre outputs")
plt.plot(HD_by_coeff_av, color='firebrick')
plt.plot(bycoeff_max, 'orange')
plt.plot(bycoeff_min, color='green')
plt.xlabel('Coeff changed')
plt.ylabel('HD (\%)', color='firebrick')
plt.savefig("input_HD_bitFlip_bycoeff", bbox_inches='tight')
plt.show()


