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

    std::string dir_name = "logs/log_encode/";
    std::string file_name_hd = "encodeHD_withRNS&NTT";
    std::string file_name_norm2 =  "encodeN2_withRNS&NTT";
    std::string file_name_hd_decode = "encodeHD_withRNS&NTT_decode";
    std::string file_name_norm2_decode = "encodeN2_withRNS&NTT_decode";

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

    Plaintext x_plain;
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_plain_original);

    int x_plain_size = (coeff_modulus_size-1) * x_plain_size;

    Plaintext plain_result;
    Ciphertext x_encrypted;


    vector<double> result;
    result.reserve(input.size());
    vector<double> result_decode;
    result_decode.reserve(input.size());



    uint64_t res_hamming = 0;
    uint64_t res_hamming_decode = 0;
    double res_norm2 = 0;
    double res_norm2_decode = 0;
    uint64_t max_hd = 60*coeff_modulus_size;
    double max_n2 = -100000;
    int modulus_index = 0;
    int modulus_bits = 0;
    int index_value = 0;
    bool new_file = 1;

    saveDataLog(dir_name+file_name_hd, res_hamming,  new_file);
    saveDataLog(dir_name+file_name_norm2, res_norm2,  new_file);
    saveDataLog(dir_name+file_name_hd_decode, res_hamming_decode, new_file);
    saveDataLog(dir_name+file_name_norm2_decode, res_norm2_decode, new_file);

    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;

    cout << "Coeff modulus size: " << coeff_modulus_size << std::endl;
    int count = 0;
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
    {
        uint64_t modulus_value=0;
        modulus_value = coeff_modulus[modulus_index].value();
        cout << "Modulus value " << modulus_value << endl;

        modulus_bits = modulus[modulus_index];
        size_t coeff_count = parms.poly_modulus_degree();

        for (; coeff_count--; index_value++)
        {
            cout << index_value << ", " << std::flush;

            for (int bit_change=0; bit_change<modulus_bits; bit_change++)
            {
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                uint64_t hd_modified = hamming_distance(x_plain, x_plain_original);
                Plaintext x_plain_new(x_plain);
                if (hd_modified != 1)
                    cout << "More than one bit! Bits modified " << hd_modified << endl;

                if (x_plain[index_value] >= modulus_value)
                {
                    x_plain[index_value] = x_plain_original[index_value];
                    saveDataLog(dir_name+file_name_hd, max_hd, !new_file);
                    saveDataLog(dir_name+file_name_norm2, max_n2, !new_file);
                    // la mayor cantidad de bits...
                    saveDataLog(dir_name+file_name_hd_decode, max_hd, !new_file);
                    saveDataLog(dir_name+file_name_norm2_decode, max_n2, !new_file);
                }

                else
                {


                    encoder.decode(x_plain_new, result_decode);
                    res_hamming_decode = hamming_distance(input, result_decode);
                    res_norm2_decode = norm2(input, result_decode);
                    saveDataLog(dir_name+file_name_hd_decode, res_hamming_decode, !new_file);
                    saveDataLog(dir_name+file_name_norm2_decode, res_norm2_decode, !new_file);

                    encryptor.encrypt(x_plain_new, x_encrypted);
                    decryptor.decrypt(x_encrypted, plain_result);
                    encoder.decode(plain_result, result);


                    res_hamming = hamming_distance(input, result);
                    res_norm2 = norm2(input, result);
                    //cout << x_plain_original[index_value]<<  " vs "  << x_plain[index_value] << " , indexvalue: " << index_value << " bitchange: " << bit_change << " hd: " << res_hamming << " n2: "<< res_norm2 << endl;
                    saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);
                    saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);

                //    uint64_t hd_decode = hamming_distance(x_plain, plain_result);
                //    uint64_t hd_res = hamming_distance(result, result_decode);
                //    cout << "HD of encoding with encrypt and without " << hd_decode << " res: " << hd_res<< endl;

                    x_plain[index_value] = x_plain_original[index_value];
                }
            }
        }
    }
   return 0;
}


