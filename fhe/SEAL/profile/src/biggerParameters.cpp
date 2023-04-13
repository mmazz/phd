// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include "examples.h"
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

    vector<int> modulus;
    if(poly_modulus_degree==8192){ // max degree 3
        modulus ={ 60, 40, 40, 60 };
    }
    if(poly_modulus_degree==16384){ // max degree 7
        modulus ={ 60, 40, 40, 40, 40, 40, 40, 40,  60 };
    }
    if (poly_modulus_degree==32768){ // max degree 20
        modulus ={ 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 };
    }
    print_example_banner("Example: CKKS Basics");

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));

    double scale = pow(2.0, 40);

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;

    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);
    size_t slot_count = encoder.slot_count();

    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
   // cout << "Input vector: " << endl;
   // print_vector(input, 3, 7);

    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);
    Ciphertext x_encrypted;
    encryptor.encrypt(x_plain, x_encrypted);

    Ciphertext x_pow_encrypted;
    encryptor.encrypt(x_plain, x_pow_encrypted);

    Ciphertext x_res;
    for (int i=2; i<=poly_degree; i++){
        evaluator.multiply(x_encrypted, x_pow_encrypted, x_res);
        evaluator.relinearize_inplace(x_res, relin_keys);
        evaluator.rescale_to_next_inplace(x_res);
        parms_id_type last_parms_id = x_res.parms_id();
        evaluator.mod_switch_to_inplace(x_encrypted, last_parms_id);
        x_pow_encrypted = x_res;
    }


   // vector<double> true_result;
   // for (size_t i = 0; i < input.size(); i++)
   // {
   //     double x = input[i];
   //     true_result.push_back(pow(x,poly_degree));
   // }
   // print_vector(true_result, 3, 7);

    Plaintext plain_result;
    decryptor.decrypt(x_res, plain_result);
    vector<double> result;
    encoder.decode(plain_result, result);
   // print_vector(result, 3, 7);
    return 0;
}
