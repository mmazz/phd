#include "ciphertext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#include <string>
#define PROFILE
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"
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
int main(int argc, char* argv[]) {
    // Step 1: Setup CryptoContext
    uint32_t multDepth = 1;
    uint32_t scaleModSize = 30;
    uint32_t firstModSize = 30;
    uint32_t batchSize = 1024;
    uint32_t ringDim= 2048;
    ScalingTechnique rescaleTech = FIXEDMANUAL;
//    ScalingTechnique rescaleTech = FIXEDAUTO;
//    ScalingTechnique rescaleTech = FLEXIBLEAUTO;

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
            std::cout << std::endl << "Starting CKKS with Optimizations";
            if(argc>2)
            {
                std::cout<< " with multiple RNS limbs";
                multDepth = (uint32_t)atoi(argv[2])-1;
                extra = "_multiRNS_"+std::to_string(multDepth+1);
            }
            std::cout<< std::endl << std::endl;
        }

        // Sin NTT
        else if(atoi(argv[1])==1)
        {
            std::cout << std::endl << "Starting CKKS with no NTT";
            extra = "_nonNTT";
            nonNTT = true;
            if(argc>2)
            {
                std::cout<< " with multiple RNS limbs";
                multDepth = (uint32_t)atoi(argv[2])-1;
                extra = "_nonNTT_multiRNS_"+std::to_string(multDepth+1);
            }
            std::cout << std::endl<< std::endl;
        }
        // Sin RNS
        else if(atoi(argv[1])==2)
        {
            std::cout << std::endl << "Starting CKKS with no RNS" << std::endl<< std::endl;
            multDepth = 0;
            firstModSize = 60;
            extra = "_nonRNS";
        }
        // Sin Ops
        else if(atoi(argv[1])==3)
        {
            std::cout << std::endl << "Starting CKKS with no Optimizations" << std::endl<< std::endl;
            multDepth = 0;
            firstModSize = 60;
            nonNTT = true;
            extra = "_nonOps";
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

    std::string dir_name = "./logs/log_encode/";
    std::string fileHD_limbsNotChange = "encodeHD"+extra+"_limbsNotChanged";
    std::string fileHD_limbChange     = "encodeHD"+extra+"_limbChanged";
    std::string fileHD_positions      = "encodeHD"+extra+"_positions";
    std::string fileN2_bounded        = "encodeN2"+extra+"_bounded";

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();

    // Input setup
    int max_diff = 255;
    size_t dataSize = 28*28;
    std::ifstream file("../data/example.txt");
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
    auto encryptElems_original = c2->GetElements();

    // Me haga un backup
    Plaintext ptxt_original = cc->MakeCKKSPackedPlaintext(input);
    DCRTPoly plainElem_original = ptxt_original->GetElement<DCRTPoly>();
    auto elems_original =  plainElem_original.GetAllElements();

    Plaintext result;
    std::vector<double> resultData(dataSize);

    size_t RNS_size = ptxt1->GetElement<DCRTPoly>().GetAllElements().size();
    std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << " RNS size: " << RNS_size << std::endl;
    std::cout << "NTT condition: " << nonNTT << ", RNS condition: " << multDepth << std::endl;
    // Chequeo que realmente estoy usando la cantidad de limbs de RNS que busco
    if (RNS_size == (size_t)multDepth+1 && execute)
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
            uint64_t count = 0;
            uint64_t HD_RNS_limbsNotChanged = 0;
            uint64_t HD_RNS_limbChanged = 0;
            double N2_bounded = 0;
            bool new_file = 1;
            auto ringDim = cc->GetRingDimension();
            size_t bits_coeff = 64;

            int total_loops  = RNS_size*ringDim;

            // Quiero guardarme un acumulador de cada posicion en bit de la encriptacion
            std::vector<uint64_t> encryption_bitChange(2*RNS_size*bits_coeff*ringDim, 0);
            saveDataLog(dir_name+fileHD_limbsNotChange, HD_RNS_limbsNotChanged,  new_file, RNS_size);
            saveDataLog(dir_name+fileHD_limbChange, HD_RNS_limbChanged, new_file, RNS_size);
            saveDataLog(dir_name+fileN2_bounded, N2_bounded,  new_file, RNS_size);

            // Me quedo con la componente cero por que aca no tengo RNS y es el unico
            std::cout << "Total loops: " << total_loops << std::endl;
            auto original_vector = ptxt1->GetElement<DCRTPoly>().GetAllElements();
            for (size_t i = 0; i < RNS_size; i++)
            {
                for (size_t j = 0; j < ringDim; j++)
                {
                    std::cout << count << ", " << std::flush;

                    auto original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                    for(size_t bit=0; bit<bits_coeff; bit++)
                    {
                        // Paso la codificacion al formato de coeficientes (INTT)
                        if(nonNTT)
                        {
                            // Paso la codificacion al formato de coeficientes (INTT)
                            ptxt1->GetElement<DCRTPoly>().SwitchFormat();
                            // Me quedo con un coefficiente. Tengo un backup de todos ya que cambian todos aca
                            original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j];
                        }
                        ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = bit_flip(original_coeff, bit);
                        // Vuelvo al espacio de evaluacion (NTT)
                        if(nonNTT)
                            ptxt1->GetElement<DCRTPoly>().SwitchFormat();
                        c1 = cc->Encrypt(keys.publicKey, ptxt1);
                        encryptElems = c1->GetElements();

                        auto [HD_RNS_limbsNotChanged, HD_RNS_limbsChanged] = hamming_distance_RNS(encryptElems, encryptElems_original, RNS_size, i);
                        hamming_distance_position(encryption_bitChange, encryptElems, encryptElems_original, RNS_size, bits_coeff);
                        saveDataLog(dir_name+fileHD_limbsNotChange, HD_RNS_limbsNotChanged, !new_file, RNS_size);
                        saveDataLog(dir_name+fileHD_limbChange, HD_RNS_limbsChanged, !new_file, RNS_size);

                        cc->Decrypt(keys.secretKey, c1, &result);
                        result->SetLength(dataSize);
                        resultData = result->GetRealPackedValue();
                      //  res_norm2 = norm2(input, resultData, dataSize);
                        N2_bounded = norm2_bounded(input, resultData, dataSize, max_diff);

                       // saveDataLog(dir_name+file_name_norm2, res_norm2, !new_file);
                        saveDataLog(dir_name+fileN2_bounded, N2_bounded, !new_file, RNS_size);
                        // Cambian todos los coefficientes, tengo que resetearlo entero
                        if(nonNTT)
                            ptxt1->GetElement<DCRTPoly>().GetAllElements() = original_vector;
                        // cambia solo uno
                        else
                            ptxt1->GetElement<DCRTPoly>().GetAllElements()[i][j] = original_coeff;

                    }
                    count++;
                }
            }
            std::fstream logFile;
            logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/logs/log_encode/"+fileHD_positions+".txt", std::ios::out);
            for(size_t i=0; i<encryption_bitChange.size(); i++)
                logFile << encryption_bitChange[i] << ", ";
            logFile.close();
        }
        else
            std::cout << "Test fail" << std::endl;
    }
    else
        std::cout << "Condition of amount of RNS limbs fail!" << std::endl;
    return 0;
}
