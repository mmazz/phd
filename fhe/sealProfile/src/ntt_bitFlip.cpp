// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
/*
 * Codifico un input, lo saco de su version NTT, y alli le cambio un bit,
 * y lo regreso a su representacion NTT.
 * luego calculo tanto el hamming distance como la norma 2 contra el NTT original.
 */
#include <iostream>
#include <seal/ciphertext.h>
#include <seal/plaintext.h>
#include <seal/util/ntt.h>
#include <string>
#include <strings.h>
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
    if (argc>1){
        int num = atoi(argv[1]);
        if (num!=12 && num!= 13 && num!=14 && num!=15){
            cout << "Solo se permiten valores de 12 a 15" << endl;
        }
        else
            poly_modulus_degree = pow(2,num);
    }

    //size_t poly_modulus_degree = 4096;
    vector<int> modulus = {60, 30};
    int modulus_bits = 60;

    std::string name;
    if(poly_modulus_degree==4096)
        name = "_4096";
    else if(poly_modulus_degree==8192)
        name = "_8192";
    else if(poly_modulus_degree==16384)
        name = "_16384";
    else
        name = "_32768";

    std::string dir_name = "logs/";
    std::string file_name_hd = "antt_bitFlip_HD"+name;
    std::string file_name_norm2 =  "antt_bitFlip_N2"+name;

    cout << file_name_norm2 << endl;

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
    //auto context_data_ptr = context.get_context_data(context.first_parms_id());
    auto context_data_ptr = context.get_context_data(parms.parms_id());
    auto &context_data = *context_data_ptr;
    auto ntt_tables = context_data.small_ntt_tables();


    // Me hago 3 copias codificadas.
    Plaintext x_plain_nonNTT;
    Plaintext x_nonNTT_original;
    Plaintext x_plain_original;
    encoder.encode(input, scale, x_plain_nonNTT);
    encoder.encode(input, scale, x_nonNTT_original);
    encoder.encode(input, scale, x_plain_original);

    int x_plain_size = (coeff_modulus_size-1) * x_plain_size;

    Plaintext plain_result;
    Ciphertext x_encrypted;


    vector<double> result;
    result.reserve(input.size());
    vector<double> result_decode;
    result_decode.reserve(input.size());

    uint64_t res_hamming = 0;
    double res_norm2 = 0;

    bool new_file = 1;

    saveDataLog(dir_name+file_name_hd, res_hamming,  new_file);
    saveDataLog(dir_name+file_name_norm2, res_norm2,  new_file);
    cout << "Starting bitflips with x_plain_size of: " << x_plain_size << endl;
    cout << "Coeff count " << x_plain_nonNTT.coeff_count() << endl;


//    for (int i = 0; i<ntt_tables[0].coeff_count(); i++)
//    {
//        std::cout << ntt_tables[0].get_root() << std::endl;
//    }





    int count = 0;
    // Ne guardo la info sin ntt
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
    {
        util::inverse_ntt_negacyclic_harvey(x_plain_nonNTT.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
        util::inverse_ntt_negacyclic_harvey(x_nonNTT_original.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
    }
    // corroboro que estas dos cosas son iguales
    uint64_t hd_modified_test = hamming_distance(x_plain_nonNTT, x_nonNTT_original);
    if (hd_modified_test != 0)
        cout << "Zero test fail! " << hd_modified_test << endl;
    // corroboro que estas dos cosas son iguales
    hd_modified_test = hamming_distance(x_plain_nonNTT, x_plain_original);
    if (hd_modified_test == 0)
        cout << "First test fail! " << hd_modified_test << endl;
    // Agarro uno y lo vuelo a transformar y la comparo con la que no le aplique INTT
    ntt_transformation(x_plain_nonNTT, ntt_tables, 0, poly_modulus_degree);
    hd_modified_test = hamming_distance(x_plain_nonNTT, x_plain_original);
    if (hd_modified_test != 0)
        cout << "Second test fail! " << hd_modified_test << endl;


    // el inverse_ntt_negacyclic_harvey parece que al menos no cambia eso....
    bool isNTT = x_nonNTT_original.is_ntt_form();
    cout << "Is in NTT form? " << isNTT << endl;


    // Dado que dio todo bien le vuelvo aplicar INTT
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
        util::inverse_ntt_negacyclic_harvey(x_plain_nonNTT.data(0)+(modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);

    hd_modified_test = hamming_distance(x_plain_nonNTT, x_nonNTT_original);
    if (hd_modified_test != 0)
        cout << "Third test fail! " << hd_modified_test << endl;


    size_t coeff_count = x_plain_original.coeff_count();

    // Basicamente modifico 1 bit a la codificacion en espacio comun y le aplico NTT y veo como cambio
    // en relacion al NTT original.
    int modulus_index = 0;
    cout << "Modulus bits: " << modulus_bits << " Poly degree: " << poly_modulus_degree << " coeff count " << coeff_count <<endl;
    for (int index_value=0; index_value<coeff_count; index_value++)
    {
        cout << index_value << ", " << std::flush;

        if(index_value>poly_modulus_degree)
            modulus_index = 1;
        // En este caso elegi 60 bits para el modulo
        for (int bit_change=0; bit_change<modulus_bits; bit_change++)
        {
            x_plain_nonNTT[index_value] = bit_flip(x_plain_nonNTT[index_value], bit_change);
            // Voy chequeando siempre que solo cambie 1 bit
          //  uint64_t hd_modified = hamming_distance(x_plain_nonNTT, x_nonNTT_original);
          //  if (hd_modified != 1)
          //      cout << "More than one bit! Bits modified " << hd_modified << endl;

           // else
           // {
                ntt_transformation(x_plain_nonNTT, ntt_tables, modulus_index, poly_modulus_degree);

                res_hamming = hamming_distance(x_plain_nonNTT, x_plain_original);
                res_norm2 = norm2(x_plain_nonNTT, x_plain_original);

                saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);
                saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
            //}
            x_plain_nonNTT = x_nonNTT_original;
        }
    }
   return 0;
}


