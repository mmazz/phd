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
    uint32_t multDepth = 4;
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

    std::string dir_name = "logs/log_encode/";
    std::string file_name_hd = "encodeHD_multiRNS_limbsNotChanged_30bits";
    std::string file_name_hd_RNS = "encodeHD_multiRNS_limbChanged_30bits";
    std::string file_name_hd_positions = "encodeHD_multiRNS_positions_30bits";
    //std::string file_name_norm2 =  "encodeN2";
    std::string file_name_norm2_bounded =  "encodeN2_multiRNS_bounded_30bits";

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
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElem = c1->GetElements();
    DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

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
    // Chequeo que realmente estoy usando la cantidad de limbs de RNS que busco
    if (RNS_size == 5)
    {
        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
        cc->Decrypt(keys.secretKey, c1, &result);
        result->SetLength(dataSize);
        std::cout << result << std::endl;
        resultData = result->GetRealPackedValue();
        size_t test = 0;
        size_t i = 0;
        // Chequeo que con los parametros que elegi puedo encriptar y desencriptar correctamente
        while(test==0 && i < dataSize)
        {
            if(round(input[i])!=round(resultData[i]))
            {
                std::cout << "Error: " <<  input[i] << " != " << resultData[i] << std::endl;
                test++;
            }
            i++;
        }

        // Si la prueba fue correcta sigo
        if(test==0)
        {
            uint64_t HD_RNS_limbsNotChanged = 0;
            uint64_t HD_RNS_limbChanged = 0;
            int count = 0;
            //double res_norm2 = 0;
            double N2_bounded = 0;
            bool new_file = 1;
            auto ringDim = cc->GetRingDimension();
            size_t bits_coeff = 64;
            int total_loops  = RNS_size*ringDim*bits_coeff;
            std::vector<uint64_t> res(2*RNS_size*bits_coeff*ringDim, 0);
            saveDataLog(dir_name+file_name_hd, HD_RNS_limbsNotChanged,  new_file, RNS_size);
            saveDataLog(dir_name+file_name_hd_RNS, HD_RNS_limbChanged, new_file, RNS_size);
            //saveDataLog(dir_name+file_name_norm2, res_norm2,  new_file);
            saveDataLog(dir_name+file_name_norm2_bounded, N2_bounded,  new_file, RNS_size);

            for (size_t i = 0; i < RNS_size; i++)
            {

                for (size_t j = 0; j < ringDim; j++)
                {
                    std::cout << count<<" : " << total_loops << std::endl;
                    auto original = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                    for(size_t bit=0; bit<bits_coeff; bit++)
                    {
                        // Cambio un bit de la codificacion
                        ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = bit_flip(original, bit);
                        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
                        encryptElem = c1->GetElements();
                        // Le caluclo el HD a la encriptacion

                        auto [HD_RNS_limbsNotChanged, HD_RNS_limbsChanged] = hamming_distance_RNS(encryptElem, encryptElem_original, RNS_size, i);
                        hamming_distance_position(res, encryptElem, encryptElem_original, RNS_size, 64);
                        saveDataLog(dir_name+file_name_hd, HD_RNS_limbsNotChanged, !new_file, RNS_size);
                        saveDataLog(dir_name+file_name_hd_RNS, HD_RNS_limbsChanged, !new_file, RNS_size);

                        cc->Decrypt(keys.secretKey, c1, &result);
                        result->SetLength(dataSize);
                        resultData = result->GetRealPackedValue();
                        // Calculo la norma 2 del input/output
                       // res_norm2 = norm2(input, resultData, dataSize);
                        N2_bounded = norm2_bounded(input, resultData, dataSize, max_diff);

                       // saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
                        saveDataLog(dir_name+file_name_norm2_bounded, N2_bounded, !new_file, RNS_size);

                        ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = original;

                    }
                    count++;
                }
            }
            for(size_t i=0; i<res.size(); i++)
                std::cout << res[i] << ", ";
            std::fstream logFile;
            logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/logs/log_encode/"+file_name_hd_positions+".txt", std::ios::out);
            for(size_t i=0; i<res.size(); i++)
                logFile << res[i] << ", ";
            logFile.close();
        }
    }
    return 0;
}
