// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include "seal/seal.h"
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
#include <seal/ciphertext.h>
#include <seal/plaintext.h>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <array>
#include <bitset>


inline void saveDataLog(std::string file_name, int index_value, int bit_change, float res, bool new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        std::cout<< " New log: " << std::endl;
        logFile.open("/home/mmazz/phd/fhe/sealProfile/log_"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/sealProfile/log_"+file_name+".txt", std::ios::app);
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
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::app);
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


// el offset_rate es la cantidad de bits que tiene los coefficientes del polinomio.
// me sirve tanto para ir mirando cada coefficiente indivudalmente
inline void  comparePlaintext(std::vector<int>& diff, seal::Plaintext x_plain, seal::Plaintext x_plain_original, int offset_rate, bool bit_to_bit)
{
    size_t size_x = x_plain.coeff_count();
    size_x = 4096;
    int offset = 0;
    for(int i=0;i<size_x; i++)
    {
        if(x_plain[i]!=x_plain_original[i])
        {
            if(bit_to_bit)
                bit_check(diff, x_plain[i], x_plain_original[i], offset, offset_rate);
            else
                bit_check_sum(diff, x_plain[i], x_plain_original[i], offset, offset_rate);
        }
        // 40 en general, auqnue en realidad son de 64....
        offset+=offset_rate;
    }
}

// Devuelvo la cantidad de coefficientes iguales que tengo
inline int  comparePlaintextCoeff(seal::Plaintext x_plain, seal::Plaintext x_plain_original)
{
    size_t size_x = x_plain.capacity();
    int offset = 0;
    int count = 0;
    for(int i=0;i<size_x; i++)
    {
        if(x_plain[i]!=x_plain_original[i])
            count++;
    }
    return size_x - count;
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
            for (int index_value=0; index_value<encoding_diff.size(); index_value++)
            {
                logFile <<  encoding_diff[index_value] << " " ;
            }
            logFile << std::endl;
        }
    }
    // close file stream
    logFile.close();

}

inline double norm2_vec(std::vector<double>  &v1, std::vector<double> &v2){
    double res = 0;
    double diff = 0;
    for (int i=0; i<v2.size(); i++)
    {
        diff = v1[i] - v2[i];
        res += pow(diff, 2);
    }
    res = std::sqrt(res);

    return res;
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
//    else{
//        for (int i=0; i<size; i++){
//            diff = abs(v1[i] - v2[i]);
//            diff_thresh = abs(v1[i]*threshold);
//            if(diff>diff_thresh)
//            {
//                //std::cout << v1[i] << ": " << diff << " > " << diff_thresh<< std::endl;
//                res = 0;
//                break;
//            }
//        }
//    }
    return res;
}
inline uint64_t bit_flip(uint64_t original, ushort bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    uint64_t res = 0;
    res =  mask^original; // I flip the bit using xor with the mask.
    return res;
}

// double tiene 8 bytes
// bit ira de 0 a 64
// 1 bit de signo, 11 bits para el exponente y 52 para la mantisa (el significando).
// el numero se interpreta como signo*1.mantisa (ese 1 es implicito) *2^(exponente)
// el exponente es en exceso 2^(n-1). Es decir le resto al exponente 2^(n-1)
// con n el numero de bits del exponente.
// Ademas dependiendo del exponente se calcula diferente el numero.
// Si el exponente es cero y el significando tambien, es +/- cero.
// Si el exponente es cero y lo otro no, entonces es 0.mantisa*2^(emin), emin= -exceso+1 (denormalizado)
// exponente todo 1 y singificando cero = infinito
// exponente todo 1 y signcando distinto de cero = Nan
// otros casos como dijimos, se llaama normalizado.
// Ej: 1, tiene significando 0 y tiene exponente 0, como lo represento? bits exponente - exceso (011 1111 1111)
// para que de cero, el exponente tiene que ser el exceso osea 011 1111 1111
inline double bit_flip(double& original, ushort bit)
{
    uint64_t* orig_int = (uint64_t*)&original;

    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    uint64_t res = 0;
    res =  mask^(*orig_int); // I flip the bit using xor with the mask.
    //std::cout<< mask << " to int " << res << std::endl;
    double res_double = *((double*)&res);
    return res_double;//_double;
}

inline uint64_t hamming_distance(double& coeff1, double& coeff2)
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
inline uint64_t hamming_distance(std::vector<double>& vec1, std::vector<double>& vec2)
{
    uint64_t count = 0;
    if (vec1.size() == vec2.size())
    {
        for(unsigned int i = 0; i < vec1.size(); i++)
            count += hamming_distance(vec1[i], vec2[i]);
    }
    return count;
}

inline bool check_equality(seal::Plaintext &x_plain, seal::Plaintext &x_plain2){
    int res=0;
    bool res_bool=0;
    int total_num_coeff = x_plain.coeff_count();
    for(int i=0;i<total_num_coeff; i++){
        if(x_plain[i]!=x_plain2[i])
            res+=1;
    }
    if (res != 0)
        std::cout<< "Equality check is: Failed, " << res << std::endl;
    else
        res_bool = 1;
    return res_bool;
}

// Se fija componente a componente si son iguales, y va sumando 1 por cada uno que es distinto
inline int count_diff_coef(seal::Ciphertext &x_encrypted, seal::Ciphertext &x_encrypted2){
    int res=0;
    bool res_bool=0;

    size_t size_x = x_encrypted.size();
    int poly_degree =  x_encrypted.poly_modulus_degree();
    int k_rns =  x_encrypted.coeff_modulus_size();
    int total_num_coeff = size_x*poly_degree*k_rns;
    for(int i=0;i<total_num_coeff; i++){
        if(x_encrypted[i]!=x_encrypted2[i])
            res+=1;
    }
    return res;
}


// Se fija si tiene al menos un elemento distinto
// da 1 si son iguales, 0 si no
inline bool check_equality(seal::Ciphertext &x_encrypted, seal::Ciphertext &x_encrypted2)
{
    int res=0;
    bool res_bool=0;

    size_t size_x = x_encrypted.size();
    int poly_degree =  x_encrypted.poly_modulus_degree();
    int k_rns =  x_encrypted.coeff_modulus_size();
    int total_num_coeff = size_x*poly_degree*k_rns;
    //std::cout << k_rns << " " << total_num_coeff<< std::endl;
    int i=0;
    while(i<total_num_coeff && res==0){
        if(x_encrypted[i]!=x_encrypted2[i])
            res+=1;
        i++;
    }
    if (res == 0)
        res_bool = 1;
    return res_bool;
}

inline void  reset_values(seal::Plaintext &x_plain){
    // Me traigo los valores de x_plain antes de aplicarle  NTT
    std::string file_name = "/home/mmazz/phd/fhe/sealProfile/SEALlog_nonNTT_plaintextValues.txt";
    std::ifstream file(file_name);
    std::vector<uint64_t>  data(std::istream_iterator<uint64_t>{file},
                        std::istream_iterator<uint64_t>{});

    uint64_t i_max = data[0];
    for (uint64_t i=0; i < i_max; i++){
        x_plain[i] = data[i+1];
    }
}

inline void ntt_transformation(seal::Plaintext &x_plain, size_t coeff_modulus_size,
                               size_t coeff_count, const seal::util::NTTTables* ntt_tables){
    for (size_t i = 0; i < coeff_modulus_size; i++)
    {
        seal::util::ntt_negacyclic_harvey(x_plain.data(i * coeff_count), ntt_tables[i]);
    }
}

// para que solo haga uno valor de k.
inline void ntt_transformation(seal::Plaintext &x_plain, const seal::util::NTTTables* ntt_tables, int modulus_index, int poly_degree){
    seal::util::ntt_negacyclic_harvey(x_plain.data() + (modulus_index * poly_degree), ntt_tables[modulus_index]);
}
// solo cambio en el modulo que me interesa.
//
inline void ntt_transformation(seal::Ciphertext &x_encrypted, const seal::util::NTTTables* ntt_tables, int modulus_index, int cipher_index){
    size_t poly_degree =  x_encrypted.poly_modulus_degree();
    if (cipher_index==0)
        seal::util::ntt_negacyclic_harvey(x_encrypted.data(0) + (modulus_index * poly_degree), ntt_tables[modulus_index]);
    if (cipher_index==1) // start at c1.
        seal::util::ntt_negacyclic_harvey(x_encrypted.data(1) + (modulus_index * poly_degree), ntt_tables[modulus_index]);
}


inline void restoreCiphertext(seal::Ciphertext &x_encrypted, seal::Ciphertext &x_encrypted_original, int modulus_index){
    size_t poly_degree =  x_encrypted.poly_modulus_degree();
    for(int i=0; i<poly_degree; i++){
        x_encrypted[i + modulus_index*poly_degree] = x_encrypted_original[i + modulus_index*poly_degree];
    }
}

inline void input_creator(std::vector<double> &input, int poly_modulus_degree, double curr_point, double max_value){
    size_t slot_count = poly_modulus_degree/2;
    std::cout << "Creating array, starting at " << curr_point << " and ending at " << max_value << std::endl;
    double step_size = max_value / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
}

inline void input_creator(std::vector<double> &input, int poly_modulus_degree, size_t slot_count, double curr_point, double max_value){
    std::cout << "Creating array, starting at " << curr_point << " and ending at " << max_value << std::endl;
    double step_size = max_value / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
}


inline void input_creator(std::vector<double> &input, int poly_modulus_degree, double curr_point, double max_value, int size){
    std::cout << "Creating array, starting at " << curr_point << " and ending at " << max_value << std::endl;
    double step_size = max_value / (static_cast<double>(size) - 1);
    for (size_t i = 0; i < size; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
}

inline void input_creator(std::vector<double> &input, int poly_modulus_degree, int seed, double max_value, int size){
    float random_value = 0;
    srand(seed);
    for (size_t i = 0; i < size; i++)
    {
        random_value = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/max_value));
        input.push_back(random_value);
    }
}

inline void input_refill(std::vector<double> &input, int poly_modulus_degree, double curr_point, double max_value){
    size_t slot_count = poly_modulus_degree/2;
    std::cout << "Creating array, starting at " << curr_point << " and ending at " << max_value << std::endl;
    double step_size = max_value / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < slot_count; i++)
    {
        input[i] = curr_point;
        curr_point += step_size;
    }
}

inline void input_refill(std::vector<double> &input, int poly_modulus_degree, int seed, double max_value){
    float random_value = 0;
    srand(seed);
    for (size_t i = 0; i < input.size(); i++)
    {
        random_value = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/max_value));
        input[i] = random_value;
    }
}
