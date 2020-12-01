#include "Mesh.h"
#include "similarity.h"

#include <Eigen/Dense>

// Returns the number of centers of rotations, computes them if not done yet
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


// Compute the similarity of skinning weights between 2 vertices
float Mesh::Similarity(int vertexIndex1, int vertexIndex2)
{
    // Check if the vertices belong to the subdivided mesh
    auto count = GetRestVertexCount();
    // both are subdivided
    if (vertexIndex1 > count - 1)
    {
        if (vertexIndex2 > count - 1)
            return ComputeSimilarity(this->subdividedWeights, vertexIndex1 - count,
                this->subdividedWeights, vertexIndex2 - count);
        else
            return ComputeSimilarity(this->subdividedWeights, vertexIndex1 - count,
                this->weights, vertexIndex2);
    }
    else
    {
        if (vertexIndex2 > count - 1)
            return ComputeSimilarity(this->weights, vertexIndex1,
                this->subdividedWeights, vertexIndex2 - count);
        else
            return ComputeSimilarity(this->weights, vertexIndex1,
                this->weights, vertexIndex2);
    }

}