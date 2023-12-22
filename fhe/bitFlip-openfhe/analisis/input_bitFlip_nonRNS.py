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

input_size = 28*28
RNS_size = 1
num_bits = 64
polynomial_size = 2048
total_bits = RNS_size*num_bits*polynomial_size
total_bits_input = num_bits*input_size
num_coeff = int(polynomial_size*RNS_size)

dir = "../logs/log_input/"
fileHD = "inputHD_nonRNS.txt"
fileHD_encrypt = "inputHD_encrypt_nonRNS.txt"

print(f"Total number of bits: {total_bits}")
# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
def data_reshape(encoding,input_size, num_bits):
    bitflip_split = np.reshape(encoding, (input_size, num_bits))
    by_coeff = bitflip_split.sum(axis=1)
    by_bits = bitflip_split.sum(axis=0)
    by_coeff_av = (by_coeff/num_bits)
    by_bits_av = (by_bits/input_size)
    return by_coeff_av, by_bits_av


dfHD = pd.read_csv(dir+fileHD, header=None,  skip_blank_lines=False)
dfHD = dfHD.iloc[1:,:]
inputHD = dfHD[dfHD.columns[0]].to_numpy(dtype='float')
inputHD = inputHD/(total_bits)*100
print(f"{fileHD}: {inputHD.mean()}")


HD_by_coeff_av, HD_by_bits_av = data_reshape(inputHD,input_size, num_bits)

plt.plot(HD_by_bits_av, color='firebrick')
plt.xlabel('Bit changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_bitFlip_nonRNS_bybit", bbox_inches='tight')

plt.plot(HD_by_coeff_av, color='firebrick')
plt.xlabel('Coeff changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_bitFlip_nonRNS_bycoeff", bbox_inches='tight')



dfHD_encrpyt = pd.read_csv(dir+fileHD_encrypt, header=None,  skip_blank_lines=False)
dfHD_encrypt = dfHD_encrpyt.iloc[1:,:]
inputHD_encrypt = dfHD_encrypt[dfHD_encrypt.columns[0]].to_numpy(dtype='float')
# el por 2 es que la encryptacion consiste en 2 polinomios
inputHD_encrypt = inputHD_encrypt/(2*total_bits)*100
print(f"{fileHD_encrypt}: {inputHD_encrypt.mean()}")

HD_encrypt_by_coeff_av, HD_encrypt_by_bits_av = data_reshape(inputHD_encrypt, input_size, num_bits)

plt.plot(HD_encrypt_by_bits_av, color='firebrick')
plt.xlabel('Bit changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_encrypt_bitFlip_nonRNS_bybit", bbox_inches='tight')

plt.plot(HD_encrypt_by_coeff_av, color='firebrick')
plt.xlabel('Coeff changed')
plt.ylabel('Hamming distance (\%)', color='firebrick')
plt.savefig("input_encrypt_bitFlip_nonRNS_bycoeff", bbox_inches='tight')


