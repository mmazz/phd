// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include "examples.h"
#include <ios>
#include <iostream>
#include <memory>
#include <seal/ciphertext.h>
#include <seal/ckks.h>
#include <seal/context.h>
#include <seal/decryptor.h>
#include <seal/keygenerator.h>
#include <seal/randomgen.h>
#include <vector>

#include <bitset>
#include <chrono>

using namespace seal;
using namespace std;
int MAX_DIFF = 1;

int main(int argc, char * argv[])
{
    int num_stats = 0;
    double curr_point = 0;
    double max_value = 1.;
    if (argc==1)
        cout << "Starting in default test mode: true" << endl;
    if(argc >= 2)
    {
        num_stats = atoi(argv[1]);
        cout << "Starting with " << num_stats << " numbers of stats" << endl;
    }

    size_t poly_modulus_degree = 4096;
    vector<int> modulus;

    modulus ={ 60, 20};
    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

    print_example_banner("Example: CKKS random encryption with ntt");
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
    //auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto context_data_ptr = context.get_context_data(parms.parms_id());
    auto &context_data = *context_data_ptr;
    auto &coeff_modulus = parms.coeff_modulus();
    auto ntt_tables = context_data.small_ntt_tables();

    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);

    int x_plain_size = (modulus.size() - 1) * poly_modulus_degree;

    // Este es el experimento que quiero, pero antes quiero chequear que los
    // elementos que estoy tocando del encoding sean los correctos. Tiene
    // exactamente x_plain_size por 2 de tamaño, como era de esperar
    Ciphertext x_encrypted;
    Ciphertext x_encrypted_original;
    // el .data() creo que es un puntero y entonces me puedo pasar....
    // pero dan igual, si hago .data(1) empiezo desde el segundo cifrado!!
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain, x_encrypted_original);

    Plaintext plain_result;
    vector<double> result;
    bool new_file = 1;
    std::string file_name_c;

    file_name_c = "encryption_c_nonNTT_nonRNS_";

    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;

for (int i =0; i<num_stats; i++)
{
    float res = 0;
    encoder.encode(input, scale, x_plain);
    cout << "encode" << x_plain[0] << endl;
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain, x_encrypted_original);
    saveDataLog(file_name_c+std::to_string(i), res, new_file);
    int modulus_index = 0;
    int modulus_bits = 0;
    uint64_t k_rns_prime = 0;

    cout << "Starting stat: " << i << endl;
    cout <<i << ", "<< std::flush;
    for (int index_value=0; index_value<2*x_plain_size; index_value+=550)
    //for (int index_value=0; index_value<2*x_plain_size; index_value++)
    {
        if (index_value>=x_plain_size)
            modulus_index = int((index_value-x_plain_size)/poly_modulus_degree);
        else
            modulus_index = int(index_value/poly_modulus_degree);

        modulus_bits = modulus[modulus_index];
        k_rns_prime = coeff_modulus[modulus_index].value();
        //for (int bit_change=0; bit_change<modulus_bits; bit_change++)
        for (int bit_change=0; bit_change<modulus_bits; bit_change+=10)
        {
            if (index_value<x_plain_size)
                util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
            else
                util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
            x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);

            if (index_value<x_plain_size)
                ntt_transformation(x_encrypted, ntt_tables, modulus_index, 0);
            else
                ntt_transformation(x_encrypted, ntt_tables, modulus_index, 1);
            if (x_encrypted[index_value] >=  k_rns_prime){
                cout<< "Mas grande que el modulo!" << k_rns_prime << endl;
                x_encrypted = x_encrypted_original;
                break;
            }
            decryptor.decrypt(x_encrypted, plain_result);
            encoder.decode(plain_result, result);
            res = diff_vec(input, result);
            if (res < MAX_DIFF)
            {
                saveDataLog(file_name_c, 1, !new_file);
            }
            else
            {
                saveDataLog(file_name_c, 0, !new_file);
            }
            x_encrypted = x_encrypted_original;

        }
    }
    curr_point += 1.5;
    max_value += 3;
    input_refill(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input,3,7);
}
  return 0;
}
