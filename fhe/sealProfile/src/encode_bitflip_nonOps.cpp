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

void nttBit_flip(Plaintext& x_plain, const util::NTTTables *ntt_tables, int index_value,  int bit_change, size_t modulus_index, int poly_modulus_degree);

int main(int argc, char * argv[])
{
    size_t poly_modulus_degree = 4096;
    vector<int> modulus = {60, 30};

    std::string dir_name = "logs/log_encode/";
    std::string file_name_hd = "encodeHD_nonOps";
    std::string file_name_norm2 =  "encodeN2_nonOps";
    std::string file_name_hd_decode = "encodeHD_nonOps_decode";
    std::string file_name_norm2_decode = "encodeN2_nonOps_decode";

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

    auto context_data_ptr = context.get_context_data(parms.parms_id());
    auto &context_data = *context_data_ptr;
    auto ntt_tables = context_data.small_ntt_tables();


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

    int modulus_index = 0;
    int modulus_bits = 0;
    int index_value = 0;
    bool new_file = 1;

    saveDataLog(dir_name+file_name_hd, 0,  new_file);
    saveDataLog(dir_name+file_name_norm2, 0,  new_file);
    saveDataLog(dir_name+file_name_hd_decode, 0, new_file);
    saveDataLog(dir_name+file_name_norm2_decode, 0, new_file);

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
                nttBit_flip(x_plain, ntt_tables, index_value, bit_change, modulus_index, poly_modulus_degree);
                if (x_plain[index_value] >= modulus_value)
                {
                    x_plain = x_plain_original;
                    saveDataLog(dir_name+file_name_hd, 4096*60, !new_file);
                    saveDataLog(dir_name+file_name_norm2, -1, !new_file);
                    // la mayor cantidad de bits...
                    saveDataLog(dir_name+file_name_hd_decode, 4096*60, !new_file);
                    saveDataLog(dir_name+file_name_norm2_decode, -1, !new_file);
                }

                else
                {
                    encoder.decode(x_plain, result_decode);
                    res_hamming_decode = hamming_distance(input, result_decode);
                    res_norm2_decode = norm2(input, result_decode);
                    saveDataLog(dir_name+file_name_hd_decode, res_hamming_decode, !new_file);
                    saveDataLog(dir_name+file_name_norm2_decode, res_norm2_decode, !new_file);

                    encryptor.encrypt(x_plain, x_encrypted);
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

                    x_plain = x_plain_original;
                }
            }
        }
    }
   return 0;
}

void nttBit_flip(Plaintext& x_plain, const util::NTTTables *ntt_tables, int index_value,  int bit_change, size_t modulus_index, int poly_modulus_degree)
{
    util::inverse_ntt_negacyclic_harvey(x_plain.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    // probe con doble bit flip del mismo y da bien.
    x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);

    ntt_transformation(x_plain, ntt_tables, modulus_index, 0);
}
