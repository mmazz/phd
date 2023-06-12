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

using namespace seal;
using namespace std;
int MAX_DIFF = 1000;

int main(int argc, char * argv[])
{
    bool TESTING = true;
    bool RNS = true;
    double curr_point = 0;
    double max_value = 1.;
    if (argc==1)
        cout << "Starting in default test mode: true" << endl;
    if(argc >= 2)
    {
        if(atoi(argv[1])==1)
            TESTING = true;
        else
            TESTING = false;
        cout << "Starting in test mode: " << std::boolalpha << TESTING << endl;
    }
    if (argc>=3)
    {
        if(atoi(argv[2])==1)
            RNS = true;
        else
            RNS = false;
        cout << "Starting in RNS mode: " << std::boolalpha << RNS << endl;
    }
    if (argc>=4)
        curr_point = atoi(argv[3]);
    if (argc>=5)
        max_value = atoi(argv[4])-curr_point;

    size_t poly_modulus_degree = 4096;
    vector<int> modulus;
    if(RNS)
        //modulus ={ 40, 20, 20, 20};
        modulus ={ 30, 30, 20};
    else
        modulus ={ 60, 20};
    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

    print_example_banner("Example: CKKS random encryption without ntt");
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
    int x_plain_size = (modulus.size() - 1) * poly_modulus_degree;
    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    // Tiene exactamente x_plain_size por 2 de tamaño, como era de esperar
    Ciphertext x_encrypted;
    Ciphertext x_encrypted_original;
    // el .data() creo que es un puntero y entonces me puedo pasar....
    // pero dan igual, si hago .data(1) empiezo desde el segundo cifrado!!
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain, x_encrypted_original);
    auto coeff_modulus = parms.coeff_modulus();

    Plaintext plain_result;
    vector<double> result;
    float res = 0;
    if(TESTING)
    {
        bool new_file = 1;
        std::string file_name_c0;
        std::string file_name_c1;
        if (RNS){
            file_name_c0 = "encryption_c0_withRNS";
            file_name_c1 = "encryption_c1_withRNS";
        }
        else
        {
            file_name_c0 = "encryption_c0_nonRNS";
            file_name_c1 = "encryption_c1_nonRNS";
        }
        saveDataLog(file_name_c0, 0, 0, res, new_file);
        saveDataLog(file_name_c1, 0, 0, res, new_file);
        int modulus_index = 0;
        int modulus_bits = 0;
        uint64_t k_rns_prime = 0;

        cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
        for (int index_value=0; index_value<x_plain_size; index_value++)
        {
            if (index_value>=x_plain_size)
                modulus_index = int((index_value-x_plain_size)/poly_modulus_degree);
            else
                modulus_index = int(index_value/poly_modulus_degree);

            modulus_bits = modulus[modulus_index];
            k_rns_prime = coeff_modulus[modulus_index].value();
            cout <<index_value << ", "<< std::flush;
            for (int bit_change=0; bit_change<modulus_bits; bit_change++)
            {
                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
                if (x_encrypted[index_value] >= k_rns_prime){
                    cout<< "Mas grande que el modulo!" << k_rns_prime << endl;
                    x_encrypted[index_value] = x_encrypted_original[index_value];
                    break;
                }
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
                if (res < MAX_DIFF){
                    if (index_value<x_plain_size)
                        saveDataLog(file_name_c0, index_value, bit_change, res, !new_file);
                    else
                        saveDataLog(file_name_c1, index_value, bit_change, res, !new_file);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                x_encrypted[index_value] = x_encrypted_original[index_value];
            }
        }

    }

       return 0;
       //    cout <<  "validity ntt form: " << x_encrypted.is_ntt_form() << ", valid for:  ";
       //    cout <<is_valid_for(x_encrypted, context)<< ", buffer valid: " << is_buffer_valid(x_encrypted);
       //    cout << ",  data valid for: " <<  is_data_valid_for(x_encrypted, context)<< ", metadata valid for:  ";
       //    cout << is_metadata_valid_for(x_encrypted, context)<<endl;
}
