// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include "examples.h"

using namespace seal;
using namespace std;
int MAX_DIFF = 1000;

int main(int argc, char * argv[])
{
    bool TESTING = true;
    bool RNS = true;
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
    {
        if(atoi(argv[2])==1)
            RNS = true;
        else
            RNS = false;
        cout << "Starting in RNS mode: " << std::boolalpha << RNS << endl;
    }
    if (argc>=4)
        curr_point = atoi(argv[3]);
    if (argc>=5)
        max_value = atoi(argv[4])-curr_point;
    size_t poly_modulus_degree = 4096;
    vector<int> modulus;
    if(RNS)
        modulus ={ 40, 20, 20, 20};
    else
        modulus ={ 60, 20};
    double scale = pow(2.0, 40);

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

    print_example_banner("Example: CKKS random encoding without ntt");
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

    Ciphertext x_encrypted;
    Plaintext plain_result;
    vector<double> result;
    float res = 0;
    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    if(TESTING){
        // se esta rompiendo con algunos cambios de bits altos en algunos coeffs.
        // ejemplo coeff 4100 al cambiar el bit 19.
        // parece que se rompe el is_data_valid_for(x_plain, context)
        // como esta en forma de ntt, en valcheck.cpp linea 263, chequea ahi.

        bool new_file = 1;
        std::string file_name;
        if (RNS)
            file_name = "encoding_withRNS";
        else
            file_name = "encoding_nonRNS";

        saveDataLog(file_name, 0, 0, res, new_file); // simplemente crea el archivo
        int modulus_index = 0;
        int modulus_bits = 0;
        uint64_t k_rns_prime = 0;
        cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
        // Lupeo todo el vector, Son K polynomios con K = len(modulus)-1.
        // Cada uno tiene N = poly_modulus_degree coefficientes.
        // Estan pegados uno al lado del otro, entonces es un vector de N*K elementos.
        for (int index_value=0; index_value<x_plain_size; index_value++)
        {
            modulus_index = int(index_value/poly_modulus_degree);
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            modulus_bits = modulus[modulus_index];
            k_rns_prime = coeff_modulus[modulus_index].value();
            cout <<index_value << ", "<< std::flush;
            for (short bit_change=0; bit_change<modulus[modulus_index]; bit_change++)
            {
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                if (x_plain[index_value] >= k_rns_prime){
                    cout<< "Mas grande que el modulo!" << k_rns_prime << endl;
                    x_plain[index_value]= x_plain_original[index_value] ;
                    break;
                }
                encryptor.encrypt(x_plain, x_encrypted);
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
                if (res < MAX_DIFF){
                    saveDataLog(file_name,index_value, bit_change, res, new_file);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                // aca si puedo solo recuperar el valor ya que no hubo ntt ni nada
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
