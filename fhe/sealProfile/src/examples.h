// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include "seal/seal.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
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
#include <thread>
#include <vector>

/*
Helper function: Prints the name of the example in a fancy banner.
*/
inline void print_example_banner(std::string title)
{
    if (!title.empty())
    {
        std::size_t title_length = title.length();
        std::size_t banner_length = title_length + 2 * 10;
        std::string banner_top = "+" + std::string(banner_length - 2, '-') + "+";
        std::string banner_middle = "|" + std::string(9, ' ') + title + std::string(9, ' ') + "|";

        std::cout << std::endl << banner_top << std::endl << banner_middle << std::endl << banner_top << std::endl;
    }
}

/*
Helper function: Prints the parameters in a SEALContext.
*/
inline void print_parameters(const seal::SEALContext &context)
{
    auto &context_data = *context.key_context_data();

    /*
    Which scheme are we using?
    */
    std::string scheme_name;
    switch (context_data.parms().scheme())
    {
    case seal::scheme_type::bfv:
        scheme_name = "BFV";
        break;
    case seal::scheme_type::ckks:
        scheme_name = "CKKS";
        break;
    case seal::scheme_type::bgv:
        scheme_name = "BGV";
        break;
    default:
        throw std::invalid_argument("unsupported scheme");
    }
    std::cout << "/" << std::endl;
    std::cout << "| Encryption parameters :" << std::endl;
    std::cout << "|   scheme: " << scheme_name << std::endl;
    std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;

    /*
    Print the size of the true (product) coefficient modulus.
    */
    std::cout << "|   coeff_modulus size: ";
    std::cout << context_data.total_coeff_modulus_bit_count() << " (";
    auto coeff_modulus = context_data.parms().coeff_modulus();
    std::size_t coeff_modulus_size = coeff_modulus.size();
    for (std::size_t i = 0; i < coeff_modulus_size - 1; i++)
    {
        std::cout << coeff_modulus[i].bit_count() << " + ";
    }
    std::cout << coeff_modulus.back().bit_count();
    std::cout << ") bits" << std::endl;

    /*
    For the BFV scheme print the plain_modulus parameter.
    */
    if (context_data.parms().scheme() == seal::scheme_type::bfv)
    {
        std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
    }

    std::cout << "\\" << std::endl;
}

/*
Helper function: Prints the `parms_id' to std::ostream.
*/
inline std::ostream &operator<<(std::ostream &stream, seal::parms_id_type parms_id)
{
    /*
    Save the formatting information for std::cout.
    */
    std::ios old_fmt(nullptr);
    old_fmt.copyfmt(std::cout);

    stream << std::hex << std::setfill('0') << std::setw(16) << parms_id[0] << " " << std::setw(16) << parms_id[1]
           << " " << std::setw(16) << parms_id[2] << " " << std::setw(16) << parms_id[3] << " ";

    /*
    Restore the old std::cout formatting.
    */
    std::cout.copyfmt(old_fmt);

    return stream;
}

/*
Helper function: Prints a vector of floating-point values.
*/
template <typename T>
inline void print_vector(std::vector<T> vec, std::size_t print_size = 4, int prec = 3)
{
    /*
    Save the formatting information for std::cout.
    */
    std::ios old_fmt(nullptr);
    old_fmt.copyfmt(std::cout);

    std::size_t slot_count = vec.size();

    std::cout << std::fixed << std::setprecision(prec);
    std::cout << std::endl;
    if (slot_count <= 2 * print_size)
    {
        std::cout << "    [";
        for (std::size_t i = 0; i < slot_count; i++)
        {
            std::cout << " " << vec[i] << ((i != slot_count - 1) ? "," : " ]\n");
        }
    }
    else
    {
        vec.resize(std::max(vec.size(), 2 * print_size));
        std::cout << "    [";
        for (std::size_t i = 0; i < print_size; i++)
        {
            std::cout << " " << vec[i] << ",";
        }
        if (vec.size() > 2 * print_size)
        {
            std::cout << " ...,";
        }
        for (std::size_t i = slot_count - print_size; i < slot_count; i++)
        {
            std::cout << " " << vec[i] << ((i != slot_count - 1) ? "," : " ]\n");
        }
    }
    std::cout << std::endl;

    /*
    Restore the old std::cout formatting.
    */
    std::cout.copyfmt(old_fmt);
}

/*
Helper function: Prints a matrix of values.
*/
template <typename T>
inline void print_matrix(std::vector<T> matrix, std::size_t row_size)
{
    /*
    We're not going to print every column of the matrix (there are 2048). Instead
    print this many slots from beginning and end of the matrix.
    */
    std::size_t print_size = 5;

    std::cout << std::endl;
    std::cout << "    [";
    for (std::size_t i = 0; i < print_size; i++)
    {
        std::cout << std::setw(3) << std::right << matrix[i] << ",";
    }
    std::cout << std::setw(3) << " ...,";
    for (std::size_t i = row_size - print_size; i < row_size; i++)
    {
        std::cout << std::setw(3) << matrix[i] << ((i != row_size - 1) ? "," : " ]\n");
    }
    std::cout << "    [";
    for (std::size_t i = row_size; i < row_size + print_size; i++)
    {
        std::cout << std::setw(3) << matrix[i] << ",";
    }
    std::cout << std::setw(3) << " ...,";
    for (std::size_t i = 2 * row_size - print_size; i < 2 * row_size; i++)
    {
        std::cout << std::setw(3) << matrix[i] << ((i != 2 * row_size - 1) ? "," : " ]\n");
    }
    std::cout << std::endl;
}

/*
Helper function: Print line number.
*/
inline void print_line(int line_number)
{
    std::cout << "Line " << std::setw(3) << line_number << " --> ";
}

/*
Helper function: Convert a value into a hexadecimal string, e.g., uint64_t(17) --> "11".
*/
inline std::string uint64_to_hex_string(std::uint64_t value)
{
    return seal::util::uint_to_hex_string(&value, std::size_t(1));
}

inline void saveDataLog(std::string file_name, int index_value, int bit_change, float res, int new_file)
{
    std::fstream logFile;
    // Open File
    if (new_file==1){
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::out);
        logFile << "New file: " << std::endl ;
    }
    else{
        logFile.open("/home/mmazz/phd/fhe/sealProfile/"+file_name+".txt", std::ios::app);
        logFile << "Diff: " << res << " index_value: "<< index_value << " bit_changed: " << bit_change << std::endl ;
    }
    //Write data into log file
    logFile << "Diff: :" << res << " index_value: "<< index_value << " bit_changed: " << bit_change << std::endl ;
    // close file stream
    logFile.close();

}


inline float diff_vec(std::vector<double> v1, std::vector<double> v2){
    //vector<double> res(v1.size());
    float res = 0;

    if (v1.size()==v2.size()){
        for (int i=0; i<v1.size(); i++){
            res += abs(v1[i]-v2[i]);
        }
        res = res/v1.size();
    }
    else{
        std::cout << "Vectores de diferente tamaño!!!" << std::endl;
    }
    return res;
}

inline uint64_t bit_flip(uint64_t original, ushort bit){
    uint64_t mask = (1ULL << bit); // I set the bit to flip. 1ULL is for a one of 64bits
    uint64_t res = 0;
    res =  mask^original; // I flip the bit using xor with the mask.
    return res;
}
inline int check_equality(seal::Plaintext &x_plain, seal::Plaintext &x_plain2, int size_x){
    int res=0;
    for(int i=0;i<size_x; i++){
        if(x_plain[i]!=x_plain2[i])
            res+=1;
    }
    if (res == 0)
        std::cout<< "Equality check is: Passed, " << res << std::endl;
    else
        std::cout<< "Equality check is: Failed, " << res << std::endl;
    return res;
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

inline void ntt_transformation(seal::Plaintext &x_plain, size_t coeff_modulus_size, size_t coeff_count, const seal::util::NTTTables* ntt_tables){
    for (size_t i = 0; i < coeff_modulus_size; i++)
    {
        seal::util::ntt_negacyclic_harvey(x_plain.data(i * coeff_count), ntt_tables[i]);
    }
}



