//
//
//  main.cpp - sample program to test correctness and performance of fast median filter.
//
//  Project: Fast median filter
//  Author: S.Zabinskis
//
//  May, 2011
//
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
//
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
#include "medflt.h"


// very primitive initialization of 1d array - uniformly distributed values from [a,b]
void init_input1(std::vector<int>& input, int size, int a, int b)
{
    for(int j=0; j<size; j++)
        input.push_back(a+rand()%(b-a+1));
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// 'pepper and salt' like initialization of 1d array
// array is initialized by constant "value"
// noise_p - probability that array element value should be "noised"
// ampl - noise amplitude

void init_input2(std::vector<int>& input, int size, int value, int noise_p, int ampl)
{
    for(int j=0; j<size; j++)
    {
        int x = rand() % 100;
        if(x <= noise_p)
            input.push_back(value+(rand()%ampl)-ampl/2);
        else
            input.push_back(value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// perform N trials of fast median filtering.

void fast_run(int N, const std::vector<int>& input, int window_size)
{
    TMedianFilter1D<int> flt;
    flt.SetWindowSize(window_size);
    while(N-- > 0)
        flt.Execute(input, true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// perform N trials of simple median filtering (based on median definition).

void direct_run(int N, const std::vector<int>& input, int window_size)
{
    std::vector<int> output_direct;
    while(N-- > 0)
    {
        direct_median<int>(output_direct, input, window_size);
        output_direct.clear();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// here we perform N trials for both fast and simple median filters
// results are compared and if they differ - procedure immediately exits with failure

bool test_result(int N, int sample_size, int window_size)
{
    std::vector<int> input;
    std::vector<int> output_fast;
    std::vector<int> output_direct;

    // create median filter
    TMedianFilter1D<int> flt;
    flt.SetWindowSize(window_size);


    while(N-- > 0)
    {
        input.clear();
        // uncomment one of initializations
        //init_input1(input, M, 1, 10);
        init_input2(input, sample_size, 100, 20, 40);

        //std::cout << "Input:\n";
        //for(int j=0; j<input.size(); j++)
        //	std::cout << std::setw(6) << input[j];
        //std::cout << std::endl;


        direct_median<int>(output_direct, input, window_size);

        flt.Execute(input, true);

        int diff_count = 0;
        for(int j=0; j<flt.Count(); j++)
        {
            diff_count += flt[j] != output_direct[j] ? 1 : 0;
        }
        if(diff_count>0)
        {
            std::cout << "Results differ!\n";
            std::cout << "Fast:\n";
            for(int j=0; j<flt.Count(); j++)
                std::cout << flt[j] << "  ";
            std::cout << std::endl;

            std::cout << "Direct:\n";
            for(int j=0; j<output_direct.size(); j++)
                std::cout << output_direct[j] << "  ";
            std::cout << std::endl;
            // results differ - exit with failure
            return false;
        }
        output_direct.clear();
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    srand(time(0));

    int sample_size = 1000;
    int window_size = 7;

    // 1000 times run test
    // here we apply simple and fast median filters to the same input array
    // test fails if outputs are different
    if(!test_result(1000, sample_size, window_size))
    {
        std::cout << "Test failed.\n";
        return -1;
    }


    // simple performance measurement
    // when window_size = 3 - both fast and simple median filters produce the same performance parameters
    // when window_size become greater - fast median filter gives better performance.

    // initialize input array
    std::vector<int> input;
    init_input2(input, sample_size, 100, 20, 40);

    // number of trials
    int N = 1000000;

    int t1 = (int)time(0);
    // N times run simple median filter (based on median definition)
    direct_run(N, input, window_size);
    std::cout << "Direct median filter: " << (int)time(0)-t1 << "(s)\n";


    t1 = (int)time(0);
    // N times run fast median filter (based on Huang algorithm)
    fast_run(N, input, window_size);
    std::cout << "Fast median filter: " << (int)time(0)-t1 << "(s)\n";

    return 0;
}
