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
void saveData(int index_value, int bit_change, float res)
{
    std::fstream logFile;
    // Open File
    logFile.open("/home/mmazz/phd/fhe/sealProfile/log_encrypt.txt", std::ios::app);
    //Write data into log file
    logFile << "Diff: :" << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
    // close file stream
    logFile.close();

}

float diff_vec(vector<double> v1, vector<double> v2){
    //vector<double> res(v1.size());
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
//uint8_t* bits_x = (uint8_t*)(&x_plain);
//uint8_t mask = (1 << bit);
//
//uint8_t nuevo = bits_x[byte] & ~(mask);
//uint8_t negbit =~(bits_x[byte] & (mask));
//
//bits_x[byte] = (nuevo & ~(mask)) | (negbit & (mask));


uint64_t bit_flip(uint64_t &original, ushort bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    //cout <<"mask 0x" <<std::hex << mask << std::endl;
    return mask^original; // I flip the bit using xor with the mask.
}
int main()
{
    int poly_degree = 2;
    size_t poly_modulus_degree = 4096;
    size_t slot_count = poly_modulus_degree/2;

    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1. / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }


    vector<int> modulus;
    modulus ={ 40, 20, 20, 20};
    double scale = pow(2.0, 39);


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
    // Tiene una long de poly_degree por 8
    keygen.create_public_key(public_key);


    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);

    CKKSEncoder encoder(context);

    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);
    int x_plain_size = (modulus.size()-1)*poly_modulus_degree;
    int index_value = 0;
    uint64_t original_value = 0;

    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    // Tiene exactamente x_plain_size por 2 de tamaño, como era de esperar
    Ciphertext x_encrypted;
    Ciphertext x_encrypted_original;
    // el .data() creo que es un puntero y entonces me puedo pasar....
    // pero dan igual, si hago .data(1) empiezo desde el segundo cifrado!!
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain, x_encrypted_original);
    if(TESTING){
        Plaintext plain_result;
        vector<double> result;
        // chequear que es algo analogo a encoding...
        //for (int index_value=0; index_value<2*x_plain_size; index_value++){
        int modulus_index = 0;
        float res = 0;
        int mod_change = 0;
        int mod = 0;
        for (int index_value=0; index_value<x_plain_size; index_value++){
            modulus_index = int((index_value+1)/poly_modulus_degree);
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            mod = modulus[modulus_index];
            if(mod!= mod_change){
                mod_change = mod;
                cout << "Mod: " << mod << " index " << index_value<< endl;
            }
            cout<< index_value<< endl;
           // cout << "original: " << x_encrypted[index_value]  << endl;
            for (int bit_change=0; bit_change<mod-2; bit_change++){
               // cout << index_value << " " << bit_change << endl;
              //  cout << x_encrypted[index_value]  << endl;
                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);

              //  cout << x_encrypted[index_value]  << endl;
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
                if (res < 1000){
                    saveData(index_value, bit_change, res);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                x_encrypted[index_value] = x_encrypted_original[index_value];
             //   cout<< x_encrypted[index_value] << " = " << x_encrypted_original[index_value]<< endl;
            }
        }
        cout << "c0 done" <<endl;
        modulus_index = 0;
        res = 0;
        for (int index_value=x_plain_size; index_value<2*x_plain_size; index_value++){
            modulus_index = int((index_value/2+1)/poly_modulus_degree);
            // Para cada elemento le cambio un bit. Se supone que cada coefficiente tiene tantos
            // bits como su numero primo asociado que esta en el vector modulus.
            cout<< index_value<< endl;
            for (int bit_change=0; bit_change<modulus[modulus_index]-2; bit_change++){
                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
                decryptor.decrypt(x_encrypted, plain_result);
                encoder.decode(plain_result, result);
                res = diff_vec(input, result);
                if (res < 1000){
                    saveData(index_value, bit_change, res);
                    cout << res << " index_value: "<< index_value << " bit_changed: " << bit_change << endl ;
                }
                x_encrypted[index_value] = x_encrypted_original[index_value];
               // cout<< x_encrypted[index_value] << " = " << x_encrypted_original[index_value]<< endl;
            }
        }

    }

       return 0;
}
