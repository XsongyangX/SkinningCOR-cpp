#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
class Mesh
{
private:
    // rest pose
    const Eigen::MatrixXf vertices;
    const Eigen::MatrixXi triangles;

    // skin weights
    const Eigen::SparseMatrix<float> weights;

    // centers of rotation
    const Eigen::MatrixXf centersOfRotation;
public:
    Mesh(Eigen::MatrixXf vertices, Eigen::MatrixXi triangles, Eigen::SparseMatrix<float> weights)
        : vertices(vertices), triangles(triangles), weights(weights) {}
    ~Mesh(){}
};
