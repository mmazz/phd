#include "ciphertext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#include <string>
#define PROFILE
#define FIXED_SEED
#define DECRYPT_DETECTION
#include "iostream"
#include "openfhe.h"
#include "utils_mati.h"

/*
 * Modifico un bit en la codificacion y veo la norma 2 entre input/output, y veo
 * el HD entre la encriptacion original y la modificada.
 *
 *Primer input es el caso, el segundo es la cantidad de limbs
 *
 * Casos:
 *  0 - Estandard: Se fija la cantidad de limbs para RNS
 *  2 - Sin NTT: Se fija la cantidad de limbs para RNS
 *  1 - Sin RNS
 *  3 - Sin Ops
 */

using namespace lbcrypto;
int main(int argc, char *argv[])
{
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim = 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
    //    ScalingTechnique rescaleTech = FIXEDAUTO;
    //    ScalingTechnique rescaleTech = FLEXIBLEAUTO;

    bool nonNTT = false;
    bool execute = true;
    std::string extra;

    if (argc == 1)
    {
        std::cout << "Please select wich type of Optimizations you want" << std::endl;
        std::cout << "  1) 0 with optimizations " << std::endl;
        std::cout << "      i numbers of RNS limbs " << std::endl;
        std::cout << "  2) 1 without NTT " << std::endl;
        std::cout << "      i numbers of RNS limbs " << std::endl;
        std::cout << "  3) 2 without RNS " << std::endl;
        std::cout << "  4) 3 without Optimizations" << std::endl << std::endl;
        execute = false;
    }
    else if (argc > 1)
    {
        // Estandard
        if (atoi(argv[1]) == 0)
        {
            std::cout << std::endl << "Starting CKKS with Optimizations";
            if (argc > 2)
            {
                std::cout << " with multiple RNS limbs";
                multDepth = (uint32_t)atoi(argv[2]) - 1;
                extra = "_multiRNS_" + std::to_string(multDepth + 1);
            }
            std::cout << std::endl << std::endl;
        }

        // Sin NTT
        else if (atoi(argv[1]) == 1)
        {
            std::cout << std::endl << "Starting CKKS with no NTT";
            extra = "_nonNTT";
            nonNTT = true;
            if (argc > 2)
            {
                std::cout << " with multiple RNS limbs";
                multDepth = (uint32_t)atoi(argv[2]) - 1;
                extra = "_nonNTT_multiRNS_" + std::to_string(multDepth + 1);
            }
            std::cout << std::endl << std::endl;
        }
        // Sin RNS
        else if (atoi(argv[1]) == 2)
        {
            std::cout << std::endl << "Starting CKKS with no RNS" << std::endl << std::endl;
            multDepth = 0;
            firstModSize = 60;
            extra = "_nonRNS";
        }
        // Sin Ops
        else if (atoi(argv[1]) == 3)
        {
            std::cout << std::endl << "Starting CKKS with no Optimizations" << std::endl << std::endl;
            multDepth = 0;
            firstModSize = 60;
            nonNTT = true;
            extra = "_nonOps";
        }
        else
        {
            std::cout << "Please select wich type of Optimizations you want" << std::endl;
            std::cout << "  1) 0 with optimizations " << std::endl;
            std::cout << "      i numbers of RNS limbs " << std::endl;
            std::cout << "  2) 1 without NTT " << std::endl;
            std::cout << "      i numbers of RNS limbs " << std::endl;
            std::cout << "  3) 2 without RNS " << std::endl;
            std::cout << "  4) 3 without Optimizations" << std::endl << std::endl;
            execute = false;
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

    std::string dir_name = "./logs/log_encrypt/";
    std::string fileHD_C0_limnsNotChange = "catch_encryptHD" + extra + "_C0_limbsNotChanged";
    std::string fileHD_C0_limbChange = "catch_encryptHD" + extra + "_C0_limbChanged";

    std::string fileHD_C1_limnsNotChange = "catch_encryptHD" + extra + "_C1_limbsNotChanged";
    std::string fileHD_C1_limbChange = "catch_encryptHD" + extra + "_C1_limbChanged";
    std::string fileHD_positions = "catch_encryptHD" + extra + "_positions";
    std::string fileN2_bounded = "catch_encryptN2" + extra + "_bounded";

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    size_t dataSize = 28 * 28;
    std::ifstream file("data/example.txt");
    std::vector<double> input(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    input.erase(input.begin()); // pop front
    // padding of zeros
    for (size_t i = input.size(); i < batchSize; i++)
        input.push_back(0);

    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto c2 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElems = c1->GetElements();
    auto encryptElems_original = c2->GetElements();

    // Me haga un backup
    DCRTPoly plainElem_original = ptxt1->GetElement<DCRTPoly>();
    const auto elems_original = plainElem_original.GetAllElements();

    Plaintext result;
    std::vector<double> resultData(dataSize);
    std::vector<double> resultData_original(dataSize);

    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << " RNS size: " << RNS_size
              << std::endl;
    std::cout << "NTT condition: " << !nonNTT << ", RNS condition: " << multDepth << std::endl;
    // Chequeo que realmente estoy usando la cantidad de limbs de RNS que busco
    if (RNS_size == (size_t)multDepth + 1 && execute)
    {
        size_t test = 0;
        auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
        // Me fijo que a priori mantengo la misma encriptacion
        if (c1->GetElements()[0].GetAllElements()[0][0] != c2->GetElements()[0].GetAllElements()[0][0])
            test++;
        cc->Decrypt(keys.secretKey, c1, &result);
        result->SetLength(dataSize);
        resultData_original = result->GetRealPackedValue();
        size_t i = 0;
        // Chequeo que con los parametros que elegi puedo encriptar y desencriptar correctamente
        while (test == 0 && i < dataSize)
        {
            if (round(input[i]) != round(resultData_original[i]))
            {
                std::cout << "Error: " << input[i] << " != " << resultData_original[i] << std::endl;
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
            std::cout << "Total loops: " << 2*total_loops << std::endl;
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
                            try
                            {
                                cc->Decrypt(keys.secretKey, c1, &result);
                                result->SetLength(dataSize);
                                resultData = result->GetRealPackedValue();
                                std::cout<< "DONE! polinomial: "<< k << "  RNS limb: " << i << " coeff: " << j  << " bit: " << bit << std::endl;

                            }
                            catch(...)
                            {
                            }
                            // Cambian todos los coefficientes, tengo que resetearlo entero
                            if(nonNTT)
                                c1->GetElements()[k].GetAllElements() = original_vector;
                            // cambia solo uno
                            else
                                c1->GetElements()[k].GetAllElements()[i][j] = original_coeff;
                        }
                    }
                    count++;
                }
            }
        }
        // Si la prueba fue correcta sigo
        else
            std::cout << "Test fail" << std::endl;
    }
    else
        std::cout << "Condition of amount of RNS limbs fail!" << std::endl;
    return 0;
}
