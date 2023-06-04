// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include "examples.h"
#include <sys/resource.h>


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

// First argument is the multiplication depth
// Second argument is the N.
int main(int argc, char *argv[])
{
    int poly_degree = 4;
    int poly_modulus_degree = 16384;
    int size_vector = 500;
    ////////////////////////////////    struct rusage usage;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < size_vector; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
    //////////////////////


    vector<int> modulus;
    double scale = pow(2.0, 40);
    modulus = {60,40,40,40,40,60};



    struct rusage usage;
    long int start, end;
    getrusage(RUSAGE_SELF, &usage);

    start = usage.ru_maxrss;
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
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);

   // print_vector(true_result, 3, 7);
   // cout << "Input vector: " << endl;

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

    Plaintext plain_result;
    decryptor.decrypt(x_res, plain_result);
    vector<double> result;
    encoder.decode(plain_result, result);
    getrusage(RUSAGE_SELF, &usage);
    end = usage.ru_maxrss;

    cout <<  end-start <<endl;
    return 0;
}
