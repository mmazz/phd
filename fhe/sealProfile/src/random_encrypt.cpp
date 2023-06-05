// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include "examples.h"
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include <memory>

#include <bitset>
#include <chrono>

//bool TESTING = false;
bool TESTING = true;

using namespace seal;
using namespace std;
/*
     +----------------------------------------------------+
        | poly_modulus_degree | max coeff_modulus bit-length |
        +---------------------+------------------------------+
        | 1024                | 27                           |
        | 2048                | 54                           |
        | 4096                | 109                          |
        | 8192                | 218                          |
        | 16384               | 438                          |
        | 32768               | 881                          |
        +---------------------+------------------------------+
*/
float diff_vec(vector<double> v1, vector<double> v2){
    //vector<double> res(v1.size());
    float res = 0;

    if (v1.size()==v2.size()){
        for (int i=0; i<v1.size(); i++){
            res += abs(v1[i]-v2[i]);
        }
        res = res/v1.size();
    }
    else{
        cout << "Vectores de diferente tamaño!!!" << endl;
    }
    return res;
}
//uint8_t* bits_x = (uint8_t*)(&x_plain);
//uint8_t mask = (1 << bit);
//
//uint8_t nuevo = bits_x[byte] & ~(mask);
//uint8_t negbit =~(bits_x[byte] & (mask));
//
//bits_x[byte] = (nuevo & ~(mask)) | (negbit & (mask));


uint64_t bit_flip(uint64_t original, ushort bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    //cout <<"mask 0x" <<std::hex << mask << std::endl;
    return mask^original; // I flip the bit using xor with the mask.
}
int main()
{
    int poly_degree = 2;
    size_t poly_modulus_degree = 4096;
    size_t slot_count = poly_modulus_degree/2;

    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1. / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }


    vector<int> modulus;
    modulus ={ 40, 20, 20, 20};
    double scale = pow(2.0, 39);


    print_example_banner("Example: CKKS Basics");

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;
    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);


    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);

    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);
   // for (int i=0 ; i<poly_modulus_degree+2*poly_modulus_degree; i++){
   //     cout<< dec << i << ") " << x_plain[i] <<  "  " <<"0x" << hex <<x_plain[i]<<endl;
   // }

    int index_value = 0;
    int bit_change = 0;
    uint64_t original_value = 0;
    int N = poly_modulus_degree + 2*poly_modulus_degree;
    int bits = 2;
    cout << "Modulos: " << modulus[0] << " " << modulus[1] << " "<< modulus[2] << " " << modulus[3]<< endl;

    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    Ciphertext x_encrypted;
    encryptor.encrypt(x_plain, x_encrypted);
    if(TESTING){
        Plaintext plain_result;
        vector<double> result;
        // chequear que es algo analogo a encoding...
        for (int index_value=0; index_value<N; index_value++){
            original_value = x_encrypted[index_value];
            bit_change = 0;
            cout << index_value << endl;
            int modulus_index = int(index_value/poly_modulus_degree)+1;

            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            for (int bit_change=0; bit_change<modulus[modulus_index]; bit_change++){
                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                float res = diff_vec(input, result);
                if (res < 100){
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
            }
        }
    }

       return 0;
}
