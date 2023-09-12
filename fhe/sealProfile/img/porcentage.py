import matplotlib.pyplot  as plt
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
dir = "../log_encrypt_NTT/"
dir2 = "../log_encode_NTT/"

file = "encryption_c_elemDiff_withRNS.txt"
file2 = "encryption_c_withRNS.txt"
file3 = "encoding_elemDiff_withRNS.txt"
df = pd.read_csv(dir+file, header=None,  skip_blank_lines=False)
df2 = pd.read_csv(dir+file2, header=None,  skip_blank_lines=False)
df3 = pd.read_csv(dir2+file3, header=None,  skip_blank_lines=False)

df = df.iloc[1:,:]
df2 = df2.iloc[1:,:]
print(df.mean())
print(df2.mean())
print(df3.mean())


