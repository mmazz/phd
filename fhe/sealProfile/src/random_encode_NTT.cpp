// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <seal/plaintext.h>
#include <seal/util/ntt.h>
#include <vector>
#include "examples.h"
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include <memory>

#include <bitset>
#include <chrono>

#include <iterator>
#include <fstream>
#include <algorithm> // for std::copy

// bool TESTING = false;
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

void saveData(int index_value, int bit_change, float res, int new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        logFile.open("/home/mmazz/phd/fhe/sealProfile/log_encode_nonNTT.txt", std::ios::out);
        logFile << "New file: " << endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/sealProfile/log_encode.txt", std::ios::app);
        logFile << "Diff: " << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
    }
    logFile.close();
}


float diff_vec(vector<double> &v1, vector<double> &v2){
    float res = 0;

    if (v1.size()==v2.size()){
        for (int i=0; i<v1.size(); i++){
            res += abs(v1[i]-v2[i]);
        }
        res = res/v1.size();
    }
    else{
        cout << "Vectores de diferente tamaño!!!" << endl;
    }
    return res;
}

int check_equality(Plaintext &x_plain, Plaintext &x_plain2, int size_x){
    int res=0;
    for(int i=0;i<size_x; i++){
        if(x_plain[i]!=x_plain2[i])
            res+=1;
    }
    if (res == 0)
        cout<< "Equality check is: Passed, " << res <<endl;
    else
        cout<< "Equality check is: Failed, " << res <<endl;
    return res;
}

void  reset_values(Plaintext &x_plain){
    // Me traigo los valores de x_plain antes de aplicarle  NTT
    std::string file_name = "/home/mmazz/phd/fhe/sealProfile/log_values_plainText_nonNTT_ckks.txt";
    std::ifstream file(file_name);
    std::vector<uint64_t>  data(std::istream_iterator<uint64_t>{file},
                        std::istream_iterator<uint64_t>{});

    uint64_t i_max = data[0];
    for (uint64_t i=0; i < i_max; i++){
        x_plain[i] = data[i+1];
    }
}

void ntt_transformation(Plaintext &x_plain, size_t coeff_modulus_size, size_t coeff_count, const seal::util::NTTTables* ntt_tables){
    for (size_t i = 0; i < coeff_modulus_size; i++)
    {
        util::ntt_negacyclic_harvey(x_plain.data(i * coeff_count), ntt_tables[i]);
    }
}

    // Confirmado!
uint64_t bit_flip(uint64_t original, ushort bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    return mask^original; // I flip the bit using xor with the mask.
}


int main()
{
    int poly_degree = 2;
    size_t poly_modulus_degree = 4096;
    size_t slot_count = poly_modulus_degree/2;
    vector<int> modulus;
    modulus ={ 40, 20, 20, 20};
    double scale = pow(2.0, 39);

    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1. / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }

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

    Plaintext x_plain_original;
    Plaintext x_plain;

    auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto &context_data = *context_data_ptr;

    auto &parms2 = context_data.parms();
    auto &coeff_modulus = parms2.coeff_modulus();

    std::size_t coeff_modulus_size = coeff_modulus.size();
    std::size_t coeff_count = parms.poly_modulus_degree();
    auto ntt_tables = context_data.small_ntt_tables();

    encoder.encode(input, scale, x_plain_original);
    encoder.encode(input, scale, x_plain);

    // Mete los valores previos a ser transormados con NTT
    reset_values(x_plain);

    // Confirmado! Mapea bien!
    int x_plain_size = (modulus.size()-1)*poly_modulus_degree;
    ntt_transformation(x_plain, coeff_modulus_size, coeff_count, ntt_tables);
    int result = check_equality(x_plain_original, x_plain, x_plain_size);

    if(result!=0)
        cout << "Error en la transformacion ntt"<< endl;

    reset_values(x_plain);

    int index_value = 0;
    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    if(TESTING){
        Ciphertext x_encrypted;
        Plaintext plain_result;
        vector<double> result;
        float res = 0;
        int new_file = 1;
        saveData(new_file, new_file, res, new_file); // simplemente crea el archivo
        new_file = 0;
        // Lupeo todo el vector, Son K polynomios con K = len(modulus)-1.
        // Cada uno tiene N = poly_modulus_degree coefficientes.
        // Estan pegados uno al lado del otro, entonces es un vector de N*K elementos.
        for (int index_value=0; index_value<x_plain_size; index_value++){
            cout << index_value << endl;
            // el modulo cambia por polynomio:
            int modulus_index = int((index_value+1)/poly_modulus_degree);
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            //for (int bit_change=0; bit_change<modulus[modulus_index]; bit_change++){
            for (int bit_change=0; bit_change<modulus[modulus_index]-2; bit_change++){
                reset_values(x_plain);
                x_plain[index_value] = bit_flip(x_plain[index_value], bit_change);
                ntt_transformation(x_plain, coeff_modulus_size, coeff_count, ntt_tables);
                encryptor.encrypt(x_plain, x_encrypted);
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
             //   cout << res << endl;
                if (res < 10000){
                    saveData(index_value, bit_change, res, new_file);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
            }
        }
        cout << x_encrypted[x_plain_size*2-1] << endl;
    }
       return 0;
}
