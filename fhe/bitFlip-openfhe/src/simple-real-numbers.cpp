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
    uint32_t batchSize = 1<<10;

    CCParams<CryptoContextCKKSRNS> parameters;
    parameters.SetMultiplicativeDepth(multDepth);
    parameters.SetScalingModSize(scaleModSize);
    parameters.SetBatchSize(batchSize);
    CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);

    // Enable the features that you wish to use
    cc->Enable(PKE);
    cc->Enable(KEYSWITCH);
    cc->Enable(LEVELEDSHE);
    std::cout << "CKKS scheme is using ring dimension " << cc->GetRingDimension() << std::endl << std::endl;
    std::vector<int> test = {1, 2, 3};
    auto keys = cc->KeyGen();
    cc->EvalMultKeyGen(keys.secretKey);
    cc->EvalRotateKeyGen(keys.secretKey, {1, -2});

    // Step 3: Encoding and encryption of inputs
    std::ifstream file("data/example.txt");
    std::vector<double> x1(std::istream_iterator<double>{file}, std::istream_iterator<double>{});
    x1.erase(x1.begin()); // pop front

    // padding of zeros
    for (size_t i=x1.size(); i<batchSize; i++)
        x1.push_back(0);
    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(x1);
    DCRTPoly plainElem = ptxt1->GetElement<DCRTPoly>();

    size_t RNS_size = plainElem.GetAllElements().size();
    //for (size_t i = 0; i < RNS_size; i++)
    for (size_t i = 0; i < 1; i++)
    {
        //size_t poly_degree = plainElem.GetAllElements()[i].GetLength();
        //for (size_t j = 0; j < poly_degree; j++)
        for (size_t j = 0; j < 1; j++)
        {
            auto original = plainElem.GetAllElements()[i][j];
            std::cout << "Original " <<plainElem.GetAllElements()[i][j] << std::endl;
            for(size_t bit=0; bit<64; bit++)
            {
                plainElem.GetAllElements()[i][j] = bit_flip(original, bit);
                std::cout << "BitChanged " << bit << " " << plainElem.GetAllElements()[i][j] << std::endl;
                plainElem.GetAllElements()[i][j] = original;
            }
            std::cout << "Polynomial " << i << " " << plainElem.GetAllElements()[i][j] << std::endl;
        }
    }

    std::cout << "RNS size: " << RNS_size << " Polynomial degree: " << plainElem.GetAllElements()[0].GetLength() << std::endl;

    // Encrypt the encoded vectors
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);


    // Step 4: Evaluation
    // Step 5: Decryption and output
    Plaintext result;
    // We set the cout precision to 8 decimal digits for a nicer output.
    // If you want to see the error/noise introduced by CKKS, bump it up
    // to 15 and it should become visible.
    std::cout.precision(8);

    std::cout << std::endl << "Results of homomorphic computations: " << std::endl;

    cc->Decrypt(keys.secretKey, c1, &result);
    // get output of the image
    result->SetLength(28*28);
    std::cout << "Estimated precision in bits: " << result->GetLogPrecision() << std::endl;

    std::ofstream outfile_nonBitFlip;
    outfile_nonBitFlip.open("data/example_nonBitflip.txt", std::ios_base::out);

    outfile_nonBitFlip << result << std::endl;


    return 0;
}
