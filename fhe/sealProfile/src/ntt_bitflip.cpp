// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
/*
    Change one bit of the encryption and see what happens to the
    decryption.
    We can use RNS or not and also NTT or not.

    Tengo un loop por cada coeficiente, y luego por cada uno un loop por
    cada bit en el.
    El loop de coeficiente, deberia ser 2*4096 coeficientes por cada RNS
*/
#include <iostream>
#include <seal/ciphertext.h>
#include <seal/plaintext.h>
#include <seal/util/ntt.h>
#include <string>
#include <sys/types.h>
#include <system_error>
#include <vector>
#include <iterator>
#include "examples.h"
#include "utils_mati.h"
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include <memory>

#include <bitset>
#include <chrono>

using namespace seal;
using namespace std;

int main(int argc, char * argv[])
{
    size_t poly_modulus_degree = 4096;
    vector<int> modulus = {30, 30, 30};

    std::string dir_name = "logs/";
    std::string file_name_hd = "nttHD";
    std::string file_name_norm2 =  "nttN2";


    double scale = pow(2.0, 30);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
    std::ifstream     file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    print_vector(input, 3, 7);
    cout << "size vec " << input.size() << endl;
    print_example_banner("Example: CKKS random encoding");

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));
    auto &coeff_modulus = parms.coeff_modulus();
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
    auto ntt_tables = context_data.small_ntt_tables();


    // Me hago 3 copias codificadas.
    Plaintext x_plain;
    Plaintext x_nonntt_original;
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_nonntt_original);
    encoder.encode(input, scale, x_plain_original);

    int x_plain_size = (coeff_modulus_size-1) * x_plain_size;

    Plaintext plain_result;
    Ciphertext x_encrypted;


    vector<double> result;
    result.reserve(input.size());
    vector<double> result_decode;
    result_decode.reserve(input.size());

    uint64_t res_hamming = 0;
    double res_norm2 = 0;

    int modulus_bits = 30;
    bool new_file = 1;

    saveDataLog(dir_name+file_name_hd, 0,  new_file);
    saveDataLog(dir_name+file_name_norm2, 0,  new_file);
    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
    cout << "Coeff count " << x_plain.coeff_count() << endl;

    int count = 0;
    // Ne guardo la info sin ntt
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
    {
        util::inverse_ntt_negacyclic_harvey(x_plain.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
        util::inverse_ntt_negacyclic_harvey(x_nonntt_original.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    }

    // corroboro que estas dos cosas son iguales
    uint64_t hd_modified_test = hamming_distance(x_plain, x_nonntt_original);
    if (hd_modified_test != 0)
        cout << "First test fail! " << hd_modified_test << endl;

    // Agarro uno y lo vuelo a transformar y la comparo con la que no le aplique INTT
    ntt_transformation(x_plain, ntt_tables, 0, poly_modulus_degree);
    ntt_transformation(x_plain, ntt_tables, 1, poly_modulus_degree);
    hd_modified_test = hamming_distance(x_plain, x_plain_original);
    if (hd_modified_test != 0)
        cout << "Second test fail! " << hd_modified_test << endl;


    // Dado que dio todo bien le vuelvo aplicar INTT
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
        util::inverse_ntt_negacyclic_harvey(x_plain.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);


    size_t coeff_count = x_plain_original.coeff_count();

    // Basicamente modifico 1 bit a la codificacion en espacio comun y le aplico NTT y veo como cambio
    // en relacion al NTT original.
    int modulus_index = 0;
    for (int index_value=0; index_value<coeff_count; index_value++)
    {
        cout << index_value << ", " << std::flush;

        if(index_value>poly_modulus_degree)
            modulus_index = 1;
        for (int bit_change=0; bit_change<modulus_bits; bit_change++)
        {
            x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
            // Voy chequeando siempre que solo cambie 1 bit
            uint64_t hd_modified = hamming_distance(x_plain, x_nonntt_original);
            if (hd_modified != 1)
                cout << "More than one bit! Bits modified " << hd_modified << endl;

            ntt_transformation(x_plain, ntt_tables, modulus_index, poly_modulus_degree);
            res_hamming = hamming_distance(x_plain, x_plain_original);
            res_norm2 = norm2(x_plain, x_plain_original);
            saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);
            saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
            x_plain = x_nonntt_original;
        }
    }
   return 0;
}


