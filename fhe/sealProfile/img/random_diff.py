import matplotlib.pyplot  as plt
import numpy as np
import matplotlib
import pandas as pd
# import OS module
import os

# Get the list of all files and directories

plt.rcParams.update({
    "text.usetex": True,
})
matplotlib.rcParams.update({'font.size': 20})
plt.rc('xtick',labelsize=16)
plt.rc('ytick',labelsize=16)

dir = "../logs/log_encode/"

dir_list = os.listdir(dir)
for (root, dirs, file) in os.walk(dir):
    for f in file:
        if '.txt' in f:
            df = pd.read_csv(dir+f, header=None,  skip_blank_lines=False)
            df = df.iloc[1:,:]

            encoding = df[df.columns[0]].to_numpy(dtype='float')
            print(f"{f}: {encoding.mean()}")
dir = "../logs/log_encrypt/"

dir_list = os.listdir(dir)
for (root, dirs, file) in os.walk(dir):
    for f in file:
        if '.txt' in f:
            df = pd.read_csv(dir+f, header=None,  skip_blank_lines=False)
            df = df.iloc[1:,:]

            encoding = df[df.columns[0]].to_numpy(dtype='float')
            print(f"{f}: {encoding.mean()}")
