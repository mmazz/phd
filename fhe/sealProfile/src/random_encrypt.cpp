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
    double scale = pow(2.0, 40);

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

    auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto &context_data = *context_data_ptr;

    auto &parms2 = context_data.parms();
    auto &coeff_modulus = parms2.coeff_modulus();
    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);
    int x_plain_size = (modulus.size()-1)*poly_modulus_degree;
    int index_value = 0;
    uint64_t original_value = 0;

    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    // Tiene exactamente x_plain_size por 2 de tamaño, como era de esperar
    Ciphertext x_encrypted;
    Ciphertext x_encrypted_original;
    // el .data() creo que es un puntero y entonces me puedo pasar....
    // pero dan igual, si hago .data(1) empiezo desde el segundo cifrado!!
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain, x_encrypted_original);

    cout << x_encrypted_original[4095] << endl;
    cout << x_encrypted_original[4096] << endl;

    std::string file_name = "encryption";
    bool new_file = 1;
    saveDataLog(file_name, 0,0,0, new_file);
    if(TESTING){
        Plaintext plain_result;
        vector<double> result;
        int modulus_index = 0;
        float res = 0;
        int mod_change = 0;
        int mod = 0;
        uint64_t modulus_k = 0;

        cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
        for (int index_value=0; index_value<x_plain_size; index_value++){
            modulus_index = int((index_value+1)/poly_modulus_degree);
            mod = modulus[modulus_index];
            if(mod!= mod_change){
                mod_change = mod;
                cout << "Mod: " << mod << " index " << index_value<< endl;
                modulus_k = coeff_modulus[modulus_index].value();
            }

            cout <<index_value << ", "<< std::flush;

            for (int bit_change=0; bit_change<mod; bit_change++){
                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
                if (x_encrypted[index_value] >= modulus_k){
                    cout<< "Mas grande que el modulo!" << modulus_k << endl;
                    x_encrypted[index_value] = x_encrypted_original[index_value];
                    break;
                }
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
                if (res < 1000){
                    saveDataLog(file_name, index_value, bit_change, res, !new_file);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                x_encrypted[index_value] = x_encrypted_original[index_value];
            }
        }
        saveDataLog(file_name, 1000000000, 10000000, 100, !new_file);
        cout << "c0 done" <<endl;
        modulus_index = 0;
        res = 0;

        // rompio en index = 16398
        for (int index_value=x_plain_size; index_value<2*x_plain_size; index_value++){
            modulus_index = int(((index_value-x_plain_size)+1)/poly_modulus_degree);
            mod = modulus[modulus_index];
            if(mod!= mod_change){
                mod_change = mod;
                modulus_k = coeff_modulus[modulus_index].value();
                cout << "Mod: " << mod << " index " << index_value<< endl;
            }
            cout <<index_value << ", "<< std::flush;
            for (int bit_change=0; bit_change<modulus[modulus_index]; bit_change++){
                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
                if (x_encrypted[index_value] >= modulus_k){
                    cout<< "Mas grande que el modulo!" << modulus_k << endl;
                    x_encrypted[index_value] = x_encrypted_original[index_value];
                    break;
                }
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
                if (res < 1000){
                    saveDataLog(file_name, index_value, bit_change, res, !new_file);
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
