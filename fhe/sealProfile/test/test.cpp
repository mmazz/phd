#include <iostream>
#include <seal/ciphertext.h>
#include <system_error>
#include <vector>
#include "../src/examples.h"
#include "../src/utils_mati.h"
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include <memory>
#include <gtest/gtest.h>

#include <bitset>
#include <chrono>

using namespace seal;
using namespace std;
float threshold = 0.3;
double CURR_POINT = 1;
double MAX_VALUE = 2.;


bool TESTING = false;
bool RNS_ON = true;
bool NTT_ON = true;
double curr_point = CURR_POINT;
double max_value = MAX_VALUE;
size_t poly_modulus_degree = 4096;


TEST(functions, bit_flip)
{
    uint64_t target = 0;
    uint64_t expected = 1;
    uint64_t res = 1;
    ushort bit_to_change = 0;
    for (int i=0; i<10; i++)
    {
        res = bit_flip(target, bit_to_change);
        EXPECT_EQ(res, expected);
        expected = expected*2;
        bit_to_change++;
    }
}

TEST(functions, bit_flip_double)
{
    double target = 123;
    double res;
    std::vector<double> expected = {-123, 528280977408, 7.6875, 107, 123.0625,
                                    123.000244140625, 123.0000000000000142108547152020037174224853515625,
                                    123.000000000014551915228366851806640625};
    std::vector<int> bitToChange= {63, 57, 54, 50, 42, 34, 0, 10};

    int i = 0;
    for (int i=0; i<bitToChange.size(); i++)
    {
        res = bit_flip(target, bitToChange[i]);
        EXPECT_EQ(res, expected[i]);
    }
}
//inline float diff_elem(std::vector<double>  &v1, std::vector<double> &v2, float threshold, int size){
TEST(functions, diff_elem_OK)
{
    std::vector<double> v1 = {1.0, 2.0, 3.7, 4.1};
    std::vector<double> v2 = {1.05, 2.2, 3.5, 4};
    float threshold = 0.1;
    bool res = diff_elem(v1, v2, threshold);
    EXPECT_EQ(res, 1);
}

TEST(functions, diff_elem_WRONG)
{
    std::vector<double> v1 = {10.0, 2.0, 3.7, 4.1};
    std::vector<double> v2 = {1.05, 2.7, 2.9, 5};
    float threshold = 0.1;
    bool res = diff_elem(v1, v2, threshold);
    EXPECT_EQ(res, 0);
}

TEST(functions, check_equality)
{
    vector<int> modulus;
    modulus ={30, 30, 20};

    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(size_input);
    input_creator(input, poly_modulus_degree, curr_point, max_value);
    vector<double> input2;
    input2.reserve(size_input);
    input_creator(input2, poly_modulus_degree, curr_point-10, max_value+100);

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));
    SEALContext context(parms);


    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);

    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);

    Plaintext x_plain;
    encoder.encode(input, scale, x_plain);
    Plaintext x_plain2;
    encoder.encode(input2, scale, x_plain2);
    Ciphertext x_encrypted;
    Ciphertext x_encrypted2;
    // el .data() creo que es un puntero y entonces me puedo pasar....
    // pero dan igual, si hago .data(1) empiezo desde el segundo cifrado!!
    encryptor.encrypt(x_plain, x_encrypted);
    encryptor.encrypt(x_plain2, x_encrypted2);
    bool res = false;
    res = check_equality(x_encrypted, x_encrypted);
    EXPECT_EQ(res, true);
    res = check_equality(x_encrypted, x_encrypted2);
    EXPECT_EQ(res, false);

}




TEST(NTT, come_back_nonRNS)
{
    vector<int> modulus;
    modulus ={50, 20};

    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(size_input);
    input_creator(input, poly_modulus_degree, curr_point, max_value);

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));
    SEALContext context(parms);


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


    // x_plain size es un unico polinomio
    int x_plain_size =  coeff_modulus_size * poly_modulus_degree;
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

    bool res = 0;
    // chequeo que desencripta bien
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);

    // Transformo y destransformo.
    bool ntt_res = true;
    bool ntt_temp = false;
    x_encrypted = x_encrypted_original;
    util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0), ntt_tables[0]);
    ntt_transformation(x_encrypted, ntt_tables, 0, 0);
    ntt_temp = check_equality(x_encrypted, x_encrypted_original);
    ntt_res = ntt_res&ntt_temp;
    EXPECT_EQ(ntt_res, 1);
    // pruebo que sigo desencriptando bien
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);

    // lo mismo pero para el segundo polinomio
    ntt_res = true;
    ntt_temp = false;
    x_encrypted = x_encrypted_original;

    // start at c1, this is polydegree*K_rns_values
    util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1), ntt_tables[0]);
    ntt_transformation(x_encrypted, ntt_tables, 0, 1);
    ntt_temp = check_equality(x_encrypted, x_encrypted_original);
    ntt_res = ntt_res&ntt_temp;
    EXPECT_EQ(ntt_res, 1);
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);

}


TEST(NTT, come_back_RNS)
{
    vector<int> modulus;
    modulus ={30, 30, 20};

    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(size_input);
    input_creator(input, poly_modulus_degree, curr_point, max_value);

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));
    SEALContext context(parms);


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

    int x_plain_size =  coeff_modulus_size * poly_modulus_degree;
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

    bool res = 0;
    // chequeo que desencripta bien
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);

    // Transformo y destransformo.
    bool ntt_res = true;
    bool ntt_temp = false;
    x_encrypted = x_encrypted_original;
    for (size_t modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
    {
        util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0) + (modulus_index * poly_modulus_degree),
                                            ntt_tables[modulus_index]);
        ntt_transformation(x_encrypted, ntt_tables, modulus_index, 0);
        ntt_temp = check_equality(x_encrypted, x_encrypted_original);
        ntt_res = ntt_res&ntt_temp;
    }
    EXPECT_EQ(ntt_res, 1);
    // pruebo que sigo desencriptando bien
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);

    // lo mismo pero para el segundo polinomio
    ntt_res = true;
    ntt_temp = false;
    x_encrypted = x_encrypted_original;
    for (int modulus_index = 0; modulus_index < coeff_modulus_size; modulus_index++)
    {
        // start at c1, this is polydegree*K_rns_values
        util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1) + (modulus_index * poly_modulus_degree), ntt_tables[modulus_index]);
        ntt_transformation(x_encrypted, ntt_tables, modulus_index, 1);
        ntt_temp = check_equality(x_encrypted, x_encrypted_original);
        ntt_res = ntt_res&ntt_temp;
    }
    EXPECT_EQ(ntt_res, 1);
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);
}

TEST(NTT, same_encryption)
{
    vector<int> modulus;
    modulus ={50, 20};

    double scale = pow(2.0, 40);
    int coeff_modulus_size = modulus.size()-1;

    size_t size_input = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(size_input);
    input_creator(input, poly_modulus_degree, curr_point, max_value);

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, modulus));
    SEALContext context(parms);


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


    // x_plain size es un unico polinomio
    int x_plain_size =  coeff_modulus_size * poly_modulus_degree;
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

    bool res = 0;
    // chequeo que desencripta bien
    decryptor.decrypt(x_encrypted, plain_result);
    encoder.decode(plain_result, result);
    res = diff_elem(input, result, threshold);
    EXPECT_EQ(res, 1);

    // pruebo que solo estoy haciendo INTT a un polinomio
    x_encrypted = x_encrypted_original;
    util::inverse_ntt_negacyclic_harvey(x_encrypted.data(0), ntt_tables[0]);
    for(int i=int(x_plain_size); i<2*x_plain_size; i++)
        EXPECT_EQ(x_encrypted[i], x_encrypted_original[i]);

    x_encrypted = x_encrypted_original;
    util::inverse_ntt_negacyclic_harvey(x_encrypted.data(1), ntt_tables[0]);
    for(int i=0; i<int(x_plain_size); i++)
        EXPECT_EQ(x_encrypted[i], x_encrypted_original[i]);
}


int main(int argc, char * argv[])
{

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
