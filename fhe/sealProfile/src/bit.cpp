// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include "examples.h"

#include <bitset>
#include <chrono>
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
int main(int argc, char *argv[])
{
    int poly_degree = 4;

    if (argc>1){
        int num = atoi(argv[1]);
        if (num < 2){
            cout << "The polynomial results must have a degree greater than 2" << endl;
        }
        else
            poly_degree = num;
    }

    size_t poly_modulus_degree = 32768;
    if (argc>2){
        poly_modulus_degree = atoi(argv[2]);
        if (poly_modulus_degree != 32768 or poly_modulus_degree != 16384){
            cout << "The polynomial modulus must be" << endl;
        }

    }

    ////////////////////////////////    struct rusage usage;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }

    //////////////////////


    vector<int> modulus;
    double scale = pow(2.0, 40);
    if(poly_modulus_degree==4096){ // max degree 3
        modulus ={ 40, 20, 20, 20};
        scale = pow(2.0, 10);
    }
    if(poly_modulus_degree==8192){ // max degree 3
        modulus = {60};
        for (int i=0; i<poly_degree; i++){
            modulus.push_back(40);
        }
        modulus.push_back(60);
    }
    if(poly_modulus_degree==16384){ // max degree 7
        modulus = {60};
        for (int i=0; i<poly_degree; i++){
            modulus.push_back(40);
        }
        modulus.push_back(60);
    }
    if (poly_modulus_degree==32768){ // max degree 20
        modulus = {60};
        for (int i=0; i<poly_degree; i++){
            modulus.push_back(40);
        }
        modulus.push_back(60);
        //modulus ={ 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 };
    }

    print_example_banner("Example: CKKS Basics");


    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));


    SEALContext context(parms);
    print_parameters(context);
    cout << endl;
    cout << "Generating keys: ";
    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);

   // print_vector(true_result, 3, 7);
   // cout << "Input vector: " << endl;
    cout << "Encode+Encrypt: ";
    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);
//////////////////////////////////////////////
///
///       ACA seria. El x_plain.
///
/////////////////////////////////////////////

//    uint8_t* bits_x = (uint8_t*)(&x_plain);
//    int bit = 0;
//    int byte = 0;
//    uint8_t mask = (1 << bit);
//
//    uint8_t nuevo = bits_x[byte] & ~(mask);
//    uint8_t negbit =~(bits_x[byte] & (mask));

//    bits_x[byte] = (nuevo & ~(mask)) | (negbit & (mask));

    cout << "\n Tamaño en memoria del encoding: " << x_plain.capacity() << endl;
    Ciphertext x_encrypted;
    encryptor.encrypt(x_plain, x_encrypted);


    cout << "Dencode+decrypt: ";
    Plaintext plain_result;
    decryptor.decrypt(x_encrypted, plain_result);

    vector<double> result;
    encoder.decode(plain_result, result);
    print_vector(input, 3, 7);
    print_vector(result, 3, 7);

    return 0;
}
