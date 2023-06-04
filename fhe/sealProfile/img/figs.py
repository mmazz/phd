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


df = pd.read_csv('../data/time_profile.txt', sep=",", header=None)
values = df[0].values
df = df.iloc[:,:-1]
df = df.drop(0, axis=1)
time = df.mean(axis=1).values

df = pd.read_csv('../data/time_profile_degrees.txt', sep=",", header=None)
degrees = df[0].values
df = df.iloc[:,:-1]
df = df.drop(0, axis=1)
time_values = df.mean(axis=1).values

df = pd.read_csv('../data/memory_profile.txt', sep=",", header=None)
values = df[0].values
df = df.iloc[:,:-1]
df = df.drop(0, axis=1)
memory_coeff = df.mean(axis=1).values

df = pd.read_csv('../data/memory_profile_degrees.txt', sep=",", header=None)
degrees = df[0].values
df = df.iloc[:,:-1]
df = df.drop(0, axis=1)
memory_poly_degree = df.mean(axis=1).values

#degrees = [4096, 8192, 16384, 32768]
#values = [2, 4, 6 ,8, 10 ,12, 14, 16, 18]

#time_values = [ 23824,44129,88357,173270]
fig,ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )
ax.plot(degrees, time_values, '-o', color='steelblue', lw=3, label='CKKS')
ax.set_xlabel("Ring Polynomial degree (N)")
ax.set_ylabel("Time ($\mu s$)", color='steelblue')
ax.set_yscale('log')
ax.set(xticks=degrees)

#memory_poly_degree = [11000, 20100, 34000, 65000]
#ax2 = ax.twinx()  # instantiate a second axes that shares the same x-axis
#ax2.plot(degrees, memory_poly_degree, '-o', color='firebrick', lw=3, label='CKKS')
#ax2.set_ylabel("Memory usage (Bytes)", color='firebrick')
#ax2.set(xticks=degrees)
fig.savefig('ckks_N.png', bbox_inches='tight')

###

#time = [165921,271041,434388,648560,904618,1278232,1733814,2211678,2769027]
fig, ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )
ax.plot(values, time, '-o', color='steelblue', lw=3, label='N=32768')
ax.set_xlabel("Multiplicative depth")
ax.set_ylabel(r'Time ($\mu s$)', color='steelblue')
ax.set_yscale('log')
ax.legend()
ax.set(xticks=values)
###
#memory_coeff = [66000, 122000, 185000, 271000, 365000, 477000, 600000, 746000, 900000]
#ax2 = ax.twinx()  # instantiate a second axes that shares the same x-axis
#ax2.plot(values, memory_coeff, '-o', color='firebrick', lw=3, label='CKKS')
#ax2.set_ylabel("Memory usage (Bytes)", color='firebrick')
#ax2.set_yscale('log')
#ax2.set(xticks=values)
fig.savefig('ckks_mult_depth.png', bbox_inches='tight')

