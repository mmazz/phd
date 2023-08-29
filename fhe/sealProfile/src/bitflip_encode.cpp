// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include "examples.h"

using namespace seal;
using namespace std;
int MAX_DIFF = 1;
float threshold = 0.1;
double CURR_POINT = 1;
double MAX_VALUE = 2.;
int size_input= 1000;

uint64_t max_valueBit(int bits)
{
    std::string s;
    for (int i=0; i<bits; i++)
        s.insert(s.begin()+i, '1');
    std::bitset<64> a(s);
    uint64_t x =  a.to_ulong();
    return x;
}

int main(int argc, char * argv[])
{
    bool TESTING = true;
    bool RNS = true;
    double curr_point = CURR_POINT;
    double max_value = MAX_VALUE;
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
    //modulus ={ 30, 30, 20};
    double scale = pow(2.0, 40);

    size_t slot_count = size_input;
    vector<double> input;
    input.reserve(slot_count);
    input_creator(input, poly_modulus_degree, slot_count,curr_point, max_value);
    print_vector(input, 3, 7);

    // se copia de forma deep
    vector<double> input_orig = input;

    print_example_banner("Example: CKKS random encoding bitflip commparison");
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


    CKKSEncoder encoder(context);

    auto &coeff_modulus = parms.coeff_modulus();
    Plaintext x_plain;
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_plain_original);
    int x_plain_size = (modulus.size() - 1) * poly_modulus_degree;

    int index_rate = 1;
    short bit_rate = 1;
    if(TESTING)
    {
        index_rate = 100;
        bit_rate = 15;
    }
    bool new_file = 1;
    std::string dir_name = "new_log_encode/";
    std::string file_name = "encoding";

    saveEncodig(dir_name + file_name,  new_file);
    //saveEncodig(dir_name + file_name,  x_plain,  x_plain_size, !new_file);

    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
    int bits_coeff = modulus[0];
    uint64_t max_coeff = max_valueBit(bits_coeff);

    for (int index_value=0; index_value<slot_count; index_value+=index_rate)
    {
        cout <<index_value << ", "<< std::flush;
        std::vector<int> diff_coeff;
        for (short bit_change=0; bit_change<64; bit_change+=bit_rate)
        {
            input[index_value] = bit_flip(input[index_value], bit_change);
            if (input[index_value] <= max_coeff)
            {
   //             std::cout << "Bit flip: " << bit_change << ", origin value: " << input_orig[index_value] << ", new: "
   //                 << std::setprecision(53) << input[index_value] << std::endl;
                encoder.encode(input, scale, x_plain);
                comparePlaintext(diff_coeff, x_plain, x_plain_original, bits_coeff);
               // int diff = comparePlaintextCoeff(x_plain, x_plain_original);
               // diff_coeff.push_back(diff);
            }
            else
                std::cout << " Fuera de rango en bit " << bit_change << std::endl;
            input[index_value] = input_orig[index_value];
            saveEncodig(dir_name + file_name, diff_coeff, !new_file);
        }
    }
   return 0;
}
