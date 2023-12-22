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

def data_read(dir, file, HD, total_bits_hdCompare):
    df = pd.read_csv(dir+file, header=None,  skip_blank_lines=False)
    df = df.iloc[1:,:]
    input = df[df.columns[0]].to_numpy(dtype='float')
    if (HD):
        input = input/total_bits_hdCompare*100
    print(f"{file}: {input.mean()}")
    return input

# Hago una matriz de cantidad de coeficientes por cantidad de bits por coeff
def data_reshape(data, num_rows, num_cols, bounded, max_diff_tot):
    if (bounded):
        data = data*100/max_diff_tot
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
    return by_coeff_av, by_bits_av, bycoeff_max, bycoeff_min, bybits_max, bybits_min



