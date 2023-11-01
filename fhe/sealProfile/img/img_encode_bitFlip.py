import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

size = 28

dir = "../data/"
file = "example.txt"
file_notChange = "img_encode_nonBitflip.txt"
file_bitflip = "img_encode_bitFlip_nonOps.txt"
file_bitflip_decode = "img_encode_bitFlip_nonOps_decode.txt"


df = pd.read_csv(dir+file, header=None,  skip_blank_lines=False)
data = df[df.columns[0]].to_numpy(dtype='float')
data_matrix = np.reshape(data[1:], (size, size))
plt.style.use('grayscale')
plt.imshow(data_matrix)
plt.savefig("original.png")
plt.show()

#data_matrix = np.loadtxt(dir+file_notChange,  delimiter=",").reshape(28,28)*10000

#plt.imshow(data_matrix)
#plt.title("Decripted with no change")
#plt.show()

#data_matrix = np.loadtxt(dir+file_bitflip_decode,  delimiter=",", dtype=int).reshape(28,28)*10000

#plt.imshow(data_matrix)
#plt.title("Only decode with bitflip")
#plt.show()

#df = pd.read_csv(dir+file_bitflip, delimiter=',')
data_matrix = np.loadtxt(dir+file_bitflip, delimiter=",").reshape(28,28)
#data = df.to_numpy(dtype='float')
#print(data)
#data_matrix = np.reshape(data[1:], (size, size))
#plt.style.use('grayscale')
plt.imshow(data_matrix)
plt.title("Decrypted with bitflip")
plt.savefig("bitflip.png", bbox_inches ="tight", format="png", dpi=30)
plt.show()
