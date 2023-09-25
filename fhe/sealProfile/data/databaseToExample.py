import pandas as pd
import numpy as np
df = pd.read_csv("mnist.csv")

row = df.iloc[1].to_numpy()
np.savetxt("example.txt",row)
print(row)
