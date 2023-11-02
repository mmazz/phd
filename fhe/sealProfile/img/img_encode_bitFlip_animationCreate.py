import matplotlib.pyplot as plt
import numpy as np

size = 28

dir = "../data/animation/"
dir_fig = "../data/animation_imgs/"
bits = 60
polydegree =4096
bits = 1
polydegree =1
offset = 500
amount_imgs = 3
count = 0
for i in range(amount_imgs):
    file = f"img_encode_bitFlip_nonOps_decode_{i+offset}.txt"
    bit = 0
    with open(dir+file, 'r') as file:
      for line in file:
        print(count)
        data = np.fromstring(line, dtype=float, sep=',')
        coeff = data[0]
        hd = round(data[0],2)
        norm2 = round(data[1],2)
        data_matrix = data[2:].reshape(28,28)
        plt.imshow(data_matrix)
        plt.title(f"Coeff={coeff}, bit chaged={bit}, HD={hd}, norm2={norm2}")
        plt.savefig(dir_fig+"animation"+str(count)+".png", bbox_inches ="tight", format="png", dpi=700)
        plt.cla()
        bit += 1
        count += 1
