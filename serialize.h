#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <string>

// serialize eigen matrices to csv
void SerializeVertices(const Eigen::MatrixXf & vertices, const std::string & path);
void SerializeTriangles(const Eigen::MatrixXi & triangles, const std::string & path);
void SerializeWeights(const Eigen::SparseMatrix<float> & weights, const std::string & path);
