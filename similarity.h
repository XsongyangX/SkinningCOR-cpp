#pragma once

#include <Eigen/Sparse>

float ComputeSimilarity(const Eigen::SparseMatrix<float> & matrix1, int rowIndex1,
    const Eigen::SparseMatrix<float> & matrix2, int rowIndex2);