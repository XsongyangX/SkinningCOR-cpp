#define CENTER_OF_ROTATION_EXPORTS

#include "center_of_rotation.h"
#include "Mesh.h"
#include "exception_utils.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <vector>
#include <fstream>
#include <exception>

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
    // vertices
    Eigen::MatrixXf verts(vertexCount, 3);

    // debug
    std::ofstream logFile;
    logFile.open("C:/Users/Song/Documents/UDEM/ift6113/project/skinning_cor/logs/center_of_rotation.log");
    logFile << "Vertex count: " << vertexCount << std::endl;
    
    for (int i = 0; i < vertexCount; i++)
    {
        verts.row(i) << vertices[0], vertices[1], vertices[2];
        
        // increment pointer
        vertices += 3; // the pointer points to a struct of 3 floats
    }
    
    // debug
    logFile << verts.topRows(5) << std::endl << std::endl;

    // faces
    Eigen::MatrixXi faces(triangleCount, 3);

    // debug
    logFile << "Triangle count: " << triangleCount << std::endl;

    for (int i = 0; i < triangleCount; i++)
    {
        faces.row(i) << triangles[0], triangles[1], triangles[2];

        // increment pointer
        triangles += 3; // points to a struct of 3 int
    }
    
    // debug
    logFile << faces.topRows(5) << std::endl << std::endl;

    // bone weights
    Eigen::SparseMatrix<float> boneWeights(vertexCount, boneCount);

    // debug
    logFile << "Bone count: " << boneCount << std::endl;

    std::vector<Eigen::Triplet<float>> triplets;

    // for each vertex
    int index = 0;
    for (int i = 0; i < vertexCount; i++)
    {
        uint8_t bonesForThisVertex = bones[i];

        // debug
        if (i < 5)
            logFile << "This vertex has this many bones:" << int(bonesForThisVertex) << std::endl;

        // for each bone on this vertex
        for (int j = 0; j < bonesForThisVertex; j++)
        {
            // debug
            std::exception_ptr eptr;
            BoneWeight bone;
            try
            {
                bone = weights[index];
            }
            catch (...)
            {
                eptr = std::current_exception();
            }
            handle_eptr(eptr, logFile);

            triplets.push_back(Eigen::Triplet<float>(i, bone.boneIndex, bone.weight));

            if (i < 5)
                logFile << "Triplet: " << i << ", " << bone.boneIndex << ", " << bone.weight << std::endl;

            index++;
        }
    }
    
    // debug
    logFile << "Right before setting from triplets" << std::endl;

    boneWeights.setFromTriplets(triplets.begin(), triplets.end());
    
    logFile << std::endl;

    logFile.close();

    return new Mesh(verts, faces, boneWeights);
}

CENTER_OF_ROTATION_API void DestroyMesh(Mesh *mesh)
{
    delete mesh;
}