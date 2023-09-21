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


modulus_size = 2
coeff_bits = 30
polynomial_size = 4096
total_bits = modulus_size*coeff_bits*polynomial_size
num_coeff = int(polynomial_size*modulus_size)

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


dir = "../logs/log_encode/"
fileN2Full = "encodeN2_withRNS&NTT.txt"
fileHDFull = "encodeHD_withRNS&NTT.txt"

fileN2Decode = "encodeN2_withRNS&NTT_decode.txt"
fileHDDecode = "encodeHD_withRNS&NTT_decode.txt"


#df = pd.read_csv(dir+fileN2Full, header=None,  skip_blank_lines=False)
#df = df.iloc[1:,:]
#encoding = df[df.columns[0]].to_numpy(dtype='float')
#
#encoding1 = encoding[:polynomial_size*coeff_bits]
#encoding1 = encoding1/len(encoding1)
#encoding2 = encoding[polynomial_size*coeff_bits:]
#encoding2 = encoding2/len(encoding2)
#
#encoding = encoding/(polynomial_size*coeff_bits)
#print(f"Size first mod coeff: {len(encoding1)}, and second: {len(encoding2)}")
#print(f"{fileN2Full}: {encoding1.mean()}, {encoding2.mean()} ")
#
#by_coeff_av, by_bits_av = data_reshape(encoding)
#image_creator(encoding, by_coeff_av, by_bits_av, "Norm2", "Norm2")
#
#
#df = pd.read_csv(dir+fileHDFull, header=None,  skip_blank_lines=False)
#df = df.iloc[1:,:]
#encoding = df[df.columns[0]].to_numpy(dtype='float')
#encoding = encoding/(total_bits)*100
#print(f"{fileHDFull}: {encoding.mean()}")
#
#by_coeff_av, by_bits_av = data_reshape(encoding)
#image_creator(encoding, by_coeff_av, by_bits_av, "Hamming distance", "HD")

df = pd.read_csv(dir+fileN2Decode, header=None,  skip_blank_lines=False)
df = df.iloc[147:,:]
encoding = df[df.columns[0]].to_numpy(dtype='float')
encoding = encoding/(polynomial_size*coeff_bits)
print(f"{fileN2Decode}: {encoding.mean()}")

by_coeff_av, by_bits_av = data_reshape(encoding)
image_creator(encoding, by_coeff_av, by_bits_av, "Norm2", "Norm2_decode")


df = pd.read_csv(dir+fileHDDecode, header=None,  skip_blank_lines=False)
df = df.iloc[147:,:]
encoding = df[df.columns[0]].to_numpy(dtype='float')
encoding = encoding/(total_bits)*100
print(f"{fileHDDecode}: {encoding.mean()}")

by_coeff_av, by_bits_av = data_reshape(encoding)
image_creator(encoding, by_coeff_av, by_bits_av, "Hamming distance", "HD_decode")





