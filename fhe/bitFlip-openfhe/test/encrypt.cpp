#include "ciphertext-fwd.h"
#include "encoding/plaintext-fwd.h"
#include "math/hal/intnat/transformnat.h"
#include "math/hal/nativeintbackend.h"
#include <cstddef>
#include <cstdint>
#include <string>
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"
#include "../src/utils_mati.h"




using namespace lbcrypto;
using DggType  = typename DCRTPoly::DggType;

int main(int argc, char * argv[])
{

    uint32_t multDepth = 4;
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
    cc->Enable(PKE);
    cc->Enable(LEVELEDSHE);
    auto keys = cc->KeyGen();
    const auto cryptoParams = std::dynamic_pointer_cast<CryptoParametersRNS>(keys.publicKey->GetCryptoParameters());
    size_t numModuli = cryptoParams->GetElementParams()->GetParams().size();

    std::cout << "Modulus   numbers = " <<  numModuli << std::endl;
    const DggType& dgg =  cryptoParams->GetDiscreteGaussianGenerator();

    std::cout<< dgg.IsInitialized() << std::endl;
    // Input setup
    std::vector<double> input = {0,1,2,3};
    auto a = cryptoParams->GetSecretKeyDist() == GAUSSIAN;
    std::cout << " Secret Key dist is Gaussian ?: " <<  a << std::endl;

    std::cout << "Encryption tecnique " <<    cryptoParams->GetEncryptionTechnique() << std::endl;
    // Encoding as plaintexts
    Plaintext ptxt1 = cc->MakeCKKSPackedPlaintext(input);
    NativeInteger original_coeff = ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0];
    auto c1 = cc->Encrypt(keys.publicKey, ptxt1);
    auto encryptElems = c1->GetElements();
    auto a1 = encryptElems[0].GetAllElements()[0][0];
    auto encryptModulus = encryptElems[0].GetModulus();
    std::cout << std::endl;
    std::cout << "Modulus    = " <<  encryptModulus << std::endl;
    std::cout << "ptxt[0][0] = " << original_coeff << std::endl;
    std::cout << "Encrypt[0] = " << a1 << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    size_t bit_change = 64;
    NativeInteger c0 = 525107833192280017;
    for(; bit_change<64; bit_change++)
    {
        ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0] = bit_flip(original_coeff, bit_change);
        NativeInteger coeff_ptxt = ptxt1->GetElement<DCRTPoly>().GetAllElements()[0][0];
        auto c2 = cc->Encrypt(keys.publicKey, ptxt1);
        encryptElems = c2->GetElements();
        encryptModulus = encryptElems[0].GetModulus();
        NativeInteger encryptElems_c0 = encryptElems[0].GetAllElements()[0][0];
        std::cout << "bitchange     = " << bit_change <<std::endl;
        std::cout << "Modulus       = " << encryptModulus << std::endl;
        std::cout << "ptxt[0][0]    = " << coeff_ptxt << std::endl;
        std::cout << "Encrypt[0][0] = " << encryptElems_c0 << std::endl;
        std::cout << "c0+ptxt       = " << c0+coeff_ptxt << std::endl;
        std::cout << "c0+ptxt mod   = " << (c0+coeff_ptxt)%(NativeInteger)encryptModulus  <<std::endl;
        std::cout << "c0+ptxt mod2  = " << c0.ModAddEq(coeff_ptxt, encryptModulus)  <<std::endl;
        std::cout << std::endl;
    }
    Plaintext result;
    cc->Decrypt(keys.secretKey, c1, &result);

    const std::vector<std::complex<double>> complex[4] = {{0,1},{2,3},{4,5},{6,7}};
    return 0;
}
