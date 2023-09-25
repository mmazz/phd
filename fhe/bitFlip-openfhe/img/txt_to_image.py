import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

size = 28

dir = "../data/"
file = "example.txt"
file_bitflip = "example_bitflip.txt"
file_bitflip_decode = "example_bitflip_decode.txt"
file_nonBitflip = "example_nonBitflip.txt"


df = pd.read_csv(dir+file, header=None,  skip_blank_lines=False)
data = df[df.columns[0]].to_numpy(dtype='float')
data_matrix = np.reshape(data[1:], (size, size))
plt.style.use('grayscale')
plt.imshow(data_matrix)
plt.show()

data_matrix = np.loadtxt(dir+file_nonBitflip,  delimiter=",").reshape(28,28)

plt.imshow(data_matrix)
plt.show()

