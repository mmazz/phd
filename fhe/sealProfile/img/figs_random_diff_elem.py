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
#matplotlib.rcParams['text.usetex'] = False

norm2 = False
num_bits_coeff = 50
num_seeds = 3
num_inputs = 20
total_runs = num_seeds * num_inputs
if(norm2):
    dir = "../log_nonRNS_nonNTT/"
else:
    dir = "../log_nonRNS_nonNTT_elem/"
file = "encryption_c_nonNTT_nonRNS_"
df = pd.read_csv(dir+str(10)+file+str(0)+'.txt', header=None,  skip_blank_lines=False)

df = df.iloc[1:,:]
temp = df[df.columns[0]].to_numpy()
bitflip = np.zeros(len(temp))
seeds = ["10", "11", "12"]


size_df = 0
for i in range(num_seeds):
    for j in range(num_inputs):
        path = dir+seeds[i]+file+str(j)+'.txt'
        print(path)
        df = pd.read_csv(path, header=None,  skip_blank_lines=False)
        df = df.iloc[1:,:]
        encoding = df[df.columns[0]].to_numpy(dtype='float')
        if size_df != len(encoding):
            print("Change of bit lenth ", size_df)
            size_df = len(encoding)
        bitflip = bitflip + encoding

# agrego estas 9 corridas extras tambien
for i in range(3):
    for j in range(3):
        path = dir+file+str(i)+"_"+str(j)+'.txt'
        print(path)
        df = pd.read_csv(path, header=None,  skip_blank_lines=False)
        df = df.iloc[1:,:]
        encoding = df[df.columns[0]].to_numpy(dtype='float')
        bitflip = bitflip + encoding

# Aca tengo el promedio de todas las corridas por cada bit
bitflip = bitflip/(total_runs+3*3) # agrego las 9 corridas extras que tenia
#plt.plot(bitflip)
#plt.show()
#
#plt.clf()

# Lo paso a una representacion matricial, donde cada fila son los bits de un coefficiente.
print("Total bits: ", len(bitflip))
rows = int(len(bitflip)/num_bits_coeff) # Es N*2...
cols = num_bits_coeff
bitflip_split = np.reshape(bitflip, (rows, cols))

# Sumo por filas y columnas
by_coeff = bitflip_split.sum(axis=1)
by_bits = bitflip_split.sum(axis=0)



by_coeff_av = (by_coeff/cols)*100
by_bits_av = (by_bits/rows)*100

plt.plot(by_coeff_av, color='steelblue',linewidth=5.0)
plt.ylim(0,100)
plt.xlim(0,8000)
plt.axvline(x=int(4096), color='k', ls='--', label="Polynomial separation", linewidth=2)
plt.xlabel('Polynomail cofficient')
plt.ylabel('Correct decryption (\%)')
if(norm2):
    plt.savefig("by_coeff.png", dpi=800, format="png",bbox_inches="tight")
else:
    plt.savefig("by_coeff_diff.png", dpi=800, format="png",bbox_inches="tight")
plt.legend()
plt.show()
plt.clf()

plt.plot(by_bits_av, color='steelblue',linewidth=5.0)
plt.xlim(0,50)
plt.ylim(-0.5,102)
plt.xlabel('Number of Bit in cofficient')
plt.ylabel('Correct decryption(\%)')
if(norm2):
    plt.savefig("by_bits.png", dpi=800, format="png", bbox_inches="tight")
else:
    plt.savefig("by_bits_diff.png", dpi=800, format="png", bbox_inches="tight")
plt.show()

