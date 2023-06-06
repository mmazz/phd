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

void saveData(int index_value, int bit_change, float res)
{
    std::fstream logFile;
    // Open File
    logFile.open("/home/mmazz/phd/fhe/sealProfile/log_encode.txt", std::ios::app);
    //Write data into log file
    logFile << "Diff: :" << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
    // close file stream
    logFile.close();

}



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
    uint64_t res = 0;
    res =  mask^original; // I flip the bit using xor with the mask.
    return res;
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
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_plain_original);
    int x_plain_size = poly_modulus_degree + 2*poly_modulus_degree;
    int bits = 2;

    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    if(TESTING){
        // se esta rompiendo con algunos cambios de bits altos en algunos coeffs.
        // ejemplo coeff 4100 al cambiar el bit 19.
        // parece que se rompe el is_data_valid_for(x_plain, context)
        // como esta en forma de ntt, en valcheck.cpp linea 263, chequea ahi.
        Ciphertext x_encrypted;
        Plaintext plain_result;
        vector<double> result;
        // Lupeo todo el vector, Son K polynomios con K = len(modulus)-1.
        // Cada uno tiene N = poly_modulus_degree coefficientes.
        // Estan pegados uno al lado del otro, entonces es un vector de N*K elementos.
        for (int index_value=0; index_value<x_plain_size; index_value++){
            cout << index_value << endl;
            int modulus_index = int(index_value/poly_modulus_degree)+1;
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            for (short bit_change=0; bit_change<modulus[modulus_index]-2; bit_change++){
                //cout << bit_change << endl;
               // cout << x_plain[index_value]<< endl;
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
              //  cout << x_plain[index_value] << endl;
               // cout <<  "validity " <<x_plain.is_ntt_form() << " " <<is_valid_for(x_plain, context)<< " " << is_buffer_valid(x_plain)<< " " <<  is_data_valid_for(x_plain, context)<< " " << is_metadata_valid_for(x_plain, context)<<endl;
                encryptor.encrypt(x_plain, x_encrypted);
                decryptor.decrypt(x_encrypted, plain_result);
              //  cout << "decode?" << endl;
                encoder.decode(plain_result, result);
                float res = diff_vec(input, result);
                if (res < 10000){
                    saveData(index_value, bit_change, res);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                x_plain[index_value]= x_plain_original[index_value] ;
            }
        }
    }


    // Estaria teniendo diferentes valores con el log_ckks.txt que es un log que mete
    // el dato dentro del codigo en un log.
    // Ya esta, estaba mirando los datos antes de aplicarle NTT. Dan lo mismo.
    // Queda la duda si la sensibilidad viene de parte de NTT y por eso cambiando un
    // unico bit da todo mal.
    if(!TESTING){
        int index_value = 0;
        for(int i=0; i<poly_modulus_degree; i++){
            for(int j=0; j<modulus.size()-1; j++){
                index_value = i + (j*poly_modulus_degree);
                cout << index_value << ": " << x_plain[index_value] << endl;
                //cout << index_value << ": " << x_plain.data()[index_value] <<endl;
            }
        }
    }

       return 0;
}
