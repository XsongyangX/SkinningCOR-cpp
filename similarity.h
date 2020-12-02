#pragma once

#include <Eigen/Sparse>

float ComputeSimilarity(const Eigen::SparseMatrix<float> & matrix1, int colIndex1,
    const Eigen::SparseMatrix<float> & matrix2, int colIndex2);

// All matrices have one column
float ComputeVertexFaceSimilarity(const Eigen::SparseMatrix<float> & vertexWeight,
    const Eigen::SparseMatrix<float> & triangleWeight);