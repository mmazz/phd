import sys
import time
import math
import numpy as np
from seal import *
from seal_helper import *


def encrypt(n=16):
    parms = EncryptionParameters(scheme_type.ckks)
    poly_modulus_degree = 16384
    parms.set_poly_modulus_degree(poly_modulus_degree)
    parms.set_coeff_modulus(CoeffModulus.Create(
        poly_modulus_degree, [60, 40, 40, 40, 40, 60]))
    scale = 2.0**40
    context = SEALContext(parms)
    print_parameters(context)

    ckks_encoder = CKKSEncoder(context)
    slot_count = ckks_encoder.slot_count()
    print(f'Number of slots: {slot_count}')

    keygen = KeyGenerator(context)
    public_key = keygen.create_public_key()
    secret_key = keygen.secret_key()

    encryptor = Encryptor(context, public_key)
    decryptor = Decryptor(context, secret_key)

    x = np.arange(1, n)

    plain_x= ckks_encoder.encode(x, scale)
    cipher_x = encryptor.encrypt(plain_x)
    return x, cipher_x, decryptor, ckks_encoder

def bitchange(x):
    return x

def decrypt(cipher_x, decryptor, ckks_encoder):
    plain = decryptor.decrypt(cipher_x)
    res = ckks_encoder.decode(plain)
    return res


if __name__ == "__main__":
    args = sys.argv[1:]
    n = int(args[0]) if args else 4
    print(f'n: {n}')
    x, cipher_x, decryptor, ckks_encoder = encrypt()
    x_bit = bitchange(cipher_x)
    res = decrypt(x_bit, decryptor, ckks_encoder)
    for i in range(len(x)):
        print(x[i], " vs ", round(res[i]))

