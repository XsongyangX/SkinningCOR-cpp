#include "similarity.h"

#include <Eigen/Sparse>
#include <cmath>

// #include <iostream>

#ifndef KERNEL_WIDTH
#define KERNEL_WIDTH 1
#endif

// The similarity between two weight vectors is a float.
// s(w1,w2) = sum_over_all_different_jk w1j * w1k * w2j * w2k 
//      * exp(-1/kernel_width^2 * (w1j*w2k - w1k*w2j)^2)
float ComputeSimilarity(const Eigen::SparseVector<float> & weight1,
    const Eigen::SparseVector<float> & weight2)
{
    float similarity = 0;

    for (Eigen::SparseVector<float>::InnerIterator it1(weight1); it1; ++it1)
        for (Eigen::SparseVector<float>::InnerIterator it2(weight2); it2; ++it2)
        {
            if (it1.index() == it2.index()) continue; // same bone

            auto j = it1.index();
            auto k = it2.index();
            auto w1_j = it1.value();
            auto w1_k = weight1.coeff(k); // O(log n)
            auto w2_j = weight2.coeff(k); // O(log n)
            auto w2_k = it2.value();

            auto coef = w1_j * w1_k * w2_j * w2_k;

            // exponential part
            auto difference = w1_j * w2_k - w1_k * w2_j;
            similarity += coef * exp(- difference * difference / KERNEL_WIDTH * KERNEL_WIDTH);
        }


    return similarity;
}