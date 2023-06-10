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
int MAX_DIFF = 1000;

void Bitflip_ci(seal::Ciphertext &x_encrypted,
                seal::Ciphertext &x_encrypted_original, int ci,
                seal::SEALContext &context, seal::Decryptor &decryptor,
                vector<double> &input, seal::CKKSEncoder &encoder,
                std::string file_name, int poly_modulus_degree,
                vector<int> modulus) {
    int x_plain_size = (modulus.size() - 1) * poly_modulus_degree;
    bool new_file = 1;
    int modulus_value = 0;
    int modulus_value_last = 0;
    int modulus_index = 0;
    int modulus_k = 0;
    float res = 0;
    Plaintext plain_result;
    vector<double> result;

    auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto &context_data = *context_data_ptr;
    auto &parms2 = context_data.parms();
    auto &coeff_modulus = parms2.coeff_modulus();
    std::size_t coeff_modulus_size = coeff_modulus.size();
    std::size_t coeff_count = parms2.poly_modulus_degree();
    auto ntt_tables = context_data.small_ntt_tables();

    saveDataLog(file_name, 0, 0, 0, new_file);
    int starting_index = 0;
    int final_index = x_plain_size;
    if (ci == 1)
    {
        starting_index = x_plain_size;
        final_index = 2 * x_plain_size;
    }
    bool res_ntt = 0;
    bool res_ntt1 = 0;
    cout << "Starting with index = " << starting_index << " ending at: " << final_index << endl;
    for (int index_value = starting_index; index_value < final_index; index_value += 100)
    {
        if (ci == 0)
            modulus_index = int((index_value + 1) / poly_modulus_degree);
        if (ci == 1)
            modulus_index = int(((index_value - x_plain_size) + 1) / poly_modulus_degree);

        modulus_value = modulus[modulus_index];
        if (modulus_value != modulus_value_last)
        {
          modulus_value_last = modulus_value;
          cout << "Mod: " << modulus_value << " index " << index_value << endl;
          modulus_k = coeff_modulus[modulus_index].value();
        }

        // cout <<index_value << ", "<< std::flush;
        for (int bit_change = 0; bit_change < modulus_value; bit_change += 10)
        {
            if (ci == 0)
                util::inverse_ntt_negacyclic_harvey(x_encrypted.data() + (modulus_index * coeff_count),
                                                    ntt_tables[modulus_index]);
            if (ci == 1)
                util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1) + (modulus_index * coeff_count),
                                                    ntt_tables[modulus_index]);
            x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
            cout << "encript non func:  " << index_value << " = " << x_encrypted[index_value] << endl;
            ntt_transformation(x_encrypted, ntt_tables, modulus_index, ci);
            if (x_encrypted[index_value] >= modulus_k)
            {
                cout << "Mas grande que el modulo!" << modulus_k << endl;
                x_encrypted[index_value] = x_encrypted_original[index_value];
                break;
            }
            decryptor.decrypt(x_encrypted, plain_result);
            encoder.decode(plain_result, result);
            res = diff_vec(input, result);
            if (res < MAX_DIFF)
            {
                saveDataLog(file_name, index_value, bit_change, res, !new_file);
                cout << res << " index_value: " << index_value  << " bit_changed: " << bit_change << endl;
            }
            restoreCiphertext(x_encrypted, x_encrypted_original, modulus_index,
                            poly_modulus_degree);
        }
    }
}

int main(int argc, char * argv[])
{
    bool TESTING = true;
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
        curr_point = atoi(argv[2]);
    if (argc>=4)
        max_value = atoi(argv[3])-curr_point;

    size_t poly_modulus_degree = 4096;
    vector<int> modulus;
    modulus ={ 40, 20, 20, 20};
    int coeff_modulus_size = modulus.size();
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

    cout << "params vs 2    " << parms.parms_id() << endl;
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);
    //auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto context_data_ptr = context.get_context_data(parms.parms_id());
    auto &context_data = *context_data_ptr;
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

    std::string file_name_c0 = "encryption_nonNTT_c0";
    std::string file_name_c1 = "encryption_nonNTT_c1";
    bool new_file = 1;
    Plaintext plain_result;
    vector<double> result;
    float res = 0;
    if (TESTING)
    {
        saveDataLog(file_name_c0, 0, 0, 0, new_file);
        int modulus_index = 0;
        int mod_change = 0;
        int modulus_name = 0;
        uint64_t modulus_k = 0;

        cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;

        cout << "starting c0" << endl;
        Bitflip_ci(x_encrypted, x_encrypted_original, 0, context, decryptor, input,
                   encoder, file_name_c0, poly_modulus_degree, modulus);
        cout << "c0 done, starting c1" << endl;
        Bitflip_ci(x_encrypted, x_encrypted_original, 1, context, decryptor,
                   input,  encoder, file_name_c1,  poly_modulus_degree, modulus);
        cout <<"c1 done" << endl;
    }
    // for (int index_value=0; index_value<x_plain_size; index_value+=100){
    //     modulus_index = int((index_value+1)/poly_modulus_degree);
    //     modulus_name = modulus[modulus_index];
    //     if(modulus_name!= mod_change){
    //         mod_change = modulus_name;
    //         cout << "Mod: " << modulus_name << " index " << index_value<<
    //         endl; modulus_k = coeff_modulus[modulus_index].value();
    //     }
    //     cout <<index_value << ", "<< std::flush;
    //     for (int bit_change=0; bit_change<modulus_name; bit_change+=10){
    //         util::inverse_ntt_negacyclic_harvey(x_encrypted.data()+(modulus_index
    //         * coeff_count), ntt_tables[modulus_index]);
    //         x_encrypted[index_value] = bit_flip(x_encrypted[index_value],
    //         bit_change); cout << "encript non func:  "<< index_value << " = "
    //         << x_encrypted[index_value] << endl;
    //         ntt_transformation(x_encrypted, coeff_modulus_size, coeff_count,
    //         ntt_tables, modulus_index, 0); if (x_encrypted[index_value] >=
    //         modulus_k){
    //             cout<< "Mas grande que el modulo!" << modulus_k << endl;
    //             x_encrypted[index_value] = x_encrypted_original[index_value];
    //             break;
    //         }
    //         decryptor.decrypt(x_encrypted, plain_result);
    //         encoder.decode(plain_result, result);
    //         res = diff_vec(input, result);
    //         if (res < 1000){
    //             saveDataLog(file_name_c0, index_value, bit_change, res,
    //             !new_file); cout << res << " index_value: "<< index_value <<
    //             " bit_changed: " << bit_change << endl ;
    //         }
    //         restoreCiphertext(x_encrypted, x_encrypted_original,
    //         modulus_index, poly_modulus_degree);
    //     }
    // }
    //         cout << "c0 done" <<endl;
    //       modulus_index = 0;
    //       res = 0;

    //      saveDataLog(file_name_c1, 0, 0, 0, new_file);
    //      // segundo cifrado.
    //      for (int index_value=x_plain_size; index_value<2*x_plain_size;
    //      index_value++){
    //          modulus_index =
    //          int(((index_value-x_plain_size)+1)/poly_modulus_degree);
    //          modulus_name = modulus[modulus_index];
    //          if(modulus_name!= mod_change){
    //              mod_change = modulus_name;
    //              modulus_k = coeff_modulus[modulus_index].value();
    //              cout << "Mod: " << modulus_name << " index " <<
    //              index_value<< endl;
    //          }
    //          cout <<index_value << ", "<< std::flush;
    //          for (int bit_change=0; bit_change<modulus[modulus_index];
    //          bit_change++){
    //              util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1)+(modulus_index
    //              * coeff_count), ntt_tables[modulus_index]);
    //              x_encrypted[index_value] =
    //              bit_flip(x_encrypted[index_value], bit_change);
    //              ntt_transformation(x_encrypted, coeff_modulus_size,
    //              coeff_count, ntt_tables, modulus_index, 0); if
    //              (x_encrypted[index_value] >= modulus_k){
    //                  cout<< "Mas grande que el modulo!" << modulus_k << endl;
    //                  x_encrypted[index_value] =
    //                  x_encrypted_original[index_value]; break;
    //              }
    //              decryptor.decrypt(x_encrypted, plain_result);
    //              encoder.decode(plain_result, result);
    //              res = diff_vec(input, result);
    //              if (res < 1000){
    //                  saveDataLog(file_name_c1, index_value, bit_change, res,
    //                  !new_file); cout << res << " index_value: "<<
    //                  index_value << " bit_changed: " << bit_change << endl ;
    //              }
    //              restoreCiphertext(x_encrypted, x_encrypted_original,
    //              modulus_index, poly_modulus_degree);
    //          }
    //      }


    else
    {
        decryptor.decrypt(x_encrypted, plain_result);
        encoder.decode(plain_result, result);
        res = diff_vec(input, result);
        if (res<1)
            cout << "Good decryption" << endl;

        bool ntt_res = true;
        bool ntt_temp = false;
        x_encrypted = x_encrypted_original;
        for (size_t modulus_index = 0; modulus_index < coeff_modulus_size-1; modulus_index++)
        {
            util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0) + (modulus_index * poly_modulus_degree),
                                                ntt_tables[modulus_index]);
            ntt_transformation(x_encrypted, ntt_tables, modulus_index, 0);
            ntt_temp = check_equality(x_encrypted, x_encrypted_original);
            ntt_res = ntt_res&ntt_temp;
        }
        cout << "c0 inverse ntt and ntt: " << std::boolalpha << ntt_res <<endl;
        ntt_res = true;
        ntt_temp = false;

        x_encrypted = x_encrypted_original;
        for (int modulus_index = 0; modulus_index < coeff_modulus_size-1; modulus_index++)
        {
            // start at c1, this is polydegree*K_rns_values
            util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1) + (modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
            ntt_transformation(x_encrypted, ntt_tables, modulus_index, 1);
            ntt_temp = check_equality(x_encrypted, x_encrypted_original);
            ntt_res = ntt_res&ntt_temp;
        }
        cout << "c1 inverse ntt and ntt: " << std::boolalpha << ntt_res <<endl;
    }

  return 0;
  //    cout <<  "validity ntt form: " << x_encrypted.is_ntt_form() << ", valid
  //    for:  "; cout <<is_valid_for(x_encrypted, context)<< ", buffer valid: "
  //    << is_buffer_valid(x_encrypted); cout << ",  data valid for: " <<
  //    is_data_valid_for(x_encrypted, context)<< ", metadata valid for:  ";
  //    cout << is_metadata_valid_for(x_encrypted, context)<<endl;
}