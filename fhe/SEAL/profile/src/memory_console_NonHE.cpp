// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include "examples.h"
#include <sys/resource.h>


using namespace seal;
using namespace std;
/*
     +----------------------------------------------------+
        | poly_modulus_degree | max coeff_modulus bit-length |
        +---------------------+------------------------------+
        | 1024                | 27                           |
        | 2048                | 54                           |
        | 4096                | 109                          |
        | 8192                | 218                          |
        | 16384               | 438                          |
        | 32768               | 881                          |
        +---------------------+------------------------------+
*/
int main()
{
    int poly_degree = 4;
    int poly_modulus_degree = 16384;
    int size_vector = 500;

    ////////////////////////////////    struct rusage usage;
    struct rusage usage_non;
    long int start_non, end_non;
    getrusage(RUSAGE_SELF, &usage_non);
    start_non = usage_non.ru_maxrss;

    size_t slot_count = poly_modulus_degree/2;
    vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (static_cast<double>(slot_count) - 1);
    for (size_t i = 0; i < size_vector; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }

    int N = input.size();
    vector<double> true_result(N);
    for (size_t i = 0; i < size_vector; i++)
    {
        double x = input[i];
        true_result[i] = pow(x,poly_degree);
    }
    getrusage(RUSAGE_SELF, &usage_non);
    end_non = usage_non.ru_maxrss;


    cout <<   start_non << " " << end_non <<endl;
    return 0;
}
