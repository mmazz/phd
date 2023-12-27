#pragma once

#include "lattice/hal/lat-backend.h"
#include "math/hal/intnat/ubintnat.h"
#include "openfhe.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>
#include <array>
#include <bitset>
#include <tuple>

using namespace lbcrypto;

using Integer  = intnat::NativeIntegerT<long unsigned int>;

inline void deepCopy(std::vector<Integer>& vector, std::vector<DCRTPoly>& encryptElems, size_t RNS_size)
{
    int index = 0;
    for(size_t k=0; k<2 ; k++)
    {
        size_t ringDim = encryptElems[k].GetRingDimension();
        for (size_t i = 0; i < RNS_size; i++)
        {
            std::cout << i << std::endl;
            for (size_t j = 0; j < ringDim; j++)
            {
                vector[index] = encryptElems[k].GetAllElements()[i][j];
                index++;
            }
        }
    }
}
inline void saveDataLog(std::string file_name, double res, bool new_file, size_t rns_size)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/"+file_name+".txt", std::ios::out);
        logFile << "New file: "<< rns_size << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/"+file_name+".txt", std::ios::app);
        logFile << res << std::endl ;
    }
    // close file stream
    logFile.close();

}


inline uint64_t bit_flip(uint64_t original, size_t bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    uint64_t res = 0;
    res =  mask^original; // I flip the bit using xor with the mask.
    return res;
}

inline uint64_t bit_flip(Integer original, size_t bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    uint64_t res = 0;
    uint64_t* original_int = (uint64_t*)&original;
    res =  mask^(*original_int); // I flip the bit using xor with the mask.
    return res;
}

inline double norm2(NativePoly &x_plain, NativePoly &x_plain_original){
    double res = 0;
    uint64_t diff = 0;
    // Itero sobre el del input por si el del output por construccion quedo mas grande
    for (size_t i=0; i<x_plain.GetLength(); i++)
    {
        diff = (uint64_t)x_plain[i] - (uint64_t)x_plain_original[i];
        res += pow(diff, 2);
    }
    res = std::sqrt(res);
    return res;
}


inline double norm2(std::vector<double>  &vecInput, std::vector<double> &vecOutput, size_t size){
    double res = 0;
    double diff = 0;
    // Itero sobre el del input por si el del output por construccion quedo mas grande
    for (size_t i=0; i<size; i++)
    {
        diff = vecInput[i] - vecOutput[i];
        res += pow(diff, 2);
    }
    res = std::sqrt(res);
    return res;
}

inline double norm2_bounded(std::vector<double>  &vecInput, std::vector<double> &vecOutput, size_t size, int max_diff){
    double res = 0;
    double diff = 0;
    // Itero sobre el del input por si el del output por construccion quedo mas grande
    for (size_t i=0; i<size; i++)
    {
        diff = fabs(vecInput[i] - vecOutput[i]);
        if(diff>(double)max_diff)
            res += pow((double)max_diff, 2);
        else
            res += pow(diff, 2);
    }
    res = std::sqrt(res);
    return res;
}



inline uint64_t hamming_distance(double coeff1, double coeff2)
{
    uint64_t* coeff1_int= (uint64_t*)&coeff1;
    uint64_t* coeff2_int= (uint64_t*)&coeff2;

    uint64_t res = 0;
    res = (*coeff1_int)^(*coeff2_int);
    uint64_t count = 0;
    while (res) {
        count += res & 1;
        res >>= 1;
    }
    return count;
}

inline uint64_t hamming_distance(uint8_t coeff1, uint64_t coeff2)
{
    uint8_t coeff2_8b = 0;
    if(coeff2 > 255)
        coeff2_8b = 255;
    else
        coeff2_8b = (uint8_t) coeff2;
    uint8_t res = 0;
    res = coeff1^coeff2_8b;
    uint8_t count = 0;
    while (res) {
        count += res & 1;
        res >>= 1;
    }
    return count;
}

inline uint64_t hamming_distance(uint64_t coeff1, uint64_t coeff2)
{
    uint64_t res = 0;
    res = coeff1^coeff2;
    uint64_t count = 0;
    while (res) {
        count += res & 1;
        res >>= 1;
    }
    return count;
}



// se podria paralelizar...
inline uint64_t hamming_distance(std::vector<double>& vecInput, std::vector<double>& vecOutput, size_t size)
{
    uint64_t count = 0;
    // el vector output puede tener otro size... entonces miro el input
    for(size_t i = 0; i<size; i++)
        count += hamming_distance(vecInput[i], vecOutput[i]);
    return count;
}

inline uint64_t hamming_distance(std::vector<uint8_t>& vecInput, std::vector<double>& vecOutput, size_t size)
{
    uint64_t count = 0;
    for(size_t i = 0; i<size; i++)
        count += hamming_distance(vecInput[i], (uint64_t)vecOutput[i]);
    return count;
}

inline uint64_t hamming_distance(std::vector<uint64_t> &x_plain, std::vector<uint64_t> &x_plain_original, uint64_t coeff_count)
{
    uint64_t count = 0;
    for(unsigned int i = 0; i < coeff_count; i++)
        count += hamming_distance(x_plain[i], x_plain_original[i]);
    return count;
}

inline uint64_t hamming_distance(DCRTPoly &x_plain, DCRTPoly &x_plain_original, size_t RNS_size)
{
    uint64_t count = 0;
    // el vector output puede tener otro size... entonces miro el input
    auto x_plain_elems = x_plain.GetAllElements();
    auto x_plain_original_elems = x_plain_original.GetAllElements();

    for (size_t i = 0; i < RNS_size; i++)
    {
        size_t ringDim = x_plain.GetRingDimension();
        for(size_t j = 0; j < ringDim; j++)
            count += hamming_distance((uint64_t)x_plain_elems[i][j], (uint64_t)x_plain_original_elems[i][j]);
    }
    return count;
}

inline uint64_t hamming_distance(std::vector<DCRTPoly> &x_encrypt, std::vector<DCRTPoly> &x_encrypt_original, size_t RNS_size)
{
    uint64_t count = 0;
    // el vector output puede tener otro size... entonces miro el input
    for (size_t k=0 ; k<2; k++)
    {
        auto x_encrypt_elems = x_encrypt[k].GetAllElements();
        auto x_encrypt_original_elems = x_encrypt_original[k].GetAllElements();


        for (size_t i = 0; i < RNS_size; i++)
        {
            size_t ringDim = x_encrypt[k].GetRingDimension();
            for(size_t j = 0; j < ringDim; j++)
                count += hamming_distance((uint64_t)x_encrypt_elems[i][j], (uint64_t)x_encrypt_original_elems[i][j]);
        }
    }
    return count;
}

inline uint64_t hamming_distance(NativePoly &x_plain, NativePoly &x_plain_original)
{
    uint64_t count = 0;
    // el vector output puede tener otro size... entonces miro el input
    auto coeff_count = x_plain.GetLength();
    for(unsigned int i = 0; i < coeff_count; i++)
        count += hamming_distance((uint64_t)x_plain[i], (uint64_t)x_plain_original[i]);
    return count;
}


// Calcula el HD del limb de RNS que no fue modificado
inline std::tuple<uint64_t, uint64_t>hamming_distance_RNS(DCRTPoly &x_plain, DCRTPoly &x_plain_original, size_t RNS_size, size_t RNS_limb)
{
    uint64_t count_RNS_limbsNotChanged = 0;
    uint64_t count_RNS_limbChanged = 0;

    // el vector output puede tener otro size... entonces miro el input
    auto x_plain_elems = x_plain.GetAllElements();
    auto x_plain_original_elems = x_plain_original.GetAllElements();
    size_t ringDim = x_plain.GetRingDimension();
    for(size_t i=0; i<RNS_size; i++)
    {
        if(i!=RNS_limb)
        {
            for(size_t j = 0; j < ringDim; j++)
                count_RNS_limbsNotChanged += hamming_distance((uint64_t)x_plain_elems[i][j], (uint64_t)x_plain_original_elems[i][j]);
        }
        else
        {
            for(size_t j = 0; j < ringDim; j++)
                count_RNS_limbChanged += hamming_distance((uint64_t)x_plain_elems[i][j], (uint64_t)x_plain_original_elems[i][j]);
        }
    }
    return {count_RNS_limbsNotChanged, count_RNS_limbChanged };
}

inline std::tuple<uint64_t, uint64_t>hamming_distance_RNS(std::vector<DCRTPoly> &x_encrypt, std::vector<DCRTPoly> &x_encrypt_original, size_t RNS_size, size_t RNS_limb)
{
    uint64_t count_RNS_limbsNotChanged = 0;
    uint64_t count_RNS_limbChanged = 0;
    size_t polynomials = 2;

    // el vector output puede tener otro size... entonces miro el input
    for (size_t k=0 ; k<polynomials; k++)
    {
        auto x_encrypt_elems = x_encrypt[k].GetAllElements();
        auto x_encrypt_original_elems = x_encrypt_original[k].GetAllElements();


        size_t ringDim = x_encrypt[k].GetRingDimension();
        for(size_t i=0; i<RNS_size; i++)
        {
            if(i!=RNS_limb)
            {
                for(size_t j = 0; j < ringDim; j++)
                    count_RNS_limbsNotChanged += hamming_distance((uint64_t)x_encrypt_elems[i][j], (uint64_t)x_encrypt_original_elems[i][j]);
            }
            else
            {
                for(size_t j = 0; j < ringDim; j++)
                    count_RNS_limbChanged += hamming_distance((uint64_t)x_encrypt_elems[i][j], (uint64_t)x_encrypt_original_elems[i][j]);
            }
        }
    }
    return {count_RNS_limbsNotChanged, count_RNS_limbChanged };
}

inline void hamming_distance_position(std::vector<uint64_t>& acumulator, DCRTPoly &x_plain, DCRTPoly &x_plain_original, size_t RNS_size, size_t coeff_bits)
{
    size_t ringDim = x_plain.GetRingDimension();
    int index_bit = 0;
    size_t count_bits_perCoeff = 0;
    uint64_t xor_res = 0;
    uint64_t coeff_ij = 0;
    uint64_t coeff_orig_ij = 0;

    auto x_plain_elems = x_plain.GetAllElements();
    auto x_plain_original_elemsm = x_plain_original.GetAllElements();

    for(size_t i=0; i<RNS_size; i++)
    {
        for(size_t j = 0; j < ringDim; j++)
        {
            coeff_bits = (uint64_t)x_plain_elems[i][j];
            coeff_orig_ij = (uint64_t)x_plain_original_elemsm[i][j];
            xor_res = coeff_ij^coeff_orig_ij;
            count_bits_perCoeff = 0;
            while(count_bits_perCoeff<64)
            {
                acumulator[index_bit] += xor_res & 1;
                xor_res >>= 1;
                index_bit++;
                count_bits_perCoeff++;
            }
        }

    }
}

inline void hamming_distance_position(std::vector<uint64_t>& acumulator, std::vector<DCRTPoly> &x_encrypt, std::vector<DCRTPoly> &x_encrypt_original, size_t RNS_size, size_t coeff_bits)
{
    size_t ringDim = x_encrypt[0].GetRingDimension();
    int index_bit = 0;
    size_t count_bits_perCoeff = 0;
    uint64_t xor_res = 0;
    uint64_t coeff_ij = 0;
    uint64_t coeff_orig_ij = 0;

    // el vector output puede tener otro size... entonces miro el input
    for (size_t k=0 ; k<2; k++)
    {
        auto x_encrypt_elems = x_encrypt[k].GetAllElements();
        auto x_encrypt_original_elems = x_encrypt_original[k].GetAllElements();

        ringDim = x_encrypt[k].GetRingDimension();
        for(size_t i=0; i<RNS_size; i++)
        {
            for(size_t j = 0; j < ringDim; j++)
            {
                coeff_bits = (uint64_t)x_encrypt_elems[i][j];
                coeff_orig_ij = (uint64_t)x_encrypt_original_elems[i][j];
                xor_res = coeff_ij^coeff_orig_ij;
                count_bits_perCoeff = 0;
                while(count_bits_perCoeff<64)
                {
                    acumulator[index_bit] += xor_res & 1;
                    xor_res >>= 1;
                    index_bit++;
                    count_bits_perCoeff++;
                }
            }

        }
    }
}



