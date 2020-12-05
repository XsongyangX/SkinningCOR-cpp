#define CENTER_OF_ROTATION_EXPORTS

#include "center_of_rotation_api.h"
#include "Mesh.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <vector>
// #include <fstream>
#include <sstream>
#include <string.h>

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
    // std::ofstream logFile;
    // logFile.open("C:/Users/Song/Documents/UDEM/ift6113/project/skinning_cor/logs/center_of_rotation.log");
    // logFile << "Vertex count: " << vertexCount << std::endl;

    for (int i = 0; i < vertexCount; i++)
    {
        verts.row(i) << vertices[0], vertices[1], vertices[2];

        // increment pointer
        vertices += 3; // the pointer points to a struct of 3 floats
    }

    // // debug
    // logFile << verts.topRows(5) << std::endl
    //         << std::endl;

    // faces
    Eigen::MatrixXi faces(triangleCount, 3);

    // // debug
    // logFile << "Triangle count: " << triangleCount << std::endl;

    for (int i = 0; i < triangleCount; i++)
    {   
        // check for inconsistent triangles
        if (triangles[0] == triangles[1] || triangles[0] == triangles[2] 
            || triangles[1] == triangles[2])
        {
            // return a null mesh, a mesh object with a null flag
            std::stringstream sstm;
            sstm << "Triangle " << i << " has degenerate vertices: " <<
                triangles[0] << triangles[1] << triangles[2] << std::endl;
            auto message = sstm.str();
            return new Mesh(message);
        }

        faces.row(i) << triangles[0], triangles[1], triangles[2];

        // increment pointer
        triangles += 3; // points to a struct of 3 int
    }

    // // debug
    // logFile << faces.topRows(5) << std::endl
    //         << std::endl;

    // bone weights, one column of weights per vertex
    Eigen::SparseMatrix<float> boneWeights(boneCount, vertexCount);

    // // debug
    // logFile << "Bone count: " << boneCount << std::endl;

    std::vector<Eigen::Triplet<float>> triplets;

    // for each vertex
    int index = 0;
    for (int i = 0; i < vertexCount; i++)
    {
        uint8_t bonesForThisVertex = bones[i];

        // // debug
        // if (i < 5)
        //     logFile << "This vertex has this many bones:" << int(bonesForThisVertex) << std::endl;

        // for each bone on this vertex
        for (int j = 0; j < bonesForThisVertex; j++)
        {
            BoneWeight bone;
            bone = weights[index];
            triplets.push_back(Eigen::Triplet<float>(bone.boneIndex, i, bone.weight));

            // // debug
            // if (i < 5)
            //     logFile << "Triplet: " << bone.boneIndex << ", " << i << ", " << bone.weight << std::endl;

            index++;
        }
    }

    // // debug
    // logFile << "Right before setting from triplets" << std::endl;

    boneWeights.setFromTriplets(triplets.begin(), triplets.end());

    // logFile << std::endl;

    // logFile.close();

    return new Mesh(verts, faces, boneWeights);
}

// empty string means no error
const char* GetFailureMessage(Mesh* mesh)
{
    auto error = mesh->failureContextMessage;
    auto length = std::strlen(error.c_str()) + 1;
    auto message = new char[length];

#ifdef __GNUC__
    strcpy(message, error.c_str());
#else
    strcpy_s(message, length, error.c_str());
#endif

    // reset failure message in mesh to empty string
    mesh->ResetFailureMessage();

    return message;
}

CENTER_OF_ROTATION_API const char* HasFailedMeshConstruction(Mesh * mesh)
{
    return GetFailureMessage(mesh);
}

CENTER_OF_ROTATION_API void FreeErrorMessage(const char* message)
{
    delete[] message;
}

CENTER_OF_ROTATION_API void DestroyMesh(Mesh *mesh)
{
    delete mesh;
}

// debug
CENTER_OF_ROTATION_API int GetRestVertexCount(Mesh *mesh)
{
    return mesh->GetRestVertexCount();
}
CENTER_OF_ROTATION_API int GetRestFaceCount(Mesh *mesh)
{
    return mesh->GetRestFaceCount();
}
// CENTER_OF_ROTATION_API int GetSubdividedVertexCount(Mesh *mesh)
// {
//     return mesh->GetSubdividedVertexCount();
// }
// CENTER_OF_ROTATION_API int GetSubdividedFaceCount(Mesh *mesh)
// {
//     return mesh->GetSubdividedFaceCount();
// }

// get centers of rotation
CENTER_OF_ROTATION_API int GetCenterCount(Mesh * mesh)
{
    return mesh->GetCenterCount();
}

// vertices pointer should point to an allocated Vector3[] in C#
CENTER_OF_ROTATION_API void GetCentersOfRotation(Mesh * mesh, 
    float * vertices, int vertexCount)
{
    const auto& centers = mesh->GetCentersOfRotation();

    for (int i = 0; i < vertexCount; i++)
    {
        const auto& point = centers.row(i);
        *vertices = point.x();
        *(vertices + 1) = point.y();
        *(vertices + 2) = point.z();

        vertices += 3; // next struct of 3 floats
    }
    
}

// Empty string means success
CENTER_OF_ROTATION_API const char * HasFailedGettingCentersOfRotation(Mesh * mesh)
{
    return GetFailureMessage(mesh);
}

// Serialization
CENTER_OF_ROTATION_API void SerializeMesh(Mesh * mesh, const char * path)
{
    mesh->Serialize(std::string(path));
}

CENTER_OF_ROTATION_API void ReadCenters(Mesh * mesh, const char * path)
{
    mesh->ReadCentersOfRotation(path);
}

CENTER_OF_ROTATION_API void SerializeCenters(Mesh * mesh, const char * path)
{
    mesh->WriteCentersOfRotation(path);
}

CENTER_OF_ROTATION_API const char * SerializationError(Mesh * mesh)
{
    return GetFailureMessage(mesh);
}