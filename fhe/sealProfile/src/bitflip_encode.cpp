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
        curr_point = atoi(argv[3]);
    if (argc>=4)
        max_value = atoi(argv[4])-curr_point;


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

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);

    auto &coeff_modulus = parms.coeff_modulus();
    Plaintext x_plain;
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_plain_original);
    int x_plain_size = (modulus.size() - 1) * poly_modulus_degree;

    Plaintext plain_result;
    vector<double> result;
    float res = 0;
    float res_elem = 0;

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

    saveEncodig(dir_name + file_name,  x_plain,  x_plain_size, new_file);
    saveEncodig(dir_name + file_name,  x_plain,  x_plain_size, !new_file);

    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;

    for (int index_value=0; index_value<slot_count; index_value+=index_rate)
    {
        cout <<index_value << ", "<< std::flush;
        for (short bit_change=0; bit_change<30; bit_change+=bit_rate)
        {
            input[index_value] = bit_flip(input[index_value], bit_change);
            encoder.encode(input, scale, x_plain);
            saveEncodig(dir_name + file_name,  x_plain,  x_plain_size, !new_file);
            input[index_value] = input_orig[index_value];
        }
    }




       return 0;
}
