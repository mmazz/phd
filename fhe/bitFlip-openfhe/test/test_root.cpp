#include <string>
#define FIXED_SEED
#include "openfhe.h"
#include "iostream"

using namespace lbcrypto;

int main(int argc, char * argv[])
{

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

    unsigned int m = 16;
    const DCRTPoly::Integer modulo = 33;
    auto a = RootOfUnity(m, modulo);
    std::cout << std::endl;
    std::cout << "raiz de la unidad" << a << std::endl;
    std::cout << std::endl;

    return 0;
}
