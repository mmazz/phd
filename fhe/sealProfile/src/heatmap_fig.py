import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns


data = np.loadtxt('heatmap_encoding.txt', delimiter=',')

data = data[:250]
plt.imshow( data, cmap = 'jet' )
cbar = plt.colorbar(ticks=[0, 0.5, 1])
cbar.ax.set_yticklabels(['Low', 'Medium', 'High'])
plt.title( "2-D Heat Map" )

# plotting the heatmap
#hm = sns.heatmap(data=data, annot=True)

plt.savefig("heatmap.pdf", bbox_inches='tight')
# displaying the plotted heatmap
plt.show()
