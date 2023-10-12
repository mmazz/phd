- encode_bitflip_nonOps.cpp: Cambia de a un bit el encoding de CKKS sin RNS ni NTT y calcula distancia hamming y la norma 2
entre el resutaldo llegado a desencriptar y el input.
- ntt_bitFlip.cpp: Agarra un input, lo codifica, lo saca del formato NTT, le cambia un bit, y luego vuelve al espacio de NTT y
le calcula la distancia hamming y la norma 2 a ese input en NTT modificado y su original.
- example.h y utils_mati.h: Son simplemente funciones que cree para usar en los diferentes scripts
