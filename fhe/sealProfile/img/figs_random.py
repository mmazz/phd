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

dir = "../log_nonRNS_nonNTT/encryption_c_nonNTT_nonRNS_"
df = pd.read_csv(dir+"0_0"+'.txt', header=None,  skip_blank_lines=False)

df = df.iloc[1:,:]
temp = df[df.columns[0]].to_numpy()
bitflip = np.zeros(len(temp))
for i in range(3):
    for j in range(3):
        path = dir+str(i)+"_"+str(0)+'.txt'
        print(path)
        df = pd.read_csv(path, header=None,  skip_blank_lines=False)
        df = df.iloc[1:,:]
        encoding = df[df.columns[0]].to_numpy(dtype='float')
        bitflip = bitflip + encoding
bitflip = bitflip/9*100

bitflip_split = bitflip.reshape(int(len(bitflip)/50),50)

by_coeff = bitflip_split.sum(axis=1)
by_bits = bitflip_split.sum(axis=0)
by_coeff = by_coeff/by_coeff.max()*100
by_bits = by_bits/by_bits.max()*100

plt.plot(by_coeff, color='steelblue',linewidth=5.0)
plt.ylim((0,101))
plt.xlabel('Polynomail cofficient')
plt.ylabel('Correct decryption(\%)')
plt.savefig("by_coeff.png", dpi=800, format="png",bbox_inches="tight")
plt.show()

plt.plot(by_bits, color='steelblue',linewidth=5.0)
plt.ylim((-1,105))
plt.xlabel('Number of Bit in cofficient')
plt.ylabel('Correct decryption(\%)')
plt.savefig("by_bits.png", dpi=800, format="png", bbox_inches="tight")
#plt.show()

