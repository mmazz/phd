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
int main(int argc, char* argv[]) {
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;

    bool execute = true;
    std::string extra;

    if(argc == 1)
    {
        std::cout << "Please select wich type of Optimizations you want" << std::endl;
        std::cout << "  1) 0 with optimizations " << std::endl;
        std::cout << "      i numbers of RNS limbs " << std::endl;
        std::cout << "  2) 1 without RNS " << std::endl;
        execute = false;
    }
    else if(argc>1)
    {
        // Estandard
        if(atoi(argv[1])==0)
        {
            if(argc>2)
            {
                multDepth = (uint32_t)atoi(argv[2])-1;
                extra = "multiRNS_"+std::to_string(multDepth+1);
            }
        }

        // Sin RNS
        else if(atoi(argv[1])==1)
        {
            multDepth = 0;
            firstModSize = 60;
            extra = "nonRNS_";
        }

    }


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
    std::string fileHD_limbsNotChange = "inputHD_"+extra+"_limbsNotChanged";
    std::string fileHD_limbChange     = "inputHD_"+extra+"_limbChanged";
    std::string fileHD_encode_limbsNotChange = "inputHD_encode_"+extra+"_limbsNotChanged";
    std::string fileHD_encode_limbChange     = "inputHD_encode_"+extra+"_limbChanged";
    std::string fileHD_encode_positions      = "inputHD_encode_"+extra+"_positions";          // Enable the features that you wish to use
    std::string fileHD_positions      = "inputHD_"+extra+"_positions";          // Enable the features that you wish to use
    std::string fileN2_bounded        = "inputN2_"+extra+"_bounded";           cc->Enable(PKE);

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
    if (RNS_size == (size_t)multDepth+1 && execute)
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
            uint64_t count = 0;
            uint64_t HD_encode_limbsNotChanged = 0;
            uint64_t HD_encode_limbChanged = 0;
            uint64_t HD_limbsNotChanged = 0;
            uint64_t HD_limbChanged = 0;
            double N2_bounded = 0;
            bool new_file = 1;
            auto ringDim = cc->GetRingDimension();
            size_t bits_coeff = 64;

            int total_loops  = dataSize*bits_coeff;

            // Quiero guardarme un acumulador de cada posicion en bit de la encriptacion
            std::vector<uint64_t> encryption_bitChange(2*RNS_size*bits_coeff*ringDim, 0);
            std::vector<uint64_t> encode_bitChange(RNS_size*bits_coeff*ringDim, 0);

            saveDataLog(dir_name+fileHD_encode_limbsNotChange, HD_encode_limbsNotChanged,  new_file, RNS_size);
            saveDataLog(dir_name+fileHD_encode_limbChange, HD_encode_limbChanged, new_file, RNS_size);
            saveDataLog(dir_name+fileHD_limbsNotChange, HD_limbsNotChanged,  new_file, RNS_size);
            saveDataLog(dir_name+fileHD_limbChange, HD_limbChanged, new_file, RNS_size);
            saveDataLog(dir_name+fileN2_bounded, N2_bounded,  new_file, RNS_size);



            std::cout << "Total loops: " << total_loops << std::endl;
            for (size_t i = 0; i < dataSize; i++)
            {
                std::cout << count << ", " << std::flush;
                auto original = input[i];
                for(size_t bit=0; bit<bits_coeff; bit++)
                {
                    // modifico el input y codifico
                    input[i] = bit_flip(original, bit);
                    ptxt1 = cc->MakeCKKSPackedPlaintext(input);
                    plainElem = ptxt1->GetElement<DCRTPoly>();
                    auto elems =  plainElem.GetAllElements();
                    // calculo el HD entre las codificaciones
                    auto [HD_encode_limbsNotChanged, HD_encode_limbsChanged] = hamming_distance_RNS(plainElem, plainElem_original, RNS_size, i);
                    saveDataLog(dir_name+fileHD_encode_limbsNotChange, HD_encode_limbsNotChanged,  !new_file, RNS_size);
                    saveDataLog(dir_name+fileHD_encode_limbChange, HD_encode_limbChanged, !new_file, RNS_size);
                    hamming_distance_position(encode_bitChange, plainElem, plainElem_original, RNS_size, bits_coeff);
                    // encritpo y calculo el HD
                    c1 = cc->Encrypt(keys.publicKey, ptxt1);
                    encryptElem = c1->GetElements();
                    auto [HD_limbsNotChanged, HD_limbsChanged, HD_limbsNotChanged1, HD_limbsChanged1] = hamming_distance_RNS(encryptElem, encryptElem_original,  RNS_size, i);

                    saveDataLog(dir_name+fileHD_limbsNotChange, HD_limbsNotChanged,  !new_file, RNS_size);
                    saveDataLog(dir_name+fileHD_limbChange, HD_limbChanged, !new_file, RNS_size);
                    hamming_distance_position(encryption_bitChange, encryptElem, encryptElem_original, RNS_size, bits_coeff);
                    // desencripto y calculo HD y N2 de input/output
                    cc->Decrypt(keys.secretKey, c1, &result);
                    result->SetLength(dataSize);
                    resultData = result->GetRealPackedValue();
                   // res_norm2 = norm2(input, resultData, dataSize);
                    N2_bounded = norm2_bounded(input, resultData, dataSize, max_diff);
                    saveDataLog(dir_name+fileN2_bounded, N2_bounded, !new_file, RNS_size);
                   // saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
                    input[i] = original;
                }
            }

            std::fstream logFile_encode;
            logFile_encode.open("/home/mmazz/phd/fhe/bitFlip-openfhe/logs/log_encode/"+fileHD_encode_positions+".txt", std::ios::out);
            for(size_t i=0; i<encode_bitChange.size(); i++)
                logFile_encode << encode_bitChange[i] << ", ";
            logFile_encode.close();
            std::fstream logFile;
            logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/logs/log_encode/"+fileHD_positions+".txt", std::ios::out);
            for(size_t i=0; i<encryption_bitChange.size(); i++)
                logFile << encryption_bitChange[i] << ", ";
            logFile.close();
        }
    }
    return 0;
}
