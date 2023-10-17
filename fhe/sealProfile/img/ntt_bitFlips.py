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


modulus_size = 1
coeff_bits = 60
polynomial_size = 4096
total_bits = modulus_size*coeff_bits*polynomial_size
num_coeff = int(polynomial_size*modulus_size)
dir = "../logs/"
fileN2Full = f"ntt_bitFlip_N2_{polynomial_size}.txt"
fileHDFull = f"ntt_bitFlip_HD_{polynomial_size}.txt"


print(f"Total number of bits: {total_bits}")
# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
def data_reshape(encoding):
    bitflip_split = np.reshape(encoding, (num_coeff, coeff_bits))
    by_coeff = bitflip_split.sum(axis=1)
    by_bits = bitflip_split.sum(axis=0)
    by_coeff_av = (by_coeff/coeff_bits)
    by_bits_av = (by_bits/num_coeff)
    return by_coeff_av, by_bits_av

def image_creator(data, by_coeff_data, by_bits_data, metric, nameType):
    plt.plot(data)
    plt.xlabel('Encoding bit')
    plt.ylabel(f"{metric}")
    plt.show()
    plt.savefig(f"raw_{nameType}.jpg", bbox_inches="tight")

    plt.plot(by_coeff_data, color='steelblue',linewidth=5.0)
    plt.xlabel('Polynomail cofficient')
    plt.ylabel(f"{metric}")
    plt.show()
    plt.savefig(f"byCoeff_{nameType}.jpg", bbox_inches="tight")

    plt.clf()
    plt.plot(by_bits_data, color='steelblue',linewidth=5.0)
    plt.xlabel('Number of Bit in cofficient')
    plt.ylabel(f"{metric}")
    plt.show()
    plt.savefig(f"byBits_{nameType}.jpg", bbox_inches="tight")


dfN2Full = pd.read_csv(dir+fileN2Full, header=None,  skip_blank_lines=False)
dfN2Full = dfN2Full.iloc[1:,:]
encodingN2 = dfN2Full[dfN2Full.columns[0]].to_numpy(dtype='float')
print(f"Norm2: {round(encodingN2.mean(), 2)} ")

dfHDFull = pd.read_csv(dir+fileHDFull, header=None,  skip_blank_lines=False)
dfHDFull = dfHDFull.iloc[1:,:]
encodingHD = dfHDFull[dfHDFull.columns[0]].to_numpy(dtype='float')
encodingHD = encodingHD/(total_bits)*100
print(f"Hamming Distance: {round(encodingHD.mean(), 2)}%")


by_coeff_av, by_bits_av = data_reshape(encodingN2)
image_creator(encodingN2, by_coeff_av, by_bits_av, "Norm2", "Norm2")

by_coeff_av, by_bits_av = data_reshape(encodingHD)
image_creator(encodingHD, by_coeff_av, by_bits_av, "HD (\%)", "HD")




