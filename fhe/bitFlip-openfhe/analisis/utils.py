import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt


plt.rcParams.update({
    "text.usetex": True,
})
matplotlib.rcParams.update({'font.size': 20})
plt.rc('xtick',labelsize=16)
plt.rc('ytick',labelsize=16)

def data_read(dir, file):
    df = pd.read_csv(dir+file, header=None,  skiprows=1, skip_blank_lines=False, dtype='float64')
    input = df[df.columns[0]].to_numpy(dtype='int')
    print(f"{file}: {input.mean()}")
    return input


def data_read_pos(dir, file):
    with open (dir+file, 'r') as f:
        num = f.readline().split(', ')
        num_list = [x.replace('\'', '') for x in num]
    num_list = num_list[:-1]
    input = np.array(num_list, dtype=int)
    print(f"{file}: {input.mean()}")
    return input

# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
def data_reshape(data, num_rows, num_cols):
    bitflip_split = np.reshape(data, (num_rows, num_cols))
    bycoeff_max = []
    bycoeff_min = []
    for row in bitflip_split:
        bycoeff_max.append(max(row))
        bycoeff_min.append(min(row))

    bybits_max = []
    bybits_min = []
    for column in bitflip_split.T:
        bybits_max.append(max(column))
        bybits_min.append(min(column))

    by_coeff = bitflip_split.sum(axis=1)
    by_bits = bitflip_split.sum(axis=0)
    by_coeff_av = (by_coeff/num_cols)
    by_bits_av = (by_bits/num_rows)
    return by_coeff_av, by_bits_av, np.array(bycoeff_max), np.array(bycoeff_min), np.array(bybits_max), np.array(bybits_min)

def data_N2_bounded(max_diff_tot,  by_coeff_av, by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min):
    by_coeff_av = 100*by_coeff_av/max_diff_tot
    by_bits_av  = 100*by_bits_av/max_diff_tot
    bycoeff_max = 100*bycoeff_max/max_diff_tot
    bycoeff_min = 100*bycoeff_min/max_diff_tot
    bybits_max  = 100*bybits_max/max_diff_tot
    bybits_min  = 100*bybits_min/max_diff_tot
    return by_coeff_av, by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min

def ploter(data, data_max, data_min, verbose, label, HD_plot, by_bits, title, savename, multiRNS, RNS_size, polynomial_size):
    if(HD_plot):
        plt.plot(data, color='green', label=label)
        plt.ylabel('HD (\%)', color='green')
    else:
        plt.plot(data, color='green')
        plt.ylabel('Norm2 (\%)', color='green')
    plt.plot(data_max, 'firebrick', ls='--', label="max value")
    plt.plot(data_min, color='steelblue', ls='--', label="min value")
    if(multiRNS):
        plt.axvline(0, color='k', ls='--', lw=2, label="Limb separation")
        for i in range(RNS_size):
            plt.axvline(polynomial_size*(i+1), color='k', ls='--', lw=2)
    if(by_bits):
        plt.xlabel('Bit changed')
    else:
        plt.xlabel('Coeff changed')

    plt.legend()
    if(verbose):
        plt.title(title)
        plt.show()
    else:
        plt.title(savename)
    if(HD_plot):
        plt.savefig("img/encodeHD_"+savename, bbox_inches='tight')
    else:
        plt.savefig("img/encodeN2_"+savename, bbox_inches='tight')
    plt.clf()
