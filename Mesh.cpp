#include "Mesh.h"
#include "similarity.h"
#include "area.h"
#include "serialize.h"

#include <Eigen/Dense>

#define DIVISION_BY_ZERO_THRESHOLD 1e-10

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

    // Some vertices have no center of rotation.
    // So this acts like an offset into the compact
    // matrix of center coords
    this->indexOfCenter.clear();
    std::vector<Eigen::Vector3f> computed;

    // computation cache
    std::vector<Eigen::SparseVector<float>> cacheTriangleWeights;
    std::vector<float> cacheTriangleAreas;

    // fill up the cache
    for (int i = 0; i < GetRestFaceCount(); i++)
    {
        FindTriangleWeight(i, cacheTriangleWeights);
        
        const auto & triangle = triangles.row(i);

        Eigen::Vector3f vertexA = vertices.row(triangle.x());
        Eigen::Vector3f vertexB = vertices.row(triangle.y());
        Eigen::Vector3f vertexC = vertices.row(triangle.z());

        cacheTriangleAreas.push_back(area(vertexA, vertexB, vertexC));
    }
    
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
            auto center = ComputeCenterOfRotation(i, 
                cacheTriangleWeights, cacheTriangleAreas);
            computed.push_back(center);
            this->indexOfCenter.push_back(centerCount);

            centerCount++;
        }
        catch (const std::exception & e)
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

void Mesh::FindTriangleWeight(int triangleIndex,
    std::vector<Eigen::SparseVector<float>> & cacheTriangleWeights)
{
    auto restTriangleCount = GetRestFaceCount();
    
    Eigen::Vector3i triangle = this->triangles.row(triangleIndex);

    Eigen::SparseVector<float> triangleWeight = 
        (weights.col(triangle.x()) + weights.col(triangle.y())
        + weights.col(triangle.z())) / 3;

    cacheTriangleWeights.push_back(triangleWeight);
}

// Find the center of rotation for this vertex and store it into the matrix
// Assumes vertex has more than one bone
Eigen::Vector3f Mesh::ComputeCenterOfRotation(int vertexIndex,
    const std::vector<Eigen::SparseVector<float>> & cacheTriangleWeights,
    const std::vector<float> & cacheTriangleAreas)
{
    // this vertex weight
    Eigen::SparseVector<float> vertexWeight = this->weights.col(vertexIndex);

    // store progress
    Eigen::Vector3f nominator;
    nominator.setZero();
    float denominator = 0;

    // loop on all triangles
    for (int i = 0; i < (int) triangles.rows(); i++)
    {
        auto similarity = ComputeSimilarity(vertexWeight, cacheTriangleWeights[i]);

        const auto & triangle = triangles.row(i);

        Eigen::Vector3f vertexA = vertices.row(triangle.x());
        Eigen::Vector3f vertexB = vertices.row(triangle.y());
        Eigen::Vector3f vertexC = vertices.row(triangle.z());

        auto triangleArea = cacheTriangleAreas[i];

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
        throw std::logic_error(message);
    }
    return nominator / denominator;
}

void Mesh::Serialize(const std::string & path)
{
    try
    {
        SerializeVertices(vertices, path + std::string(".vertices"));
        SerializeTriangles(triangles, path + std::string(".triangles"));
        SerializeWeights(weights, path + std::string(".weights"));

        if (areCentersComputed)
            SerializeVertices(centersOfRotation, path + std::string(".centers"));
    }
    catch(const std::exception& e)
    {
        this->failureContextMessage = e.what();
    }
}

void Mesh::ReadCentersOfRotation(const std::string & path)
{
    // offset indices
    this->indexOfCenter.clear();

    int centerCount = 0;
    for (int i = 0; i < this->weights.cols(); i++)
    {
        if (1 == this->weights.col(i).nonZeros())
        {
            this->indexOfCenter.push_back(-1);    
        }
        else
        {
            this->indexOfCenter.push_back(centerCount);
            centerCount++;
        }
    }
    // read from disk
    try
    {
        auto centers = ReadVertices(path + std::string(".centers"));
        this->centersOfRotation = centers;
    }
    catch(const std::exception& e)
    {
        this->failureContextMessage = e.what();
    }

    areCentersComputed = true;
}

void Mesh::WriteCentersOfRotation(const std::string & path)
{
    if (areCentersComputed)
    {
        SerializeVertices(centersOfRotation, path + std::string(".centers"));
    }
    else
        throw std::runtime_error("Centers are not computed yet");
}

// Assumes normalized quaternions
const Eigen::MatrixXf Mesh::SkinCOR(const std::vector<Eigen::Quaternionf> & rotations, 
    const std::vector<Eigen::Vector3f> & translations)
{
    if (rotations.size() != translations.size())
    {
        std::string message = "Unequal amounts of rotations and translations: ";
        message += std::to_string(rotations.size()) + std::string(" ")
            + std::to_string(translations.size());
        throw std::runtime_error(message);
    }

    Eigen::MatrixXf newVertices(this->vertices.rows(), 3);

    // Get an equivalent of rotations in matrices
    std::vector<Eigen::Matrix3f> matrixRotations;
    matrixRotations.reserve(rotations.size());
    for (auto &&i : rotations)
    {
        matrixRotations.push_back(i.toRotationMatrix());
    }
    
    // for each vertex
    for (int i = 0; i < (int) this->vertices.rows(); i++)
    {
        newVertices.row(i) = DeformVertex(i, rotations, matrixRotations, translations);
    }

    return newVertices;
}

// Runtime algorithm on one vertex
const Eigen::Vector3f Mesh::DeformVertex(int index, 
    const std::vector<Eigen::Quaternionf> & rotations,
    const std::vector<Eigen::Matrix3f> & matrixRotations,
    const std::vector<Eigen::Vector3f> & translations)
{
    Eigen::Vector4f quaternion;
    quaternion.setZero();

    const Eigen::SparseVector<float> & skinWeights = weights.col(index);

    for (Eigen::SparseVector<float>::InnerIterator it(skinWeights); it; ++it)
    {
        auto boneIndex = it.index();
        auto boneWeight = it.value();

        // this weighted quaternion
        Eigen::Vector4f weighted = boneWeight * rotations[boneIndex].coeffs();

        // quaternion arithmetic
        if (quaternion.isZero()) quaternion = weighted;
        else
        {
            auto dot = quaternion.dot(weighted);
            if (dot >= 0) quaternion += weighted;
            else quaternion -= weighted;
        }
    }

    // quaternion summation is turned into a rotation matrix
    quaternion.normalize();
    auto summedQuaternionMatrix = Eigen::Quaternionf(quaternion).toRotationMatrix();
    
    // get LBS estimates
    auto lbs = VertexLBSTransformation(index, matrixRotations, translations);
    const auto & lbsRotation = lbs.first;
    const auto & lbsTranslation = lbs.second;

    // center of rotation
    const auto centerIndex = this->indexOfCenter[index];

    // get COR modified translation
    Eigen::Vector3f finalTranslation;

    // no center case
    if (centerIndex == -1)
    {
        finalTranslation = lbsTranslation;
    }
    else
    {
        const Eigen::Vector3f center = this->centersOfRotation.row(centerIndex);
        finalTranslation = 
            lbsRotation * center
            + lbsTranslation 
            - summedQuaternionMatrix * center
        ;
    }

    // compute vertex position
    const Eigen::Vector3f restPosition = this->vertices.row(index);
    return summedQuaternionMatrix * restPosition + finalTranslation;
}

const std::pair<Eigen::Matrix3f, Eigen::Vector3f> Mesh::VertexLBSTransformation(int index,
    const std::vector<Eigen::Matrix3f> & matrixRotations,
    const std::vector<Eigen::Vector3f> & translations)
{
    const Eigen::SparseVector<float> & skinWeights = weights.col(index);

    // resulting transformations
    Eigen::Matrix3f rotation;
    rotation.setZero();
    Eigen::Vector3f translation;
    translation.setZero();

    for (Eigen::SparseVector<float>::InnerIterator it(skinWeights); it; ++it)
    {
        auto boneIndex = it.index();
        auto boneWeight = it.value();

        rotation += boneWeight * matrixRotations[boneIndex];
        translation += boneWeight * translations[boneIndex];
    }

    return std::make_pair(rotation, translation);
}