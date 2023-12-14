#include "ciphertext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#define PROFILE
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"
#include "utils_mati.h"

/* Modifico un bit del input y comparo cuanto cambio la codificacion
 *
 */

using namespace lbcrypto;
int main() {
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 0;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 60;
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

    std::string dir_name = "logs/log_input/";
    std::string file_name_hd = "inputHD_nonRNS";
    std::string file_name_hd_encrypt = "inputHD_encrypt_nonRNS";
   // std::string file_name_norm2 =  "inputN2_nonRNS";
    std::string file_name_norm2_bounded =  "inputN2_nonRNS_bounded";

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    int max_diff = 255;
    size_t dataSize = 28*28;
    std::ifstream file2("data/example.txt");
    std::vector<uint8_t> input_8(std::istream_iterator<uint8_t>{file2}, std::istream_iterator<uint8_t>{});
    input_8.erase(input_8.begin()); // pop front

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
    DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);

    auto c1_original = cc->Encrypt(keys.publicKey, ptxt_original);
    auto encryptElem_original = c1_original->GetElements();
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();
    Plaintext result;
    std::vector<double> resultData(dataSize);
    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();

    std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << " RNS size: " << RNS_size << std::endl << std::endl;
    if (RNS_size == 1)
    {
        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
        cc->Decrypt(keys.secretKey, c1, &result);
        result->SetLength(dataSize);
        std::cout << result << std::endl;
        resultData = result->GetRealPackedValue();
        size_t test = 0;
        size_t i = 0;
        while(test==0 && i < dataSize)
        {
            if(round(input[i])!=round(resultData[i]))
            {
                std::cout << "Error: " <<  input[i] << " != " << resultData[i] << std::endl;
                test++;
            }
            i++;
        }
        if(test==0)
        {
            uint64_t res_hamming = 0;
            uint64_t res_hamming_encrypt = 0;
            double res_norm2_bounded = 0;
            bool new_file = 1;

            saveDataLog(dir_name+file_name_hd, res_hamming,  new_file);
            saveDataLog(dir_name+file_name_hd_encrypt, res_hamming_encrypt,  new_file);
            saveDataLog(dir_name+file_name_norm2_bounded, res_norm2_bounded,  new_file);


            for (size_t i = 0; i < dataSize; i++)
            {
                std::cout << i << std::endl;
                auto original = input[i];
                for(size_t bit=0; bit<64; bit++)
                {
                    // modifico el input y codifico
                    input[i] = bit_flip(original, bit);
                    ptxt1 = cc->MakeCKKSPackedPlaintext(input);
                    plainElem = ptxt1->GetElement<DCRTPoly>();
                    auto elems =  plainElem.GetAllElements();
                    // calculo el HD entre las codificaciones
                    res_hamming = hamming_distance(plainElem, plainElem_original, RNS_size);
                    saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);

                    // encritpo y calculo el HD
                    c1 = cc->Encrypt(keys.publicKey, ptxt1);
                    encryptElem = c1->GetElements();
                    res_hamming_encrypt = hamming_distance(encryptElem, encryptElem_original, RNS_size);
                    saveDataLog(dir_name+file_name_hd_encrypt, res_hamming_encrypt,  !new_file);

                    // desencripto y calculo HD y N2 de input/output
                    cc->Decrypt(keys.secretKey, c1, &result);
                    result->SetLength(dataSize);
                    resultData = result->GetRealPackedValue();
                    res_norm2_bounded = norm2_bounded(input, resultData, dataSize, max_diff);
                    saveDataLog(dir_name+file_name_norm2_bounded, res_norm2_bounded, !new_file);

                    input[i] = original;
                }
            }
        }
    }
    return 0;
}
