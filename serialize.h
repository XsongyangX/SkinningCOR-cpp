#pragma once

#include "Mesh.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <string>

// serialize eigen matrices to text
void SerializeVertices(const Eigen::MatrixXf & vertices, const std::string & path);
void SerializeTriangles(const Eigen::MatrixXi & triangles, const std::string & path);
void SerializeWeights(const Eigen::SparseMatrix<float> & weights, const std::string & path);

// read matrces from text
Eigen::MatrixXf ReadVertices(const std::string & path);
Eigen::MatrixXi ReadTriangles(const std::string & path);
Eigen::SparseMatrix<float> ReadWeights(const std::string & path, int rows, int cols);

// allocated with new
Mesh* ReadMesh(const std::string & path);