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

int main(int argc, char *argv[])
{
    int poly_degree = 4;

    if (argc>1){
        int num = atoi(argv[1]);
        if (num < 2){
            cout << "The polynomial results must have a degree greater than 2" << endl;
        }
        else
            poly_degree = num;
    }

    size_t poly_modulus_degree = 32768;
    if (argc>2){
        poly_modulus_degree = atoi(argv[2]);
        if (poly_modulus_degree != 32768 or poly_modulus_degree != 16384){
            cout << "The polynomial modulus must be" << endl;
        }

    }

    ////////////////////////////////    struct rusage usage;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }

    //////////////////////


    vector<int> modulus;
    double scale = pow(2.0, 40);
    if(poly_modulus_degree==4096){ // max degree 3
        modulus ={ 40, 20, 20, 20};
        scale = pow(2.0, 10);
    }
    if(poly_modulus_degree==8192){ // max degree 3
        modulus = {60};
        for (int i=0; i<poly_degree; i++){
            modulus.push_back(40);
        }
        modulus.push_back(60);
    }
    if(poly_modulus_degree==16384){ // max degree 7
        modulus = {60};
        for (int i=0; i<poly_degree; i++){
            modulus.push_back(40);
        }
        modulus.push_back(60);
    }
    if (poly_modulus_degree==32768){ // max degree 20
        modulus = {60};
        for (int i=0; i<poly_degree; i++){
            modulus.push_back(40);
        }
        modulus.push_back(60);
        //modulus ={ 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60 };
    }

    print_example_banner("Example: CKKS Basics");

    // std::array<std::uint64_t, prng_seed_uint64_count>
  //  prng_seed_type secret_key = {
  //          random_uint64(), random_uint64(), random_uint64(), random_uint64(),
  //          random_uint64(), random_uint64(), random_uint64(), random_uint64()
  //  };
  //  std::shared_ptr<UniformRandomGeneratorFactory> rg = make_shared<Blake2xbPRNGFactory(secret_key)>;

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));

    SEALContext context(parms);
    print_parameters(context);
    cout << endl;
    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    cout << "Params ids: No cambian \n";
    cout << secret_key.parms_id() << "\n";
    cout << context.key_parms_id() << "\n";
    keygen.create_public_key(public_key);
    cout << public_key.parms_id() << "\n\n";


    cout << "Secret key first value: Si cambia \n";
    cout << secret_key.data()[0] << "\n\n";
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);
   // print_vector(true_result, 3, 7);
   // cout << "Input vector: " << endl;
    Plaintext x_plain;
    Plaintext x_plain2;
    encoder.encode(input, scale, x_plain);

    cout << "coef y scale? " << x_plain[1] << "  " << x_plain[2] << "\n";
    cout<< sizeof(x_plain.data()) << "\n";
    cout<< x_plain[0] << "\n";
    cout<< x_plain[1] << "\n";
    encoder.encode(input, scale, x_plain2);
    cout << "Encoding first value: No cambia \n";
    cout << "Coeff 0 encode 1: " << x_plain[0] << "\n";
    cout << "Coeff 0 encode 2: " << x_plain2[0] << "\n\n";


    cout << "Encoding data: \n";
   //
 //  cout << "Coeff size: " << x_plain.coeff_modulus_size() << "\n";
 //  cout << "size: " << x_plain.size() << "\n";
 //  cout << "poly_modulus_degree: " << x_plain.poly_modulus_degree() << "\n";
   cout << "capacity: " << x_plain.capacity() << "\n\n";
    // A pesar de que me aseguro que estoy encirptando lo mismo iteradas veces
    // obtengo resultados diferentes.
    Ciphertext x_encrypted;
    Ciphertext x_encrypted2;

    cout << "Chequeando que si reencripto se vuelve a realizar lo mismo"<<endl;
    encryptor.encrypt(x_plain, x_encrypted);
    cout << x_encrypted[0] << endl;
    x_encrypted[0] =1 ;
    cout << x_encrypted[0] << endl;
    encryptor.encrypt(x_plain, x_encrypted);
    cout << x_encrypted[0] << endl;

    encryptor.encrypt(x_plain, x_encrypted2);




    cout << "Encryption data: \n";
    //
    int coeff_modulus_size = x_encrypted.coeff_modulus_size();
    int size = x_encrypted.size();
    int poly_modulus_degree2 = x_encrypted.poly_modulus_degree();
    int size_capacity = x_encrypted.size_capacity();

    cout << "Ciphertext size: " << coeff_modulus_size*size*poly_modulus_degree2 << "\n";
    cout << "Coeff size: " << coeff_modulus_size << "\n";
    cout << "size: " << size<< "\n";
    cout << "poly_modulus_degree: " << poly_modulus_degree2 << "\n\n";
 //   cout << "Largest size cipher capacity: " << size_capacity << "\n\n";


    cout << "Values of first coeff of encription with same keys: \n";
    cout << x_encrypted[0]<<"\n";
    cout << x_encrypted2[0]<<"\n";
    cout << "\n ";
    Plaintext plain_result;
    decryptor.decrypt(x_encrypted, plain_result);

    vector<double> result;
    encoder.decode(plain_result, result);
   // print_vector(input, 3, 7);
   // print_vector(result, 3, 7);

    return 0;
}
