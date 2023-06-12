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


df = pd.read_csv('../log_encoding_nonNTT_nonRNS.txt', header=None,  skip_blank_lines=False)
#values = df[0].values
df = df.iloc[1:,:]
#df = df.drop(0, axis=1)
encoding = df[df.columns[0]].values.tolist()

df = pd.read_csv('../log_encryption_c_nonNTT_nonRNS.txt', header=None, skip_blank_lines=False )
df = df.iloc[1:,:]
print(df)
#df = df.iloc[1:,:]
#encrypt_c = df.values
encrypt_c = df[df.columns[0]].values.tolist()
print(encrypt_c)

#degrees = df[0].values
#df = df.iloc[:,:-1]
#df = df.drop(0, axis=1)
#encrypt_c0 = df.mean(axis=1).values
#
#df = pd.read_csv('../log_encryption_c1_nonNTT_nonRNS.txt', sep="\n", header=None)
#degrees = df[0].values
#df = df.iloc[:,:-1]
#df = df.drop(0, axis=1)
#encrypt_c1 = df.mean(axis=1).values
#degrees = [4096, 8192, 16384, 32768]
#values = [2, 4, 6 ,8, 10 ,12, 14, 16, 18]

#time_values = [ 23824,44129,88357,173270]
#fig,ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )
plt.plot(encoding)
plt.show()
plt.plot(encrypt_c)
plt.show()

#ax.plot(encoding, '-o', color='steelblue', lw=3, label='CKKS')
#ax.set_xlabel("Ring Polynomial degree (N)")
#ax.set_ylabel("Time ($\mu s$)", color='steelblue')
#ax.set_yscale('log')
#ax.set(xticks=degrees)
#
#memory_poly_degree = [11000, 20100, 34000, 65000]
#ax2 = ax.twinx()  # instantiate a second axes that shares the same x-axis
#ax2.plot(degrees, memory_poly_degree, '-o', color='firebrick', lw=3, label='CKKS')
#ax2.set_ylabel("Memory usage (Bytes)", color='firebrick')
#ax2.set(xticks=degrees)
#fig.savefig('ckks_N.png', bbox_inches='tight')


