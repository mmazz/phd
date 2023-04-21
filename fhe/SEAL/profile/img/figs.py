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
ax.plot(degrees, memory_poly_degree, '-o', color='steelblue', lw=1, label='CKKS time computation')
ax.set_xlabel("Ring Polynomial degree (N)")
ax.set_ylabel("Memory usage (Bytes)", color="steelblue")
#ax.legend()
ax.set(xticks=degrees)
#fig.savefig('memory_ckks_N.png', bbox_inches='tight')

###


time_values = [ 23824,44129,88357,173270]
#fig,ax3 = plt.subplots(nrows=1, ncols=1, tight_layout=True )
#line, = ax3.plot(degrees, time_values, '-o', color='steelblue', lw=3, label='CKKS ')
#ax3.set_yscale('log')
#ax3.set_xlabel("Ring Polynomial degree (N)")
#ax3.set_ylabel("Time ($\mu s$)")
#ax3.legend()
#ax3.set(xticks=degrees)
#fig.savefig('time_ckks_N.png', bbox_inches='tight')


ax2=ax.twinx()
ax2.plot(degrees, time_values, '-o', color='firebrick', lw=2, label='CKKS memory usage')
ax2.set_ylabel(r'Time ($\mu s$)', color='firebrick')
ax2.set_yscale('log')
#ax2.legend()
fig.savefig('ckks_N.png', bbox_inches='tight')

###

time = [165921,271041,434388,648560,904618,1278232,1733814,2211678,2769027]
fig2,ax3 = plt.subplots(nrows=1, ncols=1, tight_layout=True )
ax3.plot(values, time, '-o', color='steelblue', lw=1, label='CKKS time computation')
ax3.set_xlabel("Multiplicative depth")
ax3.set_ylabel(r'Time ($\mu s$)',color="steelblue")
ax3.set_yscale('log')
ax3.legend()
ax3.set(xticks=values)
#fig.savefig('time_ckks_mult-depth.png', bbox_inches='tight')


###

memory_coeff = [66000, 122000, 185000, 271000, 365000, 477000, 600000, 746000, 900000]
#fig,ax3 = plt.subplots(nrows=1, ncols=1, tight_layout=True )
#line, = ax3.plot(values, memory_coeff, '-o', color='steelblue', lw=3, label='CKKS')
#ax3.set_xlabel("Multiplicative depth")
#ax3.set_ylabel("Memory usage (Bytes)")
#ax3.set_yscale('log')
#ax3.set(xticks=values)
#ax3.legend()
#fig.savefig('memory_ckks_mult-depth.png', bbox_inches='tight')



ax4=ax3.twinx()
ax4.plot(values, memory_coeff, '-o', color='firebrick', lw=2, label='CKKS memory usage')
ax4.set_ylabel("Memory usage (Bytes)", color='firebrick')
ax4.set_yscale('log')
#ax4.legend()
fig2.savefig('ckks_mult_depth.png',  bbox_inches='tight')
