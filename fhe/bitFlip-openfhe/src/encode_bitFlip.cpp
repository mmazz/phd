#include "ciphertext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#define PROFILE
#include "openfhe.h"
#include "iostream"
#include "utils_mati.h"

using namespace lbcrypto;
int main() {
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 50;
    uint32_t firstModSize = 60;
    uint32_t batchSize = 1<<10;
    uint32_t ringDim= 1<<11;
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

    std::string dir_name = "logs/log_encode/";
    std::string file_name_hd = "encodeHD_withRNS&NTT";
    std::string file_name_norm2 =  "encodeN2_withRNS&NTT";


    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    size_t dataSize = 28*28;
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front

    // padding of zeros
    for (size_t i=input.size(); i<batchSize; i++)
        input.push_back(0);
    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();
    Plaintext result;
    std::vector<double> resultData(dataSize);
    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();

    std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << " RNS size: " << RNS_size << std::endl << std::endl;
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    cc->Decrypt(keys.secretKey, c1, &result);
    result->SetLength(dataSize);
    std::cout << result << std::endl;
    resultData = result->GetRealPackedValue();
    size_t test = 0;
    for (size_t i=0; i<dataSize; i++)
    {
        if(round(input[i])!=round(resultData[i]))
        {
            std::cout << "Error: " <<  input[i] << " != " << resultData[i] << std::endl;
            test++;
        }
    }

    if(test==0)
    {
        uint64_t res_hamming = 0;
        double res_norm2 = 0;

        bool new_file = 1;

        saveDataLog(dir_name+file_name_hd, res_hamming,  new_file);
        saveDataLog(dir_name+file_name_norm2, res_norm2,  new_file);
        for (size_t i = 0; i < RNS_size; i++)
        {
            for (size_t j = 0; j < cc->GetRingDimension(); j++)
            {
                std::cout << j << std::endl;
                auto original = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                for(size_t bit=0; bit<64; bit++)
                {
                    ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = bit_flip(original, bit);
                    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
                    cc->Decrypt(keys.secretKey, c1, &result);
                    result->SetLength(dataSize);
                    resultData = result->GetRealPackedValue();
                    res_hamming = hamming_distance(input, resultData, dataSize);
                    res_norm2 = norm2(input, resultData, dataSize);

                    saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);
                    saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
                    ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = original;

                }
            }
        }
    }
    return 0;
}
