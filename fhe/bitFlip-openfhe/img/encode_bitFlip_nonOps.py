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


RNS_size = 1
coeff_bits = 64
polynomial_size = 2048
total_bits = RNS_size*coeff_bits*polynomial_size
num_coeff = int(polynomial_size*RNS_size)
dir = "../logs/log_encode/"
fileN2 = "encodeN2_nonOps.txt"
fileHD = "encodeHD_nonOps.txt"

print(f"Total number of bits: {total_bits}")
# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
def data_reshape(encoding):
    count = 0
    bitflip_split = np.reshape(encoding, (num_coeff, coeff_bits))
    by_coeff = bitflip_split.sum(axis=1)
    by_bits = bitflip_split.sum(axis=0)
    by_coeff_av = (by_coeff/coeff_bits)
    by_bits_av = (by_bits/num_coeff)
    return by_coeff_av, by_bits_av


dfN2 = pd.read_csv(dir+fileN2, header=None,  skip_blank_lines=False)
dfN2 = dfN2.iloc[1:,:]
encodingN2 = dfN2[dfN2.columns[0]].to_numpy(dtype='float')
print(f"{fileN2}: {encodingN2.mean()}")

dfHD = pd.read_csv(dir+fileHD, header=None,  skip_blank_lines=False)
dfHD = dfHD.iloc[1:,:]
encodingHD = dfHD[dfHD.columns[0]].to_numpy(dtype='float')
encodingHD = encodingHD/(total_bits)*100
print(f"{fileHD}: {encodingHD.mean()}")


N2_by_coeff_av, N2_by_bits_av = data_reshape(encodingN2)

HD_by_coeff_av, HD_by_bits_av = data_reshape(encodingHD)

fig, ax1 = plt.subplots()

ax2 = ax1.twinx()
ax1.plot(HD_by_bits_av, color='firebrick')
ax2.plot(N2_by_bits_av, color='steelblue')

ax1.set_xlabel('Bit changed')
ax1.set_ylabel('Hamming distance (\%)', color='firebrick')
ax2.set_ylabel('L2 norm', color='steelblue')
plt.savefig("encode_bitFlip_nonOps_bybit")
plt.show()


fig, ax1 = plt.subplots()

ax2 = ax1.twinx()
ax1.plot(HD_by_coeff_av, color='firebrick')
ax2.plot(N2_by_coeff_av, color='steelblue')

ax1.set_xlabel('Coeff changed')
ax1.set_ylabel('Hamming distance (\%)', color='firebrick')
ax2.set_ylabel('L2 norm', color='steelblue')
plt.savefig("encode_bitFlip_nonOps_bycoeff")
plt.show()






