#include "Mesh.h"
#include "similarity.h"
#include "area.h"
#include "serialize.h"

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
    int centerCount = 0;
    this->indexOfCenter.clear();
    std::vector<Eigen::Vector3f> computed;

    // may distribute this into threads
    for (int i = 0; i < (int) vertices.rows(); i++)
    {
        try {
            // check if vertex has only one bone
            if (1 == this->weights.col(i).nonZeros())
            {
                this->indexOfCenter.push_back(-1);
                continue;
            }
            auto center = ComputeCenterOfRotation(i);
            computed.push_back(center);
            this->indexOfCenter.push_back(centerCount);

            centerCount++;
        }
        catch (std::exception e)
        {
            this->failureContextMessage = e.what();
            
            return;
        }
    }

    Eigen::MatrixXf centers(centerCount, 3);
    for (int i = 0; i < centerCount; i++)
    {
        centers.row(i) = computed[i];
    }
    this->centersOfRotation = centers;

    areCentersComputed = true;
}

// Compute the similarity of skinning weights between a vertex and a triangle
float Mesh::Similarity(int vertexIndex, int triangleIndex)
{

    auto vertexWeight = FindVertexWeight(vertexIndex);
    auto triangleWeight = FindTriangleWeight(triangleIndex);

    return ComputeSimilarity(vertexWeight, triangleWeight);
}

const Eigen::SparseVector<float> Mesh::FindVertexWeight(int vertexIndex)
{
    // Check if the vertices belong to the subdivided mesh
    auto restVertexCount = GetRestVertexCount();

    Eigen::SparseVector<float> vertexWeight;

    // check if the vertex belongs to a subdivision
    if (vertexIndex > restVertexCount - 1)
        vertexWeight = this->subdividedWeights.col(vertexIndex - restVertexCount);
    else
        vertexWeight = this->weights.col(vertexIndex);

    return vertexWeight;
}

const Eigen::SparseVector<float> Mesh::FindTriangleWeight(int triangleIndex)
{
    auto restTriangleCount = GetRestFaceCount();
    
    Eigen::Vector3i triangle;

    // check if the triangle is subdivided
    if (triangleIndex > restTriangleCount - 1)
        triangle = this->subdividedTriangles.row(triangleIndex - restTriangleCount);
    else
        triangle = this->triangles.row(triangleIndex);

    Eigen::SparseVector<float> triangleWeight = 
        (FindVertexWeight(triangle.x()) + FindVertexWeight(triangle.y())
        + FindVertexWeight(triangle.z())) / 3;

    return triangleWeight;
}

// Find the center of rotation for this vertex and store it into the matrix
// Assumes vertex has more than one bone
Eigen::Vector3f Mesh::ComputeCenterOfRotation(int vertexIndex)
{
    // check if this vertex has only one bone influence
    // if so, there is no center of rotation
    auto boneCount = this->weights.col(vertexIndex).nonZeros();
    if (boneCount == 1)
    {
        return Eigen::Vector3f();
    }

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
    return nominator / denominator;
}

void Mesh::Serialize(const std::string & path)
{
    SerializeVertices(vertices, path + std::string(".vertices"));
    SerializeTriangles(triangles, path + std::string(".triangles"));
    SerializeWeights(weights, path + std::string(".weights"));

    if (areCentersComputed)
        SerializeVertices(centersOfRotation, path + std::string(".centers"));
}