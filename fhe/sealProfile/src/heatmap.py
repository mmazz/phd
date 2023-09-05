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

matrix = np.zeros((bits_to_change, bits_encoding),  dtype=int)

file = open("new_log_encode/encoding.txt", "r")

row_count = 0
row = 0
column_count = 0
temp_array = np.zeros(bits_encoding)

# tengo que tener los datos ordenados en cada fila
# para agrupar dentro de distintas columnas divido por el tamaño que quiero y redondeo para arriba
# Voy contando las filas y cuando supere una nueva sumo uno a la row.
for line in file.readlines()[1:]:
    print(row_count)
    for word in line.split():
        temp_array[math.floor(int(word)/column_chunk)] += 1
    if(math.floor(row_count/row_chunk)/(row+1) > 1):
        matrix[row] = temp_array
        temp_array = np.zeros(bits_encoding)
        row += 1
    row_count+=1

file.close()
np.savetxt('heatmap_encoding.txt', matrix, delimiter=',')
