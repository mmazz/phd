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
    std::string file_name_hd = "encodeHD_limbsNotChanged";
    std::string file_name_hd_RNS = "encodeHD_limbChanged";
    //std::string file_name_norm2 =  "encodeN2";
    std::string file_name_norm2_bounded =  "encodeN2_bounded";

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
    if (RNS_size == 1)
    {
        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
        cc->Decrypt(keys.secretKey, c1, &result);
        result->SetLength(dataSize);
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


            ptxt1->GetElement<DCRTPoly>().SwitchFormat();
            ptxt_original->GetElement<DCRTPoly>().SwitchFormat();
            auto original = ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][9];
            auto original2 = ptxt_original->GetElement<DCRTPoly>().GetAllElements()[0][9];
            // Cambio un bit de la codificacion
            ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][9] = bit_flip(original, 63) % ptxt1->GetElementModulus();
            std::cout << original << " " << original2 << std::endl;
            ptxt_original->GetElement<DCRTPoly>().GetAllElements()[0][9] = bit_flip(original2, 63);
            std::cout << bit_flip(original, 63) << " mod " <<  ptxt1->GetElementModulus()<< " = " << bit_flip(original, 63) % ptxt1->GetElementModulus() << std::endl;
            std::cout <<  bit_flip(original2, 63)<< std::endl;
            ptxt1->GetElement<DCRTPoly>().SwitchFormat();
            ptxt_original->GetElement<DCRTPoly>().SwitchFormat();
            auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
            auto c2 = cc->Encrypt(keys.publicKey, ptxt_original);
            encryptElem = c1->GetElements();
            encryptElem_original = c2->GetElements();

            uint64_t HD_RNS_limbChanged = 0;
            // Podria agregarlo directamente al HD comun y que compute ambas
            HD_RNS_limbChanged = hamming_distance(encryptElem, encryptElem_original, RNS_size);

            std::cout << HD_RNS_limbChanged << std::endl;


        }

    }
    return 0;
}
