// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include "examples.h"
using namespace seal;
using namespace std;


int main()
{
    print_example_banner("Example: CKKS Basics");

    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 60, 40, 40, 40, 40, 40, 40, 40,  40, 40, 60 }));

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
    cout << "Number of slots: " << slot_count << endl;

    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
    cout << "Input vector: " << endl;
    print_vector(input, 3, 7);

    cout << "Evaluating polynomial x^4 " << endl;

    int loops = 4;

    Plaintext x_plain;
    print_line(__LINE__);
    cout << "Encode input vectors." << endl;
    encoder.encode(input, scale, x_plain);
    Ciphertext x_encrypted;
    encryptor.encrypt(x_plain, x_encrypted);

    Ciphertext x_pow_encrypted;
    encryptor.encrypt(x_plain, x_pow_encrypted);

    Ciphertext x_res;
    print_line(__LINE__);
    for (int i=0; i<=loops; i++){
        cout << "Compute x^" << i+2 << " and relinearize:" << endl;
        evaluator.multiply(x_encrypted, x_pow_encrypted, x_res);
        evaluator.relinearize_inplace(x_res, relin_keys);
        cout << "    + Scale of x^" << i+2 <<" before rescale: " << log2(x_res.scale()) << " bits" << endl;
        evaluator.rescale_to_next_inplace(x_res);
        parms_id_type last_parms_id = x_res.parms_id();
        evaluator.mod_switch_to_inplace(x_encrypted, last_parms_id);
        x_pow_encrypted = x_res;
    }

    cout << endl;





    print_line(__LINE__);
    cout << "Normalize scales to 2^40." << endl;
    x_res.scale() = pow(2.0, 40);
    x_encrypted.scale() = pow(2.0, 40);
   // x5_encrypted.scale() = pow(2.0, 40);

    print_line(__LINE__);
    cout << "Normalize encryption parameters to the lowest level." << endl;
    parms_id_type last_parms_id = x_res.parms_id();
    evaluator.mod_switch_to_inplace(x_encrypted, last_parms_id);


    Plaintext plain_result;
    print_line(__LINE__);
    cout << "Decrypt and decode x^" << loops+2 << endl;
    cout << "    + Expected result:" << endl;
    vector<double> true_result;
    for (size_t i = 0; i < input.size(); i++)
    {
        double x = input[i];
        true_result.push_back(pow(x,2+loops));
    }
    print_vector(true_result, 3, 7);


    decryptor.decrypt(x_res, plain_result);
    vector<double> result;
    encoder.decode(plain_result, result);
    cout << "    + Computed result ...... Correct." << endl;
    print_vector(result, 3, 7);
    return 0;
}
