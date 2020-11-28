#define CENTER_OF_ROTATION_EXPORTS

#include "center_of_rotation.h"
#include "Mesh.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <vector>
// #include <iostream>

// creates a mesh with cpp
CENTER_OF_ROTATION_API Mesh* CreateMesh(float vertices[][3], int vertexCount,
    int triangles[][3], int triangleCount,
    BoneWeight* weights[], int maxBonePerVertex, int boneCount)
{
    // fill up a vertex position matrix
    Eigen::MatrixXf verts(vertexCount, 3);

    for (int i = 0; i < vertexCount; i++)
    {
        verts.row(i) << vertices[i][0] , vertices[i][1] , vertices[i][2];
    }

    // fill up a matrix of faces
    Eigen::MatrixXi faces(triangleCount, 3);

    for (int i = 0; i < triangleCount; i++)
    {
        faces.row(i) << triangles[i][0], triangles[i][1], triangles[i][2];
    }

    // fill up bone weights
    Eigen::SparseMatrix<float> boneWeights(vertexCount, boneCount);

    std::vector<Eigen::Triplet<float>> triplets;

    for (int i = 0; i < vertexCount; i++)
    {
        BoneWeight* weight = weights[i];
        for (int j = 0; j < maxBonePerVertex; j++)
        {
            auto boneIndex = weight->index;
            auto boneWeight = weight->weight;
            if (boneWeight != 0)
            {
                triplets.push_back(Eigen::Triplet<float>(i, boneIndex, boneWeight));
            }
            weight++;
        }
    }
    
    boneWeights.setFromTriplets(triplets.begin(), triplets.end());

    // std::cout << verts << std::endl;
    // std::cout << faces << std::endl;
    // std::cout << boneWeights << std::endl;

    // manual memory management
    return new Mesh(verts, faces, boneWeights);
}

CENTER_OF_ROTATION_API void DestroyMesh(Mesh* mesh)
{
    delete mesh;
}