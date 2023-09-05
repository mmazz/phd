import matplotlib.pyplot as plt
import numpy as np
data = np.loadtxt('heatmap_encoding.txt', delimiter=',')

plt.imshow( data )

plt.title( "2-D Heat Map" )
plt.show()
