#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <string>

class Mesh
{
private:
    // debug
    bool failedConstruction = false;

    // rest pose
    const Eigen::MatrixXf vertices;
    const Eigen::MatrixXi triangles;

    // skin weights, col is vector of weights for one vertex
    const Eigen::SparseMatrix<float> weights;

    // centers of rotation
    bool areCentersComputed = false;
    Eigen::MatrixXf centersOfRotation;

    // additional subdivision
    // the index of a vertex here omits base vertex count
    Eigen::MatrixXf subdividedVertices;
    // the index of a face here omits base face count
    Eigen::MatrixXi subdividedTriangles;
    // position in this matrix represents indices of all vertices
    Eigen::SparseMatrix<float> subdividedWeights;

    // compute center of rotation for vertex at the given index
    void ComputeCenterOfRotation(int index);

    // weight of a triangle is the average of its vertices
    Eigen::SparseMatrix<float> FindTriangleWeight(int triangleIndex);
    Eigen::SparseMatrix<float> FindVertexWeight(int vertexIndex);

public:
    // debug
    std::string failureContextMessage;

#pragma region
    void ResetFailureMessage()
    {
        failureContextMessage = "";
    }

    int GetRestVertexCount() {return (int) vertices.rows();}
    int GetSubdividedVertexCount() {return (int) subdividedVertices.rows();}
    int GetRestFaceCount() {return (int) triangles.rows();}
    int GetSubdividedFaceCount() {return (int) subdividedTriangles.rows();}

    int GetCenterCount();
    const Eigen::MatrixXf & GetCentersOfRotation();

    void Serialize(const std::string & path);
#pragma endregion

    // null mesh for failed construction
    Mesh(std::string failureMessage)
    {
        failedConstruction = true;
        failureContextMessage = failureMessage;
    }

    Mesh(Eigen::MatrixXf vertices, Eigen::MatrixXi triangles, Eigen::SparseMatrix<float> weights)
        : vertices(vertices), triangles(triangles), weights(weights) 
    {
        Eigen::MatrixXf rotations(vertices.rows(), 3);
        rotations.setZero();
        this->centersOfRotation = rotations;
        this->failureContextMessage = "";
    }
    ~Mesh(){}

    // Compute the centers of rotations and store them in a private field
    void ComputeCentersOfRotation();

    // Compute the similarity value between two vertices, using their bone weights
    float Similarity(int vertexIndex1, int vertexIndex2);

    // Compute the skinning weight distance between two vertices: norm(wi - wj)
    float SkinningWeightDistance(int vertexIndex1, int vertexIndex2);

    // Subdivide the mesh to get skinning weight distances under a threshold for all triangles
    void SubdivideTriangles(float threshold);
};