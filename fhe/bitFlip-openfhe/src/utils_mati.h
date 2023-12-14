// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

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

using namespace lbcrypto;

using Integer  = intnat::NativeIntegerT<long unsigned int>;

inline void saveDataLog(std::string file_name, int index_value, int bit_change, float res, bool new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        std::cout<< " New log: " << std::endl;
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/log_"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/log_"+file_name+".txt", std::ios::app);
        logFile << "Diff: " << res << " index_value: "<< index_value << " bit_changed: " << bit_change << std::endl ;
    }
    // close file stream
    logFile.close();

}

inline void saveDataLog(std::string file_name, uint64_t  res, bool new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        //std::cout<< " New log: " << std::endl;
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/"+file_name+".txt", std::ios::app);
        logFile << res << std::endl ;
    }
    // close file stream
    logFile.close();

}
inline void saveDataLog(std::string file_name, double res, bool new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        //std::cout<< " New log: " << std::endl;
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/bitFlip-openfhe/"+file_name+".txt", std::ios::app);
        logFile << res << std::endl ;
    }
    // close file stream
    logFile.close();

}


//queda el tema de cuantos bits veo....
//
inline void bit_check(std::vector<int>& vec, uint64_t coeff_change, uint64_t coeff_original, int offset, int max_bit )
{
    uint64_t res = 0;
    res =  coeff_change^coeff_original; // quedan en 1 los diferentes.
    std::bitset<64> diff(res);
    // creo que bitset tiene en posicion 0 el mas significativo
    for(int i = max_bit-1; i>=0; i--)
    {
        // arranco con el i de la posicion en realidad mas baja y le sumo el off set
        // pero antes de sumar mapeo para invertir el orden, el i=63->0 e i=0->63
        if (diff[i])
            vec.push_back((max_bit-1-i)+offset);
    }
}
inline void bit_check_sum(std::vector<int>& vec, uint64_t coeff_change, uint64_t coeff_original, int offset, int max_bit )
{
    uint64_t res = 0;
    res =  coeff_change^coeff_original; // quedan en 1 los diferentes.
    std::bitset<64> diff(res);
    int count = 0;
    // creo que bitset tiene en posicion 0 el mas significativo
    for(int i = 0; i<max_bit; i++)
    {
        if (diff[i])
            count++;
    }
    vec.push_back(count);
}
// La uso para testear con dos valores que les cambio un solo bit
inline int bit_check(uint64_t coeff_change, uint64_t coeff_original, int max_bit )
{
    int bit_changed = 0;
    uint64_t res = 0;
    res =  coeff_change^coeff_original; // quedan en 1 los diferentes.
  //  std::cout << coeff_change << " " << coeff_original << std::endl;
    std::bitset<64> diff(res);
    for(int i = max_bit-1; i>=0; i--)
    {
        if (diff[i])
            bit_changed+= i;
    }
    return bit_changed;
}


inline void saveEncodig(std::string file_name,  bool new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        //std::cout<< " New log: " << std::endl;
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    // close file stream
    logFile.close();

}
inline void saveEncodig(std::string file_name, std::vector<int> encoding_diff, bool new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        //std::cout<< " New log: " << std::endl;
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::app);
        if(encoding_diff.size()>0)
        {
            for (size_t index_value=0; index_value<encoding_diff.size(); index_value++)
            {
                logFile <<  encoding_diff[index_value] << " " ;
            }
            logFile << std::endl;
        }
    }
    // close file stream
    logFile.close();

}




// size np es v1.size...
// 1 si la diferencia es elemento a elemento menor al threshold, cero si no.
inline bool diff_elem(std::vector<double>  &v1, std::vector<double> &v2, float threshold){
    bool res = 1;
    double diff = 0;
    double diff_thresh = 0;
    int i = 0;
    int size = v1.size();
    if (v1.size()==v2.size()){
        while(res!=0 && i<size )
        {
            diff = abs(v1[i] - v2[i]);
            diff_thresh = abs(v1[i]*threshold);
            if(diff>diff_thresh)
                res = 0;
            i++;
        }
    }

    return res;
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

//inline double bit_flip(double& original, ushort bit)
//{
//    uint64_t* orig_int = (uint64_t*)&original;
//
//    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
//    uint64_t res = 0;
//    res =  mask^(*orig_int); // I flip the bit using xor with the mask.
//    //std::cout<< mask << " to int " << res << std::endl;
//    double res_double = *((double*)&res);
//    return res_double;//_double;
//}
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
    // el vector output puede tener otro size... entonces miro el input
    for(size_t i = 0; i<size; i++)
        count += hamming_distance(vecInput[i], (uint64_t)vecOutput[i]);
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
inline uint64_t hamming_distance(std::vector<uint64_t> &x_plain, std::vector<uint64_t> &x_plain_original, uint64_t coeff_count)
{
    uint64_t count = 0;
    // el vector output puede tener otro size... entonces miro el input
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
// se podria paralelizar...
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
// Calcula el HD del limb de RNS que no fue modificado
inline uint64_t hamming_distance_RNS(std::vector<DCRTPoly> &x_encrypt, std::vector<DCRTPoly> &x_encrypt_original, size_t RNS_limb)
{
    uint64_t count = 0;
    // el vector output puede tener otro size... entonces miro el input
    for (size_t k=0 ; k<2; k++)
    {
        auto x_encrypt_elems = x_encrypt[k].GetAllElements();
        auto x_encrypt_original_elems = x_encrypt_original[k].GetAllElements();


        size_t ringDim = x_encrypt[k].GetRingDimension();
        int i = 0;
        if (RNS_limb==0)
            i = 1;
        for(size_t j = 0; j < ringDim; j++)
            count += hamming_distance((uint64_t)x_encrypt_elems[i][j], (uint64_t)x_encrypt_original_elems[i][j]);
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


