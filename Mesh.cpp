#include "Mesh.h"

#include <Eigen/Dense>

int Mesh::GetCenterCount()
{
    if (!areCentersComputed)
    {
        ComputeCentersOfRotation();
    }
    return (int)centersOfRotation.rows();
}

const Eigen::MatrixXf &Mesh::GetCentersOfRotation()
{
    if (!areCentersComputed)
    {
        ComputeCentersOfRotation();
    }
    return centersOfRotation;
}

// Compute COR according to the paper
void Mesh::ComputeCentersOfRotation()
{
    areCentersComputed = true;

    // debug
    Eigen::MatrixXf centers(3, 3);
    centers << 1, 2, 3, 4, 5, 6, 7, 8, 9;
    this->centersOfRotation = centers;
}