import matplotlib.pyplot  as plt
import numpy as np
import matplotlib

plt.rcParams.update({
    "text.usetex": True,
})
matplotlib.rcParams.update({'font.size': 20})
plt.rc('xtick',labelsize=16)
plt.rc('ytick',labelsize=16)


degrees = [4096, 8192, 16384, 32768]
values = [2, 4, 6 ,8, 10 ,12, 14, 16, 18]

# Calculate X^2 with different Polynomail degrees.
memory_poly_degree = [11000, 20100, 34000, 65000]
fig,ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )
line, = ax.plot(degrees, memory_poly_degree, '-o', color='steelblue', lw=3, label='CKKS')
ax.set_xlabel("Ring Polynomail degree (N)")
ax.set_ylabel("Memory usage (Bytes)")
ax.legend()
ax.set(xticks=degrees)
fig.savefig('memory_ckks_N.png', bbox_inches='tight')

###

time = [165921,271041,434388,648560,904618,1278232,1733814,2211678,2769027]
fig,ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )
line, = ax.plot(values, time, '-o', color='steelblue', lw=3, label='CKKS')
ax.set_xlabel("Multiplicative depth")
ax.set_ylabel(r'Time ($\mu s$)')
ax.set_yscale('log')
ax.legend()
ax.set(xticks=values)
fig.savefig('time_ckks_mult-depth.png', bbox_inches='tight')

###

degrees = [4096,8192,16384,32768]

time_values = [ 23824,44129,88357,173270]
fig,ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )
line, = ax.plot(degrees, time_values, '-o', color='steelblue', lw=3, label='CKKS')
ax.set_yscale('log')
ax.set_ylabel("Time ($\mu s$)")
ax.set_xlabel("Polynomail degree")
ax.legend()
ax.set(xticks=degrees)
fig.savefig('time_ckks_N.png', bbox_inches='tight')

###

memory_coeff = [66000, 122000, 185000, 271000, 365000, 477000, 600000, 746000, 900000]
fig,ax = plt.subplots(nrows=1, ncols=1, tight_layout=True )

line, = ax.plot(values, memory_coeff, '-o', color='steelblue', lw=3, label='CKKS')
ax.set_xlabel("Multiplicative depth")
ax.set_ylabel("Memory usage (Bytes)")
ax.set_yscale('log')
ax.set(xticks=values)
ax.legend()
fig.savefig('memory_ckks_mult-depth.png', bbox_inches='tight')


