import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from PIL import Image

size = 28

dir = "../data/"
file = "example.txt"
file_bitflip = "example_bitflip.txt"


df = pd.read_csv(dir+file, header=None,  skip_blank_lines=False)
data = df[df.columns[0]].to_numpy(dtype='float')
data_matrix = np.reshape(data[1:], (size, size))
plt.style.use('grayscale')
plt.imshow(data_matrix)
plt.show()

#df = pd.read_csv(dir+file_bitflip, delimiter=',')
data_matrix = np.loadtxt(dir+file_bitflip, dtype=int, delimiter=",").reshape(28,28)
print(data_matrix)
#data = df.to_numpy(dtype='float')
#print(data)
#data_matrix = np.reshape(data[1:], (size, size))
#plt.style.use('grayscale')
plt.imshow(data_matrix)
plt.show()
