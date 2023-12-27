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
    df = pd.read_csv(dir+file, header=None, skip_blank_lines=False, dtype='float64')
    df = df[:-1]
    input = df[df.columns[0]].to_numpy(dtype='int')
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

