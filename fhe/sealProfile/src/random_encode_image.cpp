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
#include <cstdint>
#include <iostream>
#include <seal/ciphertext.h>
#include <seal/plaintext.h>
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
    double scale = pow(2.0, 40);
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

    encryptor.encrypt(x_plain, x_encrypted);
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);

    std::ofstream outfile_nonBitFlip;
    outfile_nonBitFlip.open("data/example_nonBitflip.txt", std::ios_base::out);
    for(int i=0; i<input.size()-1; i++)
    {
        outfile_nonBitFlip<< result[i] << ",";
       // cout << "i: " <<i << " inputs: " << input[i] << " vs " << result[i] << endl;
    }
    outfile_nonBitFlip << result[input.size()-1] << endl;

    uint64_t res_hamming = 0;
    double res_norm2 = 0;
    int modulus_index = 0;
    int modulus_bits = 0;
    int index_value = 0;
    bool new_file = 1;
    std::ofstream outfile;

    cout << "valor2! " << res_hamming << endl;
    outfile.open("data/example_nonbitflip.txt", std::ios_base::out);
    for(int i=0; i<input.size()-1; i++)
        outfile << result[i] << ",";
    outfile << result[input.size()-1] << endl;
    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;

    cout << "Coeff modulus size: " << coeff_modulus_size << std::endl;

    for (size_t modulus_index = 1; modulus_index < coeff_modulus_size; modulus_index++)
    {
        uint64_t modulus_value=0;
        modulus_value = coeff_modulus[modulus_index].value();
        //cout << modulus_value << endl;

        modulus_bits = modulus[modulus_index];
        cout << modulus_bits << endl;
        size_t coeff_count = parms.poly_modulus_degree();

        coeff_count=100;
        index_value=1490;
        for (; coeff_count--; index_value++)
        {
            cout << index_value << ", " << std::flush;

            for (int bit_change=0; bit_change<modulus_bits; bit_change++)
            {
      //          cout << "bit change" << bit_change << endl;
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);

                if (x_plain[index_value] >= modulus_value)
                    x_plain[index_value] = x_plain_original[index_value];

                else
                {
                    uint64_t hd_bitflip = hamming_distance(x_plain, x_plain_original);
                    //cout << "Hamming distance x_plain bit flip: " << hd_bitflip << endl;
                    //cout << "Coeff count " << x_plain.coeff_count() << endl;
                    encoder.decode(x_plain, result);
                    res_hamming = hamming_distance(input, result)*100/(4096*60);
                    res_norm2 = norm2(input, result);
                    if(res_hamming<6)
                    {
                        std::ofstream outfile_decode;
                        cout << "valor!" << endl;
                        outfile_decode.open("data/example_bitflip_decode.txt", std::ios_base::out);
                        for(int i=0; i<input.size()-1; i++)
                            outfile_decode << result[i] << ",";
                        outfile_decode << result[input.size()-1] << endl;
                    }
                    encryptor.encrypt(x_plain, x_encrypted);
                    decryptor.decrypt(x_encrypted, plain_result);
                    encoder.decode(plain_result, result);
                    res_hamming = hamming_distance(input, result)*100/(4096*60);
                    res_norm2 = norm2(input, result);

                    x_plain[index_value] = x_plain_original[index_value];

                    if(res_hamming<6)
                    {
                        std::ofstream outfile;

                        cout << "valor2! " << res_hamming << endl;
                        outfile.open("data/example_bitflip.txt", std::ios_base::out);
                        for(int i=0; i<input.size()-1; i++)
                            outfile << result[i] << ",";
                        outfile << result[input.size()-1] << endl;
                    }
                }
            }
        }
    }

   return 0;
}


