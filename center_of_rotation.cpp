#define CENTER_OF_ROTATION_EXPORTS

#include "center_of_rotation.h"
#include "Mesh.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <vector>
// #include <iostream>

/// Creates a mesh in cpp
/// The bones parameter is an array of length equal 
/// the vertex count. Each element represents the
/// number of bones influencing the vertex of this
/// index.
CENTER_OF_ROTATION_API Mesh *CreateMesh(
    float *vertices, int vertexCount,
    int *triangles, int triangleCount,
    BoneWeight *weights, uint8_t *bones, int boneCount)
{
    Eigen::MatrixXf verts(vertexCount, 3);

    Eigen::MatrixXi faces(triangleCount, 3);

    Eigen::SparseMatrix<float> boneWeights(vertexCount, boneCount);

    return new Mesh(verts, faces, boneWeights);
}

CENTER_OF_ROTATION_API void DestroyMesh(Mesh *mesh)
{
    delete mesh;
}