#include "math/hal/intnat/transformnat.h"
#include <cstdint>
#define PROFILE
#include "openfhe.h"
#include "iostream"
#include "utils_mati.h"
#include <vector>
#include <cmath>

using namespace lbcrypto;
static const usint DCRTBITS     = MAX_MODULUS_SIZE;
static const usint RING_DIM_LOG = 12;
static const size_t POLY_NUM    = 16;


int main() {
    std::shared_ptr<ILNativeParams> Nativeparms;
    std::shared_ptr<std::vector<NativePoly>> NativepolysCoef;
    std::shared_ptr<std::vector<NativePoly>> NativepolysCoef_original;

    uint32_t m                 = (1 << (RING_DIM_LOG + 1));
    NativeInteger firstInteger = FirstPrime<NativeInteger>(DCRTBITS, m);
    NativeInteger modulo       = PreviousPrime<NativeInteger>(firstInteger, m);
    NativeInteger root         = RootOfUnity<NativeInteger>(m, modulo);
    std::cout << m << " " << firstInteger << " " << modulo << " " << root << std::endl;
    ChineseRemainderTransformFTT<NativeVector>().PreCompute(root, m, modulo);
    Nativeparms = std::make_shared<ILNativeParams>(m, modulo, root);
    std::vector<NativePoly> vecCoef;
    std::vector<NativePoly> vecCoef2;

    for (size_t i = 0; i < POLY_NUM; i++)
    {
        NativeVector vec = DiscreteUniformGeneratorImpl<NativeVector>().GenerateVector(Nativeparms->GetRingDimension(), Nativeparms->GetModulus());
        NativePoly elem(Nativeparms);
        NativePoly elem2(Nativeparms);
        elem.SetValues(vec, Format::COEFFICIENT);
        elem2.SetValues(vec, Format::COEFFICIENT);
        vecCoef.push_back(elem);
        vecCoef2.push_back(elem);
    }

    NativepolysCoef = std::make_shared<std::vector<NativePoly>>(std::move(vecCoef));
    NativepolysCoef_original = std::make_shared<std::vector<NativePoly>>(std::move(vecCoef2));

    // Pruebo aplicando INTT al primer poly y comparo el que el primer coeff al menos cambio
    // pero que del segundo poly no cambio
    // y despues le hago la NTT y veo que volvio a original
    auto coeff0          = NativepolysCoef->at(0)[0];
    auto coeff0_original = NativepolysCoef_original->at(0)[0];
    auto coeff1          = NativepolysCoef->at(1)[0];

    NativepolysCoef->at(0).SwitchFormat();
    if (coeff0 == NativepolysCoef->at(0)[0] && coeff0_original != NativepolysCoef_original->at(0)[0] && coeff1 != NativepolysCoef->at(1)[0])
        std::cout << "Test one failed" << std::endl;
    NativepolysCoef->at(0).SwitchFormat();
    if ( coeff0 != NativepolysCoef->at(0)[0]  && coeff0_original != NativepolysCoef_original->at(0)[0] && coeff1 != NativepolysCoef->at(1)[0])
        std::cout << "Test two failed" << std::endl;

    std::cout << " size " << NativepolysCoef->at(0).GetLength() << std::endl;
    uint64_t res_hd = 0;
    double res_n2 = 0;

    bool new_file = 1;
    std::string dir_name = "logs/";
    std::string file_name_hd = "ntt_bitFlip_HD";
    std::string file_name_norm2 =  "ntt_bitFlip_N2";
    saveDataLog(dir_name+file_name_hd, res_hd,  new_file, 0);
    saveDataLog(dir_name+file_name_norm2, res_n2,  new_file,0);

    size_t bits_modulus = 60;
    int size_poly = NativepolysCoef->at(0).GetLength();
    for(int index=0; index < size_poly; index++)
    {
        std::cout << index << ", " << std::flush;
        for(size_t bit=0; bit<bits_modulus; bit++)
        {
            NativepolysCoef->at(0).SwitchFormat();
            //std::cout << "Coeff " << NativepolysCoef->at(0)[index] << " " << NativepolysCoef_original->at(0)[index]<< std::endl;
            NativepolysCoef->at(0)[index] = bit_flip(NativepolysCoef->at(0)[index], bit);
            //std::cout << "Coeff " << NativepolysCoef->at(0)[index] << " " << NativepolysCoef_original->at(0)[index]<< std::endl;
            NativepolysCoef->at(0).SwitchFormat();
            res_hd = hamming_distance(NativepolysCoef->at(0), NativepolysCoef_original->at(0));
            res_n2 = norm2(NativepolysCoef->at(0), NativepolysCoef_original->at(0));
            saveDataLog(dir_name+file_name_hd, res_hd, !new_file,0);
            saveDataLog(dir_name+file_name_norm2, res_n2, !new_file,0);
            for (unsigned int i=0; i<NativepolysCoef->at(0).GetLength(); i++)
                NativepolysCoef->at(0)[i] = NativepolysCoef_original->at(0)[i];
        }
    }


    return 0;
}
