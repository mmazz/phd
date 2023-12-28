#include "ciphertext-fwd.h"
#include "encoding/plaintext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <cstdint>
#include <string>
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"
#include "../src/utils_mati.h"
#include <gtest/gtest.h>

#include <bitset>
#include <chrono>

using namespace lbcrypto;
using namespace std;

TEST(HD, encryption_difference)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto c2 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElems = c1->GetElements();
    auto encryptElems_original= c2->GetElements();

    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    for(size_t k=0; k<2; k++)
    {
        for (size_t i = 0; i < RNS_size; i++)
        {
            for (size_t j = 0; j < ringDim; j++)
            {
                EXPECT_EQ(encryptElems[k].GetAllElements()[i][j], encryptElems_original[k].GetAllElements()[i][j]);
            }
        }
    }
}

TEST(HD, encode_rns_bitflip_limbNotChange)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto c2 = c1->Clone();
    auto encryptElems = c1->GetElements();
    auto encryptElems_original= c2->GetElements();

    auto coeff0 = encryptElems_original[0].GetAllElements()[0][0];
    EXPECT_EQ(coeff0, encryptElems[0].GetAllElements()[0][0]);
    ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0] = bit_flip(ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0], 29);
    c1 = cc->Encrypt(keys.publicKey, ptxt1);
    encryptElems = c1->GetElements();

    std::cout << "start" << std::endl;
    EXPECT_EQ(coeff0, encryptElems_original[0].GetAllElements()[0][0]);
    EXPECT_NE(coeff0, encryptElems[0].GetAllElements()[0][0] );
 //   // que si cambio algo del limb cero, la encriptacion tenga de limb 0 distinto y el 1 igual
    for(size_t k=0; k<2; k++)
    {
        for (size_t j = 0; j < ringDim; j++)
        {
            EXPECT_EQ(encryptElems[k].GetAllElements()[0][j], encryptElems_original[k].GetAllElements()[0][j]);
        }
    }


}
TEST(HD, encode_rns_bitflip_limbsChanged)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto c2 = c1->Clone();
    auto encryptElems = c1->GetElements();
    auto encryptElems_original= c2->GetElements();

    auto coeff0 = encryptElems_original[0].GetAllElements()[0][0];
    EXPECT_EQ(coeff0, encryptElems[0].GetAllElements()[0][0]);
    ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0] = bit_flip(ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0], 29);
    c1 = cc->Encrypt(keys.publicKey, ptxt1);
    encryptElems = c1->GetElements();

    std::cout << "start" << std::endl;
    EXPECT_EQ(coeff0, encryptElems_original[0].GetAllElements()[0][0]);
    EXPECT_NE(coeff0, encryptElems[0].GetAllElements()[0][0] );
 //   // que si cambio algo del limb cero, la encriptacion tenga de limb 0 distinto y el 1 igual

    for(size_t k=0; k<2; k++)
    {
        for (size_t j = 0; j < ringDim; j++)
        {
            EXPECT_EQ(encryptElems[k].GetAllElements()[1][j], encryptElems_original[k].GetAllElements()[1][j]);
        }
    }

}


TEST(HD, encryption_deepCopy)
{

    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto c2 = c1->Clone();
    auto encryptElems = c1->GetElements();
    auto encryptElems_original= c2->GetElements();
    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    for(size_t k=0; k<2; k++)
    {
        for (size_t i = 0; i < RNS_size; i++)
        {
            for (size_t j = 0; j < ringDim; j++)
            {
                EXPECT_EQ(encryptElems[k].GetAllElements()[i][j], encryptElems_original[k].GetAllElements()[i][j]);
            }
        }
    }
    encryptElems[0].GetAllElements()[0][0] = (Integer)0;
    EXPECT_EQ(encryptElems[0].GetAllElements()[0][0], 0);
    EXPECT_NE(encryptElems_original[0].GetAllElements()[0][0], 0);
}

TEST(HD, encryption)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    //DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    // Me haga un backup
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);
    auto c1_original = cc->Encrypt(keys.publicKey, ptxt_original);
    auto encryptElem_original = c1_original->GetElements();
    encryptElem_original = encryptElem;
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();

    auto [HD_RNS_limbsNotChanged, HD_RNS_limbChanged] = hamming_distance_RNS(encryptElem, encryptElem_original, RNS_size, 0);
    EXPECT_EQ(HD_RNS_limbsNotChanged, 0);
    EXPECT_EQ(HD_RNS_limbChanged, 0);
    auto [HD_RNS_limbsNotChanged2, HD_RNS_limbChanged2] = hamming_distance_RNS(encryptElem, encryptElem_original, RNS_size, 1);
    EXPECT_EQ(HD_RNS_limbsNotChanged2, 0);
    EXPECT_EQ(HD_RNS_limbChanged2, 0);

}



TEST(HD, same_encoding)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt = cc->MakeCKKSPackedPlaintext(input);
    Plaintext ptxt_original = ptxt;
    DCRTPoly plainElem = ptxt->GetElement<DCRTPoly>();
    auto elems =  plainElem.GetAllElements();
    auto c1 = cc->Encrypt(keys.publicKey, ptxt);
    auto encryptElem = c1->GetElements();

    // Me haga un backup
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    auto elem0 = elems[0][0];
    elems[0][0] = 0;
    EXPECT_EQ(elems[0][0], 0);
    EXPECT_NE(elems[0][0], elems_original[0][0]);

    elems[0][0] = elem0;
    size_t RNS_size = ptxt->GetElement<DCRTPoly>().GetAllElements().size();
    auto [HD_RNS_limbsNotChanged, HD_RNS_limbChanged] = hamming_distance_RNS(plainElem, plainElem_original, RNS_size, 0);
    EXPECT_EQ(HD_RNS_limbsNotChanged, 0);
    EXPECT_EQ(HD_RNS_limbChanged, 0);
    auto [HD_RNS_limbsNotChanged1, HD_RNS_limbChanged1] = hamming_distance_RNS(plainElem, plainElem_original, RNS_size, 0);
    EXPECT_EQ(HD_RNS_limbsNotChanged1, 0);
    EXPECT_EQ(HD_RNS_limbChanged1, 0);
    auto [HD_RNS_limbsNotChanged3, HD_RNS_limbChanged3] = hamming_distance_RNS(plainElem, plainElem_original, RNS_size, 1);
    EXPECT_EQ(HD_RNS_limbsNotChanged3, 0);
    EXPECT_EQ(HD_RNS_limbChanged3, 0);
    auto [HD_RNS_limbsNotChanged2, HD_RNS_limbChanged2] = hamming_distance_RNS(plainElem, plainElem_original, RNS_size, 1);
    EXPECT_EQ(HD_RNS_limbsNotChanged2, 0);
    EXPECT_EQ(HD_RNS_limbChanged2, 0);
}

TEST(DISABLED_HD, same_encryption)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    //DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    // Me haga un backup
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);
    auto c1_original = cc->Encrypt(keys.publicKey, ptxt_original);
    auto encryptElem_original = c1_original->GetElements();
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();

    c1 = cc->Encrypt(keys.publicKey, ptxt1);
    encryptElem = c1->GetElements();
    auto [HD_RNS_limbsNotChanged0, HD_RNS_limbChanged0] = hamming_distance_RNS(encryptElem, encryptElem, RNS_size, 0);
    EXPECT_EQ(HD_RNS_limbsNotChanged0, 0);
    EXPECT_EQ(HD_RNS_limbChanged0, 0);
    auto [HD_RNS_limbsNotChanged, HD_RNS_limbChanged] = hamming_distance_RNS(encryptElem, encryptElem, RNS_size, 1);
    EXPECT_EQ(HD_RNS_limbsNotChanged, 0);
    EXPECT_EQ(HD_RNS_limbChanged, 0);
}


TEST(DISABLED_HD, encode)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    //DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    // Me haga un backup
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);
    auto c1_original = cc->Encrypt(keys.publicKey, ptxt_original);
    auto encryptElem_original = c1_original->GetElements();
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    size_t bits_coeff = 64;
    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    auto original_vector = ptxt1->GetElement<DCRTPoly>().GetAllElements();
    for (size_t i = 0; i < RNS_size; i++)
    {
        std::cout << i << std::endl;
        for (size_t j = 0; j < ringDim; j+=3)
        {
            auto original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
            for(size_t bit=0; bit<bits_coeff; bit+=7)
            {
                original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = bit_flip(original_coeff, bit);
                c1 = cc->Encrypt(keys.publicKey, ptxt1);
                encryptElem = c1->GetElements();
                auto [HD_RNS_limbsNotChanged, HD_RNS_limbsChanged] = hamming_distance_RNS(encryptElem, encryptElem_original, RNS_size, i);
                EXPECT_EQ(HD_RNS_limbsNotChanged, 0);
                ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = original_coeff;
            }
        }
    }

}

TEST(DISABLE_functions, bit_flip)
{
    uint64_t target = 0;
    uint64_t expected = 1;
    uint64_t res = 1;
    size_t bit_to_change = 0;
    for (int i=0; i<10; i++)
    {
        res = bit_flip(target, bit_to_change);
        EXPECT_EQ(res, expected);
        expected = expected*2;
        bit_to_change++;
    }
}

TEST(DISABLED_invariant, encode)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    //DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    // Me haga un backup
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);
    auto c1_original = cc->Encrypt(keys.publicKey, ptxt_original);
    auto encryptElem_original = c1_original->GetElements();
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    size_t bits_coeff = 64;
    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    auto original_vector = ptxt1->GetElement<DCRTPoly>().GetAllElements();
    for (size_t i = 0; i < RNS_size; i++)
    {
        std::cout << i << std::endl;
        for (size_t j = 0; j < ringDim; j+=3)
        {
            auto original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
            for(size_t bit=0; bit<bits_coeff; bit+=7)
            {
                original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = bit_flip(original_coeff, bit);
                c1 = cc->Encrypt(keys.publicKey, ptxt1);
                ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = original_coeff;
            }
        }
    }
    int count_diff = 0;
    for (size_t i = 0; i < RNS_size; i++)
    {
        for (size_t j = 0; j < ringDim; j++)
        {
            if(ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] != ptxt_original->GetElement<DCRTPoly>().GetAllElements()[i][j])
                count_diff++;
        }
    }
    EXPECT_EQ(count_diff, 0);
}

TEST(DISABLED_invariant, encode_nonNTT)
{
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetFirstModSize(firstModSize);
    parameters.SetBatchSize(batchSize);
    parameters.SetRingDim(ringDim);
    parameters.SetScalingTechnique(rescaleTech);
    parameters.SetSecurityLevel(HEStd_NotSet);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    //DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    // Me haga un backup
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);
    auto c1_original = cc->Encrypt(keys.publicKey, ptxt_original);
    auto encryptElem_original = c1_original->GetElements();
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    size_t bits_coeff = 64;
    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    auto original_vector = ptxt1->GetElement<DCRTPoly>().GetAllElements();
    for (size_t i = 0; i < RNS_size; i++)
    {
        std::cout << i << std::endl;
        for (size_t j = 0; j < ringDim; j+=3)
        {
            auto original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
            for(size_t bit=0; bit<bits_coeff; bit+=7)
            {
                ptxt1->GetElement<DCRTPoly>().SwitchFormat();
                original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = bit_flip(original_coeff, bit);
                ptxt1->GetElement<DCRTPoly>().SwitchFormat();
                c1 = cc->Encrypt(keys.publicKey, ptxt1);
                ptxt1->GetElement<DCRTPoly>().GetAllElements() = original_vector;
            }
        }
    }
    int count_diff = 0;
    for (size_t i = 0; i < RNS_size; i++)
    {
        for (size_t j = 0; j < ringDim; j++)
        {
            if(ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] != ptxt_original->GetElement<DCRTPoly>().GetAllElements()[i][j])
                count_diff++;
        }
    }
    EXPECT_EQ(count_diff, 0);
}


int main(int argc, char * argv[])
{

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
