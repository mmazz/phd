// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <seal/plaintext.h>
#include <seal/util/ntt.h>
#include <vector>
#include "examples.h"
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include <memory>

#include <bitset>
#include <chrono>

#include <iterator>
#include <fstream>
#include <algorithm> // for std::copy


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



int main(int argc, char * argv[])
{
    bool TESTING = true;
    double curr_point = 0;
    double max_value = 1.;
    if (argc==1)
        cout << "Starting in default test mode: true" << endl;
    if(argc >= 2){
        if(atoi(argv[1])==1)
            TESTING = true;
        else
            TESTING = false;
        cout << "Starting in test mode: " << std::boolalpha << TESTING << endl;
    }
    if (argc>=3)
        curr_point = atoi(argv[2]);
    if (argc>=4)
        max_value = atoi(argv[3])-curr_point;

    size_t poly_modulus_degree = 4096;
    vector<int> modulus;
    modulus ={ 40, 20, 20, 20};
    double scale = pow(2.0, 39);

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

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
// Hasta aca es igual en todos los ejemplos
//
    auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto &context_data = *context_data_ptr;

    auto &parms2 = context_data.parms();
    auto &coeff_modulus = parms2.coeff_modulus();

    std::size_t coeff_modulus_size = coeff_modulus.size();
    std::size_t coeff_count = parms.poly_modulus_degree();
    auto ntt_tables = context_data.small_ntt_tables();

    Plaintext x_plain_original;
    Plaintext x_plain;

    encoder.encode(input, scale, x_plain_original);
    encoder.encode(input, scale, x_plain);

    cout << "coeff_modulus_size: " <<  coeff_modulus_size <<endl;
    // Mete los valores previos a ser transormados con NTT

    int x_plain_size = (modulus.size()-1)*poly_modulus_degree;

    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    Ciphertext x_encrypted;
    Plaintext plain_result;
    vector<double> result;
    float res = 0;

    if(TESTING){
        int new_file = 1;
        std::string file_name = "encoding_nonNTT";
        saveDataLog(file_name, 0, 0, res, new_file); // simplemente crea el archivo
        new_file = 0;
        // Lupeo todo el vector, Son K polynomios con K = len(modulus)-1.
        // Cada uno tiene N = poly_modulus_degree coefficientes.
        // Estan pegados uno al lado del otro, entonces es un vector de N*K elementos.
        for (int index_value=0; index_value<x_plain_size; index_value++){
            cout << index_value << endl;
            // el modulo cambia por polynomio:
            int modulus_index = int((index_value+1)/poly_modulus_degree);
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            uint64_t modulus_k = coeff_modulus[modulus_index].value();
            for (int bit_change=0; bit_change<modulus[modulus_index]; bit_change++){
                reset_values(x_plain);
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                ntt_transformation(x_plain, coeff_modulus_size, coeff_count, ntt_tables, modulus_index);
                if (x_plain[index_value] >= modulus_k){
                    cout<< "Mas grande que el modulo!" << modulus_k << endl;
                }
                encryptor.encrypt(x_plain, x_encrypted);
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
             //   cout << res << endl;
                if (res < 10000){
                    saveDataLog(file_name, index_value, bit_change, res, new_file);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
            }
        }
        cout << x_encrypted[x_plain_size*2-1] << endl;
    }
    else{
        encryptor.encrypt(x_plain, x_encrypted);
        decryptor.decrypt(x_encrypted, plain_result);
        encoder.decode(plain_result, result);
        res = diff_vec(input, result);
        if (res<1)
            cout << "Good decryption" << endl;
        // Meto los valores previos a ntt
        reset_values(x_plain);
        // Confirmado! Mapea bien!
        ntt_transformation(x_plain, coeff_modulus_size, coeff_count, ntt_tables);
        int result = check_equality(x_plain_original, x_plain, x_plain_size);

        if(result!=0)
            cout << "Error en la transformacion ntt"<< endl;
        else
            cout << "Correcta transformacion ntt"<< endl;
        reset_values(x_plain);
    }
       return 0;
}
