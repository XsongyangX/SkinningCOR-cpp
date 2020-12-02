#include "Mesh.h"
#include "similarity.h"
#include "area.h"

#include <Eigen/Dense>

#define DIVISION_BY_ZERO_THRESHOLD 1e-5

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

    // may distribute this into threads
    for (int i = 0; i < (int) vertices.rows(); i++)
    {
        try {
            ComputeCenterOfRotation(i);
        }
        catch (std::exception e)
        {
            this->failureContextMessage = e.what();
            return;
        }
    }
    
}

// Compute the similarity of skinning weights between a vertex and a triangle
float Mesh::Similarity(int vertexIndex, int triangleIndex)
{

    Eigen::SparseMatrix<float> vertexWeight = FindVertexWeight(vertexIndex);
    Eigen::SparseMatrix<float> triangleWeight = FindTriangleWeight(triangleIndex);

    return ComputeVertexFaceSimilarity(vertexWeight, triangleWeight);
}

Eigen::SparseMatrix<float> Mesh::FindVertexWeight(int vertexIndex)
{
    // Check if the vertices belong to the subdivided mesh
    auto restVertexCount = GetRestVertexCount();

    Eigen::SparseMatrix<float> vertexWeight;

    // check if the vertex belongs to a subdivision
    if (vertexIndex > restVertexCount - 1)
        vertexWeight = this->subdividedWeights.col(vertexIndex - restVertexCount);
    else
        vertexWeight = this->weights.col(vertexIndex);

    return vertexWeight;
}

Eigen::SparseMatrix<float> Mesh::FindTriangleWeight(int triangleIndex)
{
    auto restTriangleCount = GetRestFaceCount();
    
    Eigen::Vector3i triangle;

    // check if the triangle is subdivided
    if (triangleIndex > restTriangleCount - 1)
        triangle = this->subdividedTriangles.row(triangleIndex - restTriangleCount);
    else
        triangle = this->triangles.row(triangleIndex);

    Eigen::SparseMatrix<float> triangleWeight = 
        (FindVertexWeight(triangle.x()) + FindVertexWeight(triangle.y())
        + FindVertexWeight(triangle.z())) / 3;

    return triangleWeight;
}

// Find the center of rotation for this vertex and store it into the matrix
void Mesh::ComputeCenterOfRotation(int vertexIndex)
{
    // store progress
    Eigen::Vector3f nominator;
    nominator.setZero();
    float denominator = 0;

    // loop on all triangles
    for (int i = 0; i < (int) triangles.rows(); i++)
    {
        auto similarity = Similarity(vertexIndex, i);

        const auto & triangle = triangles.row(i);

        Eigen::Vector3f vertexA = vertices.row(triangle.x());
        Eigen::Vector3f vertexB = vertices.row(triangle.y());
        Eigen::Vector3f vertexC = vertices.row(triangle.z());

        auto triangleArea = area(vertexA, vertexB, vertexC);

        nominator += similarity * (vertexA + vertexB + vertexC) / 3 * triangleArea;
        denominator += similarity * triangleArea;
    }

    if (denominator < DIVISION_BY_ZERO_THRESHOLD)
    {
        auto message = std::string("Denominator is close to zero for vertex: ") 
            + std::to_string(vertexIndex) + std::string("; threshold = ")
            + std::to_string(DIVISION_BY_ZERO_THRESHOLD)
            + std::string("; value found = ") 
            + std::to_string(denominator);
        throw std::exception(message.c_str());
    }
    this->centersOfRotation.row(vertexIndex) = nominator / denominator;
}