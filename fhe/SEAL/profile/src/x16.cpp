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



    Plaintext x_plain;
    print_line(__LINE__);
    cout << "Encode input vectors." << endl;
    encoder.encode(input, scale, x_plain);
    Ciphertext x_encrypted;
    encryptor.encrypt(x_plain, x_encrypted);

    Ciphertext x_squared_encrypted;
    print_line(__LINE__);
    cout << "Compute x^2 and relinearize:" << endl;
    evaluator.multiply(x_encrypted, x_encrypted, x_squared_encrypted);
    evaluator.relinearize_inplace(x_squared_encrypted, relin_keys);
    cout << "    + Scale of x^2 before rescale: " << log2(x_squared_encrypted.scale()) << " bits" << endl;

    print_line(__LINE__);
    cout << "Rescale x^2." << endl;
    evaluator.rescale_to_next_inplace(x_squared_encrypted);
    cout << "    + Scale of x^2 after rescale: " << log2(x_squared_encrypted.scale()) << " bits" << endl;


    Ciphertext x_pow4_encrypted;
    evaluator.multiply(x_squared_encrypted, x_squared_encrypted, x_pow4_encrypted);
    evaluator.relinearize_inplace(x_pow4_encrypted, relin_keys);
    cout << "    Scale of x^4 before rescale: " << log2(x_pow4_encrypted.scale()) << " bits" << endl;
    //evaluator.rescale_to_next_inplace(x_pow4_encrypted);
    //cout << "    Scale of x^4 after rescale: " << log2(x_pow4_encrypted.scale()) << " bits" << endl;
    print_line(__LINE__);
    cout << "Rescale x^4." << endl;
    evaluator.rescale_to_next_inplace(x_pow4_encrypted);
    cout << "    + Scale of x^4 after rescale: " << log2(x_pow4_encrypted.scale()) << " bits" << endl;


    Ciphertext x_pow8_encrypted;
    evaluator.multiply(x_pow4_encrypted, x_pow4_encrypted, x_pow8_encrypted);
    evaluator.relinearize_inplace(x_pow8_encrypted, relin_keys);
    cout << "    Scale of x^8 before rescale: " << log2(x_pow8_encrypted.scale()) << " bits" << endl;
    print_line(__LINE__);
    cout << "Rescale x^8." << endl;
    evaluator.rescale_to_next_inplace(x_pow8_encrypted);
    cout << "    + Scale of x^8 after rescale: " << log2(x_pow8_encrypted.scale()) << " bits" << endl;

    Ciphertext x_pow16_encrypted;
    evaluator.multiply(x_pow8_encrypted, x_pow8_encrypted, x_pow16_encrypted);
    evaluator.relinearize_inplace(x_pow16_encrypted, relin_keys);
    cout << "    Scale of x^16 before rescale: " << log2(x_pow16_encrypted.scale()) << " bits" << endl;
    print_line(__LINE__);
    cout << "Rescale x^16." << endl;
    evaluator.rescale_to_next_inplace(x_pow16_encrypted);
    cout << "    + Scale of x^16 after rescale: " << log2(x_pow16_encrypted.scale()) << " bits" << endl;



    Ciphertext x_pow32_encrypted;
    evaluator.multiply(x_pow16_encrypted, x_pow16_encrypted, x_pow32_encrypted);
    evaluator.relinearize_inplace(x_pow32_encrypted, relin_keys);
    cout << "    Scale of x^32 before rescale: " << log2(x_pow32_encrypted.scale()) << " bits" << endl;
    print_line(__LINE__);
    cout << "Rescale x^32." << endl;
    evaluator.rescale_to_next_inplace(x_pow32_encrypted);
    cout << "    + Scale of x^32 after rescale: " << log2(x_pow32_encrypted.scale()) << " bits" << endl;


    Ciphertext x_pow64_encrypted;
    evaluator.multiply(x_pow32_encrypted, x_pow32_encrypted, x_pow64_encrypted);
    evaluator.relinearize_inplace(x_pow64_encrypted, relin_keys);
    cout << "    Scale of x^64 before rescale: " << log2(x_pow64_encrypted.scale()) << " bits" << endl;
    print_line(__LINE__);
    cout << "Rescale x^64." << endl;
    evaluator.rescale_to_next_inplace(x_pow64_encrypted);
    cout << "    + Scale of x^64 after rescale: " << log2(x_pow64_encrypted.scale()) << " bits" << endl;

    cout << endl;
    print_line(__LINE__);
    cout << "Parameters used by all three terms are different." << endl;
    cout << "    + Modulus chain index for x encrypted: "
        << context.get_context_data(x_encrypted.parms_id())->chain_index() << endl;
    cout << "    + Modulus chain index for x^2 encrypted: "
         << context.get_context_data(x_squared_encrypted.parms_id())->chain_index() << endl;
    cout << "    + Modulus chain index for x^4 encrypted: "
         << context.get_context_data(x_pow4_encrypted.parms_id())->chain_index() << endl;
    cout << "    + Modulus chain index for x^8 encrypted: "
         << context.get_context_data(x_pow8_encrypted.parms_id())->chain_index() << endl;
    cout << "    + Modulus chain index for x^16 encrypted: "
         << context.get_context_data(x_pow16_encrypted.parms_id())->chain_index() << endl;
    cout << "    + Modulus chain index for x^32 encrypted: "
         << context.get_context_data(x_pow32_encrypted.parms_id())->chain_index() << endl;
    cout << "    + Modulus chain index for x^64 encrypted: "
         << context.get_context_data(x_pow64_encrypted.parms_id())->chain_index() << endl;
    cout << endl;




    print_line(__LINE__);
    cout << "Normalize scales to 2^40." << endl;
    x_squared_encrypted.scale() = pow(2.0, 40);
    x_encrypted.scale() = pow(2.0, 40);
   // x5_encrypted.scale() = pow(2.0, 40);

    print_line(__LINE__);
    cout << "Normalize encryption parameters to the lowest level." << endl;
    parms_id_type last_parms_id = x_squared_encrypted.parms_id();
    evaluator.mod_switch_to_inplace(x_encrypted, last_parms_id);


    Plaintext plain_result;
    print_line(__LINE__);
    cout << "Decrypt and decode x^4." << endl;
    cout << "    + Expected result:" << endl;
    vector<double> true_result;
    for (size_t i = 0; i < input.size(); i++)
    {
        double x = input[i];
        true_result.push_back( pow(x,64));
    }
    print_vector(true_result, 3, 7);


    decryptor.decrypt(x_pow64_encrypted, plain_result);
    vector<double> result;
    encoder.decode(plain_result, result);
    cout << "    + Computed result ...... Correct." << endl;
    print_vector(result, 3, 7);
    return 0;
}
