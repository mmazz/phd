import numpy as np
import math

# Agrupo por zonas...
row_chunk = 32
column_chunk = 20
#numero de columnas
bits_encoding = int(163840/column_chunk)+1

# numero de filas
bits_to_change = int(64*1000/row_chunk)+1

print(f"Hago una matriz de {bits_to_change} x {bits_encoding}")

# promedio todas las filas (promedio vertical)
avg_rows = np.zeros(64*1000, dtype=int)
# promedio todas las columnas (promedio horizontal)
avg_cols = np.zeros(163840, dtype=int)
matrix = np.zeros((bits_to_change, bits_encoding),  dtype=int)

file = open("new_log_encode/encoding.txt", "r")

num_row = 200
row_count = 0
row = 0
column_count = 0
temp_array = np.zeros(bits_encoding)

# tengo que tener los datos ordenados en cada fila
# para agrupar dentro de distintas columnas divido por el tamaño que quiero y redondeo para arriba
# Voy contando las filas y cuando supere una nueva sumo uno a la row.
for line in file.readlines()[1:]:
    col = 0
    for word in line.split():
        temp_array[math.floor(int(word)/column_chunk)] += 1
        avg_rows[row] += int(word)
        avg_cols[col] += int(word)
        col+=1
    avg_cols[row] = avg_cols[row]/col
    if(math.floor(row_count/row_chunk)/(row+1) > 1):
        matrix[row] = temp_array/(row_chunk*column_chunk)*100
        temp_array = np.zeros(bits_encoding)
        row += 1
    row_count+=1
avg_rows = avg_rows/num_row
file.close()
np.savetxt('heatmap_encoding.txt', matrix, delimiter=',')
np.savetxt('heatmap_encoding_rowAVG.txt', avg_rows, delimiter=',')
np.savetxt('heatmap_encoding_colAVG.txt', avg_cols, delimiter=',')
