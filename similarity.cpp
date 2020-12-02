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
float ComputeSimilarity(const Eigen::SparseMatrix<float> & matrix1, int colIndex1,
    const Eigen::SparseMatrix<float> & matrix2, int colIndex2)
{
    // // debug
    // std::cout << "Matrix 1" << matrix1 << std::endl << std::endl;
    // std::cout << "Matrix 2" << matrix2 << std::endl << std::endl;

    // similarity value
    float similarity = 0;

    // iterate over a vertex col
    for (Eigen::SparseMatrix<float, Eigen::ColMajor>::InnerIterator it1(matrix1,colIndex1);
        it1; ++it1)
    {
        // iterate over the other vertex col
        for (Eigen::SparseMatrix<float, Eigen::ColMajor>::InnerIterator it2(matrix2,colIndex2);
            it2; ++it2)
        {
            // skip if both iterators are pointing to the same bone
            if (it1.row() == it2.row()) continue;

            auto j = it1.row();
            auto k = it2.row();
            auto w1_j = it1.value();
            auto w1_k = matrix1.coeff(k, colIndex1); // O(log n)
            auto w2_j = matrix2.coeff(j, colIndex2); // O(log n)
            auto w2_k = it2.value();

            auto coef = w1_j * w1_k * w2_j * w2_k;

            // exponential part
            auto difference = w1_j * w2_k - w1_k * w2_j;
            similarity += coef * exp(- difference * difference / KERNEL_WIDTH * KERNEL_WIDTH);
        }
    }
    return similarity;
}

float ComputeVertexFaceSimilarity(const Eigen::SparseMatrix<float> & vertexWeight,
    const Eigen::SparseMatrix<float> & triangleWeight)
{
    return ComputeSimilarity(vertexWeight, 0, triangleWeight, 0);
}