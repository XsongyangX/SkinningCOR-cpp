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

typedef struct {
    int boneIndex;
    float weight;
} BoneWeight;

// Export interface
extern "C"
{
    // creates a mesh with cpp
    CENTER_OF_ROTATION_API Mesh* CreateMesh(
        float* vertices, int vertexCount,
        int* triangles, int triangleCount,
        BoneWeight* weights, uint8_t* bones,
        int boneCount
        );

    CENTER_OF_ROTATION_API void DestroyMesh(Mesh * mesh);
}

