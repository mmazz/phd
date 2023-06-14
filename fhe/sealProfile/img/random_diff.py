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

dir = "../log_encode_nonNTT/"

df = pd.read_csv(dir+"encoding_elemDiff_nonNTT_withRNS"+'.txt', header=None,  skip_blank_lines=False)

df = df.iloc[1:,:]

encoding = df[df.columns[0]].to_numpy(dtype='float')
print(encoding.sum())

