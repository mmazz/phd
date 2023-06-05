// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

int main()
{
    int loop_size = 200;
    int poly_degree = 4;
    int poly_modulus_degree=16384;
    int size_vector = 500;
    ////////////////////////////////    struct rusage usage;
    int slot_count = poly_modulus_degree/2;
    std::vector<double> input;
    input.reserve(slot_count);
    double curr_point = 0;
    double step_size = 1.5 / (slot_count - 1);
    for (size_t i = 0; i < size_vector; i++)
    {
        input.push_back(curr_point);
        curr_point += step_size;
    }
    for(int i=0; i<loop_size; i++){
        std::vector<double> true_result;
        true_result.reserve(slot_count);
            for (size_t i = 0; i < size_vector; i++)
            {
                double x = input[i];
                true_result[i] = pow(x, poly_degree);
            }
    }
    return 0;
}
