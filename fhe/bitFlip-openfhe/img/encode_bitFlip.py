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
# Aca estoy mirando los bits del input no del encoding
total_bits_input = num_bits*input_size
num_coeff = int(polynomial_size*RNS_size)
dir = "../logs/log_encode/"
fileN2 = "encodeN2.txt"
if (bounded):
    fileN2 = "encodeN2_bounded.txt"
fileHD = "encodeHD.txt"

print(f"Total number of bits: {total_bits}")
# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
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


encodingN2 = data_read(dir, fileN2, False, total_bits)

encodingHD = data_read(dir, fileHD, True, total_bits_input)


y_label = 'L2 norm'
N2_by_coeff_av, N2_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingN2, num_coeff, num_bits, True)

if (bounded):
    y_label = y_label + ' (\%)'



plt.plot(N2_by_bits_av, color='firebrick')
plt.plot(bybits_max, 'orange')
plt.plot(bybits_min, color='green')
plt.xlabel('Bit changed')
plt.ylabel('Norm2 (\%)', color='firebrick')
plt.savefig("encode_N2_bitFlip_bybit", bbox_inches='tight')
plt.show()

plt.plot(N2_by_coeff_av, color='firebrick')
plt.plot(bycoeff_max, 'orange')
plt.plot(bycoeff_min, color='green')
plt.xlabel('Coeff changed')
plt.ylabel('Norm2 (\%)', color='firebrick')
plt.savefig("encode_N2_bitFlip_bycoeff", bbox_inches='tight')
plt.show()

HD_by_coeff_av, HD_by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min  = data_reshape(encodingHD, num_coeff, num_bits, False)

plt.plot(HD_by_bits_av, color='firebrick')
plt.plot(bybits_max, 'orange')
plt.plot(bybits_min, color='green')
plt.xlabel('Bit changed')
plt.ylabel('HD (\%)', color='firebrick')
plt.savefig("encode_HD_bitFlip_bybit", bbox_inches='tight')
plt.show()

plt.plot(HD_by_coeff_av, color='firebrick')
plt.plot(bycoeff_max, 'orange')
plt.plot(bycoeff_min, color='green')
plt.xlabel('Coeff changed')
plt.ylabel('HD (\%)', color='firebrick')
plt.savefig("encode_HD_bitFlip_bycoeff", bbox_inches='tight')
plt.show()






