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
#include <system_error>
#include <vector>
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

void nttBit_flip(Ciphertext& x_encrypted, const util::NTTTables *ntt_tables, int x_plain_size, int index_value, int bit_change, int modulus_index, int poly_modulus_degree, uint64_t k_rns_prime);
void nttBit_flip(Ciphertext& x_encrypted, const util::NTTTables *ntt_tables, int index_value, size_t c0, int bit_change, size_t modulus_index);
void arguments(int argc, char *argv[], bool& TESTING, bool& RNS_ON, bool& NTT_ON, double& curr_point, double& max_value);
std::string fileName(bool RNS_ON, bool NTT_ON);
std::string fileName_norm2(bool RNS_ON, bool NTT_ON);

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
    std::string file_name = fileName(RNS_ON, NTT_ON);
    std::string file_name_norm2 = fileName_norm2(RNS_ON, NTT_ON);

    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(size_input);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    print_vector(input, 3, 7);

    print_example_banner("Example: CKKS random encryption");
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
    encoder.encode(input, scale, x_plain);

    int x_plain_size = coeff_modulus_size * poly_modulus_degree;
    if (RNS_ON)
        x_plain_size = (coeff_modulus_size-1)* x_plain_size;
    // Este es el experimento que quiero, pero antes quiero chequear que los elementos
    // que estoy tocando del encoding sean los correctos.
    // Tiene exactamente x_plain_size por 2 de tamaño, como era de esperar
    Ciphertext x_encrypted;
    Ciphertext x_encrypted_original;
    // el .data() creo que es un puntero y entonces me puedo pasar....
    // pero dan igual, si hago .data(1) empiezo desde el segundo cifrado!!
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain, x_encrypted_original);

    Plaintext plain_result;
    vector<double> result;

    uint64_t res_hamming = 0;
    double res_norm2 = 0;

    bool new_file = 1;
    std::string dir_name = "logs/log_encrypt/";

    saveDataLog(dir_name+file_name, 0, new_file);
    saveDataLog(dir_name+file_name_norm2, 0,  new_file);

    int modulus_index = 0;
    int modulus_bits = 0;

    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
    // Realmente estan en orden, priemro lso coefs de los dos polinomios del rns,
    // despues los del segundo y asi...
    auto size = x_encrypted.size();
    cout << " Encrypted size: " << size << endl;
    cout << " Modulus size: " << coeff_modulus_size << endl;
    // Loopea primero por polynomio c0 o c1, aniado con un loop por cada rns y luego por cada bit.
    // el orden de 0 hasta el maximo coef, es primero todos los c0 de los rns, y luego los c1
    int index_value = 0;
    for (size_t i = 0; i < size; i++)
    {
        for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
        {
            uint64_t modulus_value = coeff_modulus[modulus_index].value();
            auto poly_modulus_degree = x_encrypted.poly_modulus_degree();

            modulus_bits = modulus[modulus_index];
            for (; poly_modulus_degree--; index_value++)
            {
                cout << index_value << ", " << std::flush;
                for (int bit_change=0; bit_change<modulus_bits; bit_change++)
                {
                    if(NTT_ON)
                        x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
                    else
                        nttBit_flip(x_encrypted, ntt_tables, index_value,  i, bit_change, modulus_index);

                    if (x_encrypted[index_value] >= modulus_value)
                    {
                        cout<< "Mas grande que el modulo!" <<  endl;
                        if(!NTT_ON)
                            x_encrypted = x_encrypted_original;
                        else
                            x_encrypted[index_value] = x_encrypted_original[index_value];
                        saveDataLog(dir_name+file_name, 0, !new_file);
                        saveDataLog(dir_name+file_name_norm2, 0, !new_file);

                    }
                    else
                    {
                        decryptor.decrypt(x_encrypted, plain_result);
                        encoder.decode(plain_result, result);
                        res_hamming = hamming_distance(input, result);
                        res_norm2 = norm2(input, result);
                        //res_elem = (int)diff_elem(input, result, threshold);
                        saveDataLog(dir_name+file_name, res_hamming, !new_file);
                        saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);

                        // Si no tengo ntt tengo que cambiar todo el cifrado ya que se modifico todo
                        if(NTT_ON)
                            x_encrypted[index_value] = x_encrypted_original[index_value];
                        else
                            x_encrypted = x_encrypted_original;
                    }
                }
            }
        }
    }

    //for (int index_value=0; index_value<2*x_plain_size; index_value++)
//    for (int index_value=10846; index_value<2*x_plain_size; index_value++)
//    {
//        modulus_index = floor(index_value/(2*poly_modulus_degree));
//        modulus_bits = modulus[modulus_index];
//        auto &coeff_modulus = parms.coeff_modulus();
//        k_rns_prime = coeff_modulus[modulus_index].value();
//        cout << index_value << ", " << std::flush;
//        //for (int bit_change=0; bit_change<modulus_bits; bit_change++)
//        for (int bit_change=17; bit_change<modulus_bits; bit_change++)
//        {
//            cout << "bit changed " << bit_change << " original coeff " << x_encrypted[index_value];
//            if(!NTT_ON)
//            {
//                nttBit_flip(x_encrypted,  ntt_tables, x_plain_size, index_value, bit_change, modulus_index, poly_modulus_degree, k_rns_prime);
//            }
//            else
//                x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);
//            cout << endl;
//
//           // x_encrypted[index_value] = k_rns_prime-25000;
//            cout << "new value " << x_encrypted[index_value] << " poly_degree " << 2*poly_modulus_degree << " kprime " << k_rns_prime << " other prine " << coeff_modulus[0].value()<< endl;
//            modulus_bits = count_diff_coef(x_encrypted, x_encrypted_original);
//            cout << modulus_bits << endl;
//
//            //  para el caso index_value 10846, del bit 17
//            // Cuando cambio este bit me tira el error:
//            // encrypted is not valid for encryption paramters. Pero veo que esta cerca del
//            // modulo pero sigue siendo mas chico...
//            // y el problema viene cuando tengo RNS. Entonces sera que estoy mirando mal el k primo?
//            if (x_encrypted[index_value] >= k_rns_prime)
//            {
//                cout<< "Mas grande que el modulo!" << k_rns_prime << endl;
//                if(!NTT_ON)
//                    x_encrypted = x_encrypted_original;
//                else
//                    x_encrypted[index_value] = x_encrypted_original[index_value];
//                saveDataLog(dir_name+file_name, 0, !new_file);
//            }
//            else
//            {
//                cout << " before decrypt" << endl;
//                decryptor.decrypt(x_encrypted, plain_result);
//                cout << " before decode" << endl;
//                encoder.decode(plain_result, result);
//                cout << " after decode" << endl;
//
//                res_elem = (int)diff_elem(input, result, threshold);
//                saveDataLog(dir_name+file_name, res_elem, !new_file);
//                cout <<  x_encrypted[index_value] << " " <<  x_encrypted_original[index_value] << endl;
//                // Si no tengo ntt tengo que cambiar todo el cifrado ya que se modifico todo
//                if(!NTT_ON)
//                    x_encrypted = x_encrypted_original;
//                else
//                    x_encrypted[index_value] = x_encrypted_original[index_value];
//            }
//        }
//   }

   return 0;
   //    cout <<  "validity ntt form: " << x_encrypted.is_ntt_form() << ", valid for:  ";
       //    cout <<is_valid_for(x_encrypted, context)<< ", buffer valid: " << is_buffer_valid(x_encrypted);
       //    cout << ",  data valid for: " <<  is_data_valid_for(x_encrypted, context)<< ", metadata valid for:  ";
       //    cout << is_metadata_valid_for(x_encrypted, context)<<endl;
}


std::string fileName(bool RNS_ON, bool NTT_ON)
{
    std::string file_name;
    if (RNS_ON && NTT_ON)
        file_name = "encryption_withRNS&NTT";
    else if (RNS_ON && !NTT_ON)
        file_name = "encryption_withRNS";
    else if (!RNS_ON && NTT_ON)
        file_name = "encryption_withNTT";
    else if (!RNS_ON && !NTT_ON)
        file_name = "encryption";
    return file_name;
}
std::string fileName_norm2(bool RNS_ON, bool NTT_ON)
{
    std::string file_name;
    if (RNS_ON && NTT_ON)
        file_name = "encryptionN2_withRNS&NTT";
    else if (RNS_ON && !NTT_ON)
        file_name = "encryptionN2_withRNS";
    else if (!RNS_ON && NTT_ON)
        file_name = "encryptionN2_withNTT";
    else if (!RNS_ON && !NTT_ON)
        file_name = "encryptionN2";
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

// Transformo el cifrado a su estado sin ntt, hago el bitflip y vuelvo al espacio ntt
void nttBit_flip(Ciphertext& x_encrypted, const util::NTTTables *ntt_tables, int x_plain_size, int index_value, int bit_change, int modulus_index, int poly_modulus_degree, uint64_t k_rns_prime)
{
    if (index_value<x_plain_size)
        util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    else
        util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    // probe con doble bit flip del mismo y da bien.
    x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);

    if (index_value<x_plain_size)
        ntt_transformation(x_encrypted, ntt_tables, modulus_index, 0);
    else
        ntt_transformation(x_encrypted, ntt_tables, modulus_index, 1);
}

void nttBit_flip(Ciphertext& x_encrypted, const util::NTTTables *ntt_tables, int index_value, size_t c0, int bit_change, size_t modulus_index)
{
    auto poly_modulus_degree = x_encrypted.poly_modulus_degree();
    if (c0==0)
        util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    else
        util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    // probe con doble bit flip del mismo y da bien.
    x_encrypted[index_value] = bit_flip(x_encrypted[index_value], bit_change);

    if (c0==0)
        ntt_transformation(x_encrypted, ntt_tables, modulus_index, 0);
    else
        ntt_transformation(x_encrypted, ntt_tables, modulus_index, 1);
}
