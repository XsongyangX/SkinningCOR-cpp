#pragma once

#ifdef CENTER_OF_ROTATION_EXPORTS
#define CENTER_OF_ROTATION_API __declspec(dllexport)
#else

#ifdef CENTER_OF_ROTATION_DEBUG
#define CENTER_OF_ROTATION_API
#else
#define CENTER_OF_ROTATION_API __declspec(dllimport)
#endif

#endif

#include "Mesh.h"

// Struct of a pair of bone and its weight
typedef struct __bone_weight_pair
{
    int index;
    float weight;
} BoneWeight;

// Export interface
extern "C"
{
    // creates a mesh with cpp
    CENTER_OF_ROTATION_API Mesh* CreateMesh(float vertices[][3], int vertexCount,
        int triangles[][3], int triangleCount,
        BoneWeight* weights[], int maxBonePerVertex, int boneCount);

    CENTER_OF_ROTATION_API void DestroyMesh(Mesh * mesh);
}

