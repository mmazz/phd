import matplotlib.pyplot  as plt
import numpy as np
import matplotlib
import pandas as pd
# import OS module
import os

# Get the list of all files and directories

plt.rcParams.update({
    "text.usetex": True,
})
matplotlib.rcParams.update({'font.size': 20})
plt.rc('xtick',labelsize=16)
plt.rc('ytick',labelsize=16)


dir = "../logs/log_encode/"

file1 = "encodeN2.txt"
file3 = "encode.txt"
file2 = "encodeN2_withRNS&NTT.txt"
file4 = "encode_withRNS&NTT.txt"

df = pd.read_csv(dir+file1, header=None,  skip_blank_lines=False)
df = df.iloc[1:,:]
encoding = df[df.columns[0]].to_numpy(dtype='float')
print(f"{file1}: {encoding.mean()}")

df = pd.read_csv(dir+file2, header=None,  skip_blank_lines=False)
df = df.iloc[1:,:]
encoding = df[df.columns[0]].to_numpy(dtype='float')
print(f"{file2}: {encoding.mean()}")
print()
df = pd.read_csv(dir+file3, header=None,  skip_blank_lines=False)
df = df.iloc[1:,:]
encoding = df[df.columns[0]].to_numpy(dtype='float')
print(f"{file3}: {encoding.mean()}")

rows = int(len(encoding)/50) # Es N*2...
cols = 50
bitflip_split = np.reshape(encoding, (rows, cols))
by_coeff = bitflip_split.sum(axis=1)
by_bits = bitflip_split.sum(axis=0)



by_coeff_av = (by_coeff/cols)
by_bits_av = (by_bits/rows)
plt.plot(encoding)
plt.show()
plt.plot(by_coeff_av, color='steelblue',linewidth=5.0)

plt.axvline(x=int(4096), color='k', ls='--', label="Polynomial separation", linewidth=2)
plt.xlabel('Polynomail cofficient')
plt.ylabel('Norm2')

plt.legend()
plt.show()
plt.clf()

plt.plot(by_bits_av, color='steelblue',linewidth=5.0)

plt.xlabel('Number of Bit in cofficient')
plt.ylabel('Norm2')

plt.show()
df = pd.read_csv(dir+file4, header=None,  skip_blank_lines=False)
df = df.iloc[1:,:]
encoding = df[df.columns[0]].to_numpy(dtype='float')
print(f"{file4}: {encoding.mean()}")

rows = int(len(encoding)/50) # Es N*2...
cols = 50
bitflip_split = np.reshape(encoding, (rows, cols))
by_coeff = bitflip_split.sum(axis=1)
by_bits = bitflip_split.sum(axis=0)



by_coeff_av = (by_coeff/cols)
by_bits_av = (by_bits/rows)
plt.plot(encoding)
plt.show()
plt.plot(by_coeff_av, color='steelblue',linewidth=5.0)

plt.axvline(x=int(4096), color='k', ls='--', label="Polynomial separation", linewidth=2)
plt.xlabel('Polynomail cofficient')
plt.ylabel('Hamming distance')

plt.legend()
plt.show()
plt.clf()

plt.plot(by_bits_av, color='steelblue',linewidth=5.0)

plt.xlabel('Number of Bit in cofficient')
plt.ylabel('Hamming distance')

plt.show()
