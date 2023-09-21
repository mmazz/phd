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

int MAX_DIFF = 1;
float threshold = 10.0;
double CURR_POINT = 1;
double MAX_VALUE = 2.;

void nttBit_flip(Plaintext& x_plain, const util::NTTTables *ntt_tables, int index_value,  int bit_change, size_t modulus_index, int poly_modulus_degree);
void arguments(int argc, char *argv[], bool& TESTING, bool& RNS_ON, bool& NTT_ON, double& curr_point, double& max_value);
std::string fileName_hd(bool RNS_ON, bool NTT_ON);
std::string fileName_norm2(bool RNS_ON, bool NTT_ON);
std::string fileName_hd_decode(bool RNS_ON, bool NTT_ON);
std::string fileName_norm2_decode(bool RNS_ON, bool NTT_ON);
//                 test, rns, ntt,
int main(int argc, char * argv[])
{
    bool TESTING = false;
    bool RNS_ON = true;
    bool NTT_ON = true;
    double curr_point = CURR_POINT;
    double max_value = MAX_VALUE;
    size_t poly_modulus_degree = 4096;
    arguments(argc, argv, TESTING, RNS_ON, NTT_ON, curr_point, max_value);
    vector<int> modulus;
    if(RNS_ON)
        //modulus ={ 40, 20, 20, 20};
        modulus ={30, 20, 30};
    else
        modulus ={50, 20};
    std::string file_name_hd = fileName_hd(RNS_ON, NTT_ON);
    std::string file_name_norm2 = fileName_norm2(RNS_ON, NTT_ON);
    std::string file_name_hd_decode = fileName_hd_decode(RNS_ON, NTT_ON);
    std::string file_name_norm2_decode = fileName_norm2_decode(RNS_ON, NTT_ON);


    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
 //   vector<double> input;
    std::ifstream     file("data/example.txt");
    std::vector<double>  input(std::istream_iterator<double>{file},
                            std::istream_iterator<double>{});

//    input.reserve(size_input);
//    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

    print_example_banner("Example: CKKS random encoding");
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
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_plain_original);

    int x_plain_size = coeff_modulus_size * poly_modulus_degree;
    if (RNS_ON)
        x_plain_size = (coeff_modulus_size-1)* x_plain_size;

    Plaintext plain_result;
    Ciphertext x_encrypted;

    vector<double> result;
    uint64_t res_hamming = 0;
    double res_norm2 = 0;

    bool new_file = 1;
    std::string dir_name = "logs/log_encode/";

    saveDataLog(dir_name+file_name_hd, 0,  new_file);
    saveDataLog(dir_name+file_name_norm2, 0,  new_file);
    int modulus_index = 0;
    int modulus_bits = 0;

    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;

    int index_value = 0;

    cout << coeff_modulus_size << ", " << std::endl;
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
    {
        uint64_t modulus_value=0;
       // if(NTT_ON)
        modulus_value = coeff_modulus[modulus_index].value();
       // else
        //    modulus_value = coeff_modulus_nonNTT;

        cout << modulus_value << endl;

        // problemas de que es cero el modulo
        modulus_bits = modulus[modulus_index];
        size_t coeff_count = parms.poly_modulus_degree();
        for (; coeff_count--;index_value++)
        {
            cout << index_value << ", " << std::flush;
            //cout << index_value << " + "<< coeff_count<< ", " << std::flush;
            for (int bit_change=0; bit_change<modulus_bits; bit_change++)
            {
                if(NTT_ON)
                    x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                else
                    nttBit_flip(x_plain, ntt_tables, index_value, bit_change, modulus_index, poly_modulus_degree);

                if (x_plain[index_value] >= modulus_value)
                {
                  //  cout<< "Mas grande que el modulo!" <<  endl;
                    if(NTT_ON)
                        x_plain[index_value] = x_plain_original[index_value];
                    else
                        x_plain = x_plain_original;
                    saveDataLog(dir_name+file_name_hd, 0, !new_file);
                    saveDataLog(dir_name+file_name_norm2, 0, !new_file);
                }
                else
                {
                    encoder.decode(x_plain, result);

                    res_hamming = hamming_distance(input, result);
                    res_norm2 = norm2_vec(input, result);
                    //res_elem = (int)diff_elem(input, result, threshold);
                    saveDataLog(dir_name+file_name_hd_decode, res_hamming, !new_file);
                    saveDataLog(dir_name+file_name_norm2_decode, res_norm2, !new_file);

                    encryptor.encrypt(x_plain, x_encrypted);
                    decryptor.decrypt(x_encrypted, plain_result);
                    encoder.decode(x_plain, result);

                    res_hamming = hamming_distance(input, result);
                    res_norm2 = norm2_vec(input, result);
                    //res_elem = (int)diff_elem(input, result, threshold);
                    saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);
                    saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
                    // Si no tengo ntt tengo que cambiar todo el cifrado ya que se modifico todo
                    if(!NTT_ON)
                        x_plain = x_plain_original;
                    else
                        x_plain[index_value] = x_plain_original[index_value];
                }
            }
        }
    }
   return 0;
}


std::string fileName_hd(bool RNS_ON, bool NTT_ON)
{
    std::string file_name;
    if (RNS_ON && NTT_ON)
        file_name = "encodeHD_withRNS&NTT";
    else if (RNS_ON && !NTT_ON)
        file_name = "encodeHD_withRNS";
    else if (!RNS_ON && NTT_ON)
        file_name = "encodeHD_withNTT";
    else if (!RNS_ON && !NTT_ON)
        file_name = "encodeHD";
    return file_name;
}

std::string fileName_norm2(bool RNS_ON, bool NTT_ON)
{
    std::string file_name;
    if (RNS_ON && NTT_ON)
        file_name = "encodeN2_withRNS&NTT";
    else if (RNS_ON && !NTT_ON)
        file_name = "encodeN2_withRNS";
    else if (!RNS_ON && NTT_ON)
        file_name = "encodeN2_withNTT";
    else if (!RNS_ON && !NTT_ON)
        file_name = "encodeN2";
    return file_name;
}

std::string fileName_hd_decode(bool RNS_ON, bool NTT_ON)
{
    std::string file_name;
    if (RNS_ON && NTT_ON)
        file_name = "encodeHD_withRNS&NTT_decode";
    else if (RNS_ON && !NTT_ON)
        file_name = "encodeHD_withRNS_decode";
    else if (!RNS_ON && NTT_ON)
        file_name = "encodeHD_withNTT_decode";
    else if (!RNS_ON && !NTT_ON)
        file_name = "encodeHD_decode";
    return file_name;
}

std::string fileName_norm2_decode(bool RNS_ON, bool NTT_ON)
{
    std::string file_name;
    if (RNS_ON && NTT_ON)
        file_name = "encodeN2_withRNS&NTT_decode";
    else if (RNS_ON && !NTT_ON)
        file_name = "encodeN2_withRNS_decode";
    else if (!RNS_ON && NTT_ON)
        file_name = "encodeN2_withNTT_decode";
    else if (!RNS_ON && !NTT_ON)
        file_name = "encodeN2_decode";
    return file_name;
}

void arguments(int argc, char *argv[], bool& TESTING, bool& RNS_ON, bool& NTT_ON, double& curr_point, double& max_value)
{
   if (argc==1)
        cout << "Starting in default test mode: true" << endl;
    if (argc>=2)
    {
        if(atoi(argv[1])==1)
            RNS_ON = true;
        else
            RNS_ON = false;
        cout << "Starting in RNS mode: " << std::boolalpha << RNS_ON << endl;
    }
    if (argc>=3)
    {
        if(atoi(argv[2])==1)
            NTT_ON = true;
        else
            NTT_ON = false;
        cout << "Starting in NTT mode: " << std::boolalpha << NTT_ON << endl;
    }
    if (argc>=4)
        curr_point = atoi(argv[3]);
    if (argc>=5)
        max_value = atoi(argv[4])-curr_point;
}

void nttBit_flip(Plaintext& x_plain, const util::NTTTables *ntt_tables, int index_value,  int bit_change, size_t modulus_index, int poly_modulus_degree)
{
    util::inverse_ntt_negacyclic_harvey(x_plain.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    // probe con doble bit flip del mismo y da bien.
    x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);

    ntt_transformation(x_plain, ntt_tables, modulus_index, 0);
}
