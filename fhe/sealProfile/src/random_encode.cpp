// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include "examples.h"

//bool TESTING = false;
bool TESTING = true;

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
int main(int argc, char * argv[])
{
    bool TESTING = true;
    double curr_point = 0;
    double max_value = 1.;
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
    double scale = pow(2.0, 39);

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

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);


    Plaintext x_plain;
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain);
    encoder.encode(input, scale, x_plain_original);
    int x_plain_size = poly_modulus_degree + 2*poly_modulus_degree;
    int bits = 2;

    std::string file_name = "encoding";
    int new_file = 1;
    saveDataLog(file_name, 0, 0, 0, new_file);
    new_file = 0;
    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    if(TESTING){
        // se esta rompiendo con algunos cambios de bits altos en algunos coeffs.
        // ejemplo coeff 4100 al cambiar el bit 19.
        // parece que se rompe el is_data_valid_for(x_plain, context)
        // como esta en forma de ntt, en valcheck.cpp linea 263, chequea ahi.
        Ciphertext x_encrypted;
        Plaintext plain_result;
        vector<double> result;
        // Lupeo todo el vector, Son K polynomios con K = len(modulus)-1.
        // Cada uno tiene N = poly_modulus_degree coefficientes.
        // Estan pegados uno al lado del otro, entonces es un vector de N*K elementos.
        for (int index_value=0; index_value<x_plain_size; index_value++){
            cout << index_value << endl;
            int modulus_index = int(index_value/poly_modulus_degree)+1;
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            for (short bit_change=0; bit_change<modulus[modulus_index]-2; bit_change++){
                //cout << bit_change << endl;
               // cout << x_plain[index_value]<< endl;
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
              //  cout << x_plain[index_value] << endl;
               // cout <<  "validity " <<x_plain.is_ntt_form() << " " <<is_valid_for(x_plain, context)<< " " << is_buffer_valid(x_plain)<< " " <<  is_data_valid_for(x_plain, context)<< " " << is_metadata_valid_for(x_plain, context)<<endl;
                encryptor.encrypt(x_plain, x_encrypted);
                decryptor.decrypt(x_encrypted, plain_result);
              //  cout << "decode?" << endl;
                encoder.decode(plain_result, result);
                float res = diff_vec(input, result);
                if (res < 10000){
                    saveDataLog(file_name,index_value, bit_change, res, new_file);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                x_plain[index_value]= x_plain_original[index_value] ;
            }
        }
    }


    // Estaria teniendo diferentes valores con el log_ckks.txt que es un log que mete
    // el dato dentro del codigo en un log.
    // Ya esta, estaba mirando los datos antes de aplicarle NTT. Dan lo mismo.
    // Queda la duda si la sensibilidad viene de parte de NTT y por eso cambiando un
    // unico bit da todo mal.
    if(!TESTING){
        int index_value = 0;
        for(int i=0; i<poly_modulus_degree; i++){
            for(int j=0; j<modulus.size()-1; j++){
                index_value = i + (j*poly_modulus_degree);
                cout << index_value << ": " << x_plain[index_value] << endl;
                //cout << index_value << ": " << x_plain.data()[index_value] <<endl;
            }
        }
    }

       return 0;
}
