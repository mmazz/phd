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
int MAX_DIFF = 1;
float threshold = 0.1;
double CURR_POINT = 1;
double MAX_VALUE = 2.;
int size_input= 2048;
int main(int argc, char * argv[])
{
    bool TESTING = true;
    bool RNS = true;
    double curr_point = CURR_POINT;
    double max_value = MAX_VALUE;
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
        modulus ={ 50, 20};
    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

    print_example_banner("Example: CKKS random encoding with ntt");
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
    auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto &context_data = *context_data_ptr;
    auto &coeff_modulus = parms.coeff_modulus();
    auto ntt_tables = context_data.small_ntt_tables();

    Plaintext x_plain_original;
    Plaintext x_plain;

    encoder.encode(input, scale, x_plain_original);
    encoder.encode(input, scale, x_plain);

    // Mete los valores previos a ser transormados con NTT

    int x_plain_size = (modulus.size() - 1) * poly_modulus_degree;

    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    Ciphertext x_encrypted;
    Plaintext plain_result;
    vector<double> result;
    float res = 0;
    float res_elem = 0;
    if(TESTING){
        bool new_file = 1;
        std::string dir_name = "log_encode_nonNTT/";
        std::string file_name;
        std::string file_name_elem;
        if (RNS){
            file_name = "encoding_nonNTT_withRNS";
            file_name_elem = "encoding_elemDiff_nonNTT_withRNS";
        }
        else{
            file_name = "encoding_nonNTT_nonRNS";
            file_name_elem = "encoding_elemDiff_nonNTT_nonRNS";
        }
    //    saveDataLog(dir_name+file_name, res, new_file); // simplemente crea el archivo
        saveDataLog(dir_name+file_name_elem, 0, 0, res_elem, new_file);
        //saveDataLog(file_name, 0, 0, res, new_file); // simplemente crea el archivo
        int modulus_index = 0;
        int modulus_bits = 0;
        uint64_t k_rns_prime = 0;
        cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
        // Lupeo todo el vector, Son K polynomios con K = len(modulus)-1.
        // Cada uno tiene N = poly_modulus_degree coefficientes.
        // Estan pegados uno al lado del otro, entonces es un vector de N*K elementos.
        for (int index_value=0; index_value<x_plain_size; index_value++){
            // el modulo cambia por polynomio:
            modulus_index = int(index_value/poly_modulus_degree);
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            modulus_bits = modulus[modulus_index];
            k_rns_prime = coeff_modulus[modulus_index].value();
            cout <<index_value << ", "<< std::flush;
            for (int bit_change=0; bit_change<modulus[modulus_index]; bit_change++)
            {
                util::inverse_ntt_negacyclic_harvey(x_plain.data(0) + (modulus_index * poly_modulus_degree),
                                                    ntt_tables[modulus_index]);
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                ntt_transformation(x_plain, ntt_tables, modulus_index, poly_modulus_degree);
                if (x_plain[index_value] >= k_rns_prime){
                    cout<< "Mas grande que el modulo!" << k_rns_prime << endl;
                    x_plain = x_plain_original;
                //    saveDataLog(dir_name+file_name, 0, !new_file);
                    saveDataLog(dir_name+file_name_elem, 0, !new_file);
                }
                else
                {
                    encryptor.encrypt(x_plain, x_encrypted);
                    decryptor.decrypt(x_encrypted, plain_result);
                    encoder.decode(plain_result, result);
                 //   res = diff_vec(input, result, MAX_DIFF);
                 //   saveDataLog(dir_name+file_name, res, !new_file);

                    res_elem = diff_elem(input, result, threshold, size_input);
                    saveDataLog(dir_name+file_name_elem, res_elem, !new_file);

                    x_plain = x_plain_original;
                }
            }
        }
    }
    else{
        encryptor.encrypt(x_plain, x_encrypted);
        decryptor.decrypt(x_encrypted, plain_result);
        encoder.decode(plain_result, result);
        res = diff_vec(input, result,MAX_DIFF);
        if (res<1)
            cout << "Good decryption" << endl;
        bool ntt_res = true;
        bool ntt_temp = false;
        x_plain = x_plain_original;
        for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
        {
            util::inverse_ntt_negacyclic_harvey(x_plain.data(0) + (modulus_index * poly_modulus_degree),
                                                ntt_tables[modulus_index]);
            ntt_transformation(x_plain, ntt_tables, modulus_index, poly_modulus_degree);
            ntt_temp = check_equality(x_plain, x_plain_original);
            ntt_res = ntt_res&ntt_temp;
        }
        cout << "plain text inverse ntt and ntt: " << std::boolalpha << ntt_res <<endl;
        x_plain = x_plain_original;
        int modulus_index = 0;
        for (int index_value=0; index_value<x_plain_size; index_value+=200)
        {
            modulus_index = int((index_value+1)/poly_modulus_degree);
            for (int bit_change=0; bit_change<modulus[modulus_index]; bit_change+=8)
            {
                util::inverse_ntt_negacyclic_harvey(x_plain.data(0) + (modulus_index * poly_modulus_degree),
                        ntt_tables[modulus_index]);
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                ntt_transformation(x_plain, ntt_tables, modulus_index, poly_modulus_degree);
                ntt_temp = check_equality(x_plain, x_plain_original);
                ntt_res = ntt_res&ntt_temp;
            }
        }
        cout << "plain text inverse ntt and ntt with double bitflitp : " << std::boolalpha << ntt_res <<endl;
        }

       return 0;
}
