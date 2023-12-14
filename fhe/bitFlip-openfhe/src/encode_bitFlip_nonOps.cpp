#include "ciphertext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#define PROFILE
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"
#include "utils_mati.h"

using namespace lbcrypto;
int main() {
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 0;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 60;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
//    ScalingTechnique rescaleTech = FIXEDAUTO;
//    ScalingTechnique rescaleTech = FLEXIBLEAUTO;
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
    std::string file_name_hd = "encodeHD_nonOps";
   // std::string file_name_norm2 =  "encodeN2_nonOps";
    std::string file_name_norm2_bounded =  "encodeN2_nonOps_bounded";

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
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
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    //DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    // Me haga un backup
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
         //   double res_norm2 = 0;
            double res_norm2_bounded = 0;
            bool new_file = 1;

            saveDataLog(dir_name+file_name_hd, res_hamming,  new_file);
           // saveDataLog(dir_name+file_name_norm2, res_norm2,  new_file);
            saveDataLog(dir_name+file_name_norm2_bounded, res_norm2_bounded,  new_file);
            // Me quedo con la componente cero por que aca no tengo RNS y es el unico
            auto original_Coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[0];
            for (size_t j = 0; j < cc->GetRingDimension(); j++)
            {
                std::cout << j << std::endl;
                for(size_t bit=0; bit<64; bit++)
                {
                    // Paso la codificacion al formato de coeficientes (INTT)
                    ptxt1->GetElement<DCRTPoly>().SwitchFormat();
                    // Me quedo con un coefficiente. Tengo un backup de todos ya que cambian todos aca
                    auto original = ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][j];
                    ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][j] = bit_flip(original, bit);
                    // Vuelvo al espacio de evaluacion (NTT)
                    ptxt1->GetElement<DCRTPoly>().SwitchFormat();
                    c1 = cc->Encrypt(keys.publicKey, ptxt1);
                    encryptElem = c1->GetElements();
                    res_hamming = hamming_distance(input, resultData, dataSize);
                    saveDataLog(dir_name+file_name_hd, res_hamming, !new_file);

                    cc->Decrypt(keys.secretKey, c1, &result);
                    result->SetLength(dataSize);
                    resultData = result->GetRealPackedValue();
                  //  res_norm2 = norm2(input, resultData, dataSize);
                    res_norm2_bounded = norm2_bounded(input, resultData, dataSize, max_diff);

                   // saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
                    saveDataLog(dir_name+file_name_norm2_bounded, res_norm2_bounded, !new_file);
                    // Cambian todos los coefficientes, tengo que resetearlo entero
                    ptxt1->GetElement<DCRTPoly>().GetAllElements()[0] = original_Coeff;

                }
            }

        }
    }
    return 0;
}
