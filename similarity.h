#pragma once

#include <Eigen/Sparse>

float ComputeSimilarity(const Eigen::SparseVector<float> & weight1,
    const Eigen::SparseVector<float> & weight2);