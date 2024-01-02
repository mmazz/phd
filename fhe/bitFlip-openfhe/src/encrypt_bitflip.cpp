#include "ciphertext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#define PROFILE
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"
#include "utils_mati.h"

using namespace lbcrypto;

int main(int argc, char* argv[]) {
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;

    bool nonNTT = false;
    bool execute = true;
    std::string extra;

    if(argc == 1)
    {
        std::cout << "Please select wich type of Optimizations you want" << std::endl;
        std::cout << "  1) 0 with optimizations " << std::endl;
        std::cout << "      i numbers of RNS limbs " << std::endl;
        std::cout << "  2) 1 without NTT " << std::endl;
        std::cout << "      i numbers of RNS limbs " << std::endl;
        std::cout << "  3) 2 without RNS " << std::endl;
        std::cout << "  4) 2 without Optimizations" << std::endl << std::endl;
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

        // Sin NTT
        else if(atoi(argv[1])==1)
        {
            extra = "nonNTT";
            nonNTT = true;
            if(argc>2)
            {
                multDepth = (uint32_t)atoi(argv[2])-1;
                extra = "nonNTT_multiRNS_"+std::to_string(multDepth+1);
            }
        }
        // Sin RNS
        else if(atoi(argv[1])==2)
        {
            multDepth = 0;
            firstModSize = 60;
            extra = "nonRNS_";
        }
        // Sin Ops
        else if(atoi(argv[1])==3)
        {
            multDepth = 0;
            firstModSize = 60;
            nonNTT = true;
            extra = "nonOps_";
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

    std::string dir_name = "logs/log_encrypt/";
    std::string fileN2_bounded =  "encryptN2_bounded";

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    int max_diff = 255;
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
    if (RNS_size == (size_t)multDepth+1 && execute)
    {
        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
        cc->Decrypt(keys.secretKey, c1, &result);
        result->SetLength(dataSize);
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
            double N2_bounded = 0;
            bool new_file = 1;
            size_t bits_coeff = 64;
            auto ringDim = cc->GetRingDimension();

            saveDataLog(dir_name+fileN2_bounded, N2_bounded,  new_file, RNS_size);
            auto c1 = cc->Encrypt(keys.publicKey, ptxt1);

            int total_loops  = 2*RNS_size*ringDim*bits_coeff;
            std::cout << "Total loops: " << total_loops << std::endl;
            // k es el polinomio c0 o c1
            for (size_t k=0; k<2; k++)
            {
                auto original_vector = c1->GetElements()[k].GetAllElements();
                for (size_t i = 0; i < RNS_size; i++)
                {
                    for (size_t j = 0; j < ringDim; j++)
                    {
                        std::cout << count << ", " << std::flush;
                        auto original_coeff = c1->GetElements()[k].GetAllElements()[i][j];
                        for(size_t bit=0; bit<bits_coeff; bit++)
                        {
                            if(nonNTT)
                            {
                                // Paso la codificacion al formato de coeficientes (INTT)
                                c1->GetElements()[k].SwitchFormat();
                                // Me quedo con un coefficiente. Tengo un backup de todos ya que cambian todos aca
                                original_coeff = c1->GetElements()[k].GetAllElements()[i][j];
                            }
                            //std::cout << c1->GetElements() << std::endl;
                            c1->GetElements()[k].GetAllElements()[i][j] = bit_flip(original_coeff, bit);
                            if(nonNTT)
                                c1->GetElements()[k].SwitchFormat();
                            cc->Decrypt(keys.secretKey, c1, &result);
                            result->SetLength(dataSize);
                            resultData = result->GetRealPackedValue();

                            N2_bounded = norm2_bounded(input, resultData, dataSize, max_diff);
                            saveDataLog(dir_name+fileN2_bounded, N2_bounded, !new_file, RNS_size);
                            // Cambian todos los coefficientes, tengo que resetearlo entero
                            if(nonNTT)
                                c1->GetElements()[k].GetAllElements() = original_vector;
                            // cambia solo uno
                            else
                                c1->GetElements()[k].GetAllElements()[i][j] = original_coeff;
                        }
                    }
                }
                count++;
            }
        }
    }
    return 0;
}
