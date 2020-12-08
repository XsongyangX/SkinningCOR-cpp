#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Geometry>
#include <string>

class Mesh
{
private:

    // rest pose
    const Eigen::MatrixXf vertices;
    const Eigen::MatrixXi triangles;

    // skin weights, col is vector of weights for one vertex
    const Eigen::SparseMatrix<float> weights;

    // centers of rotation
    bool areCentersComputed = false;
    // -1 if the vertex has no center of rotation
    std::vector<int> indexOfCenter;
    Eigen::MatrixXf centersOfRotation;

    // // additional subdivision
    // // the index of a vertex here omits base vertex count
    // Eigen::MatrixXf subdividedVertices;
    // // the index of a face here omits base face count
    // Eigen::MatrixXi subdividedTriangles;
    // // position in this matrix represents indices of all vertices
    // Eigen::SparseMatrix<float> subdividedWeights;

    // compute center of rotation for vertex at the given index
    // carry a cache to hasten computations
    Eigen::Vector3f ComputeCenterOfRotation(int index,
        const std::vector<Eigen::SparseVector<float>> & cacheTriangleWeights,
        const std::vector<float> & cacheTriangleAreas);

    // weight of a triangle is the average of its vertices
    void FindTriangleWeight(int triangleIndex,
        std::vector<Eigen::SparseVector<float>> & cacheTriangleWeights);

    // Runtime algorithm on one vertex
    const Eigen::Vector3f DeformVertex(int index, 
        const std::vector<Eigen::Quaternionf> & rotations,
        const std::vector<Eigen::Matrix3f> & matrixRotations,
        const std::vector<Eigen::Vector3f> & translations);

    const std::pair<Eigen::Matrix3f, Eigen::Vector3f> VertexLBSTransformation(int index,
        const std::vector<Eigen::Matrix3f> & matrixRotations,
        const std::vector<Eigen::Vector3f> & translations);

public:
    
#pragma region
    // debug
    std::string failureContextMessage;

    void ResetFailureMessage()
    {
        failureContextMessage = "";
    }

    const Eigen::MatrixXf & GetVertices() const {return vertices;}
    const Eigen::MatrixXi & GetFaces() const {return triangles;}

    int GetRestVertexCount() {return (int) vertices.rows();}
    // int GetSubdividedVertexCount() {return (int) subdividedVertices.rows();}
    int GetRestFaceCount() {return (int) triangles.rows();}
    // int GetSubdividedFaceCount() {return (int) subdividedTriangles.rows();}

    int GetCenterCount();
    const Eigen::MatrixXf & GetCentersOfRotation();

    int GetBoneCount() {return (int) weights.rows();}

    void Serialize(const std::string & path);
    // Read from disk
    void ReadCentersOfRotation(const std::string & path);
    // Write to disk
    void WriteCentersOfRotation(const std::string & path);
#pragma endregion

    // null mesh for failed construction
    Mesh(std::string failureMessage)
    {
        failureContextMessage = failureMessage;
    }

    Mesh(Eigen::MatrixXf vertices, Eigen::MatrixXi triangles, Eigen::SparseMatrix<float> weights)
        : vertices(vertices), triangles(triangles), weights(weights) 
    {
        this->failureContextMessage = "";
    }
    ~Mesh(){}

    // Compute the centers of rotations and store them in a private field
    void ComputeCentersOfRotation();

    // additional subdivision
    // // Compute the skinning weight distance between two vertices: norm(wi - wj)
    // float SkinningWeightDistance(int vertexIndex1, int vertexIndex2);

    // // Subdivide the mesh to get skinning weight distances under a threshold for all triangles
    // void SubdivideTriangles(float threshold);

    // in case performance is an issue
    // void SetMeshVertexBuffer(void * );

    // runtime algorithm
    const Eigen::MatrixXf SkinCOR(const std::vector<Eigen::Quaternionf> & rotations,
        const std::vector<Eigen::Vector3f> & translations);
};