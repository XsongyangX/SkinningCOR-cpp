#pragma once

#ifdef CENTER_OF_ROTATION_EXPORTS
    #ifdef __GNUC__
        #define CENTER_OF_ROTATION_API __attribute__((visibility("default")))
    #else
        #define CENTER_OF_ROTATION_API __declspec(dllexport)
    #endif
#else

#ifdef CENTER_OF_ROTATION_DEBUG
    #define CENTER_OF_ROTATION_API
#else
    #ifdef __GNUC__
        #define CENTER_OF_ROTATION_API
    #else
        #define CENTER_OF_ROTATION_API __declspec(dllimport)
    #endif
#endif

#endif

#include "Mesh.h"

typedef struct _boneWeight {
    int boneIndex;
    float weight;
} BoneWeight;

// Quaternion floats + translation floats
typedef struct _boneRotation {
    float quaternionX;
    float quaternionY;
    float quaternionZ;
    float quaternionW;
} BoneQuaternion;

typedef struct _boneTranslation {
    float translationX;
    float translationY;
    float translationZ;
} BoneTranslation;

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

    // for debugging and sanity
    CENTER_OF_ROTATION_API const char* HasFailedMeshConstruction(Mesh * mesh);
    // because the message was allocated on the heap
    CENTER_OF_ROTATION_API void FreeErrorMessage(const char* message);

    // for memory management
    CENTER_OF_ROTATION_API void DestroyMesh(Mesh * mesh);

    // get debug data
    CENTER_OF_ROTATION_API int GetRestVertexCount(Mesh * mesh);
    CENTER_OF_ROTATION_API int GetRestFaceCount(Mesh * mesh);
    // CENTER_OF_ROTATION_API int GetSubdividedVertexCount(Mesh * mesh);
    // CENTER_OF_ROTATION_API int GetSubdividedFaceCount(Mesh * mesh);

    // get centers of rotation
    CENTER_OF_ROTATION_API int GetCenterCount(Mesh * mesh);
    // vertices pointer should point to an allocated Vector3[] in C#
    CENTER_OF_ROTATION_API void GetCentersOfRotation(Mesh * mesh,
        float * vertices, int vertexCount);
    CENTER_OF_ROTATION_API const char * HasFailedGettingCentersOfRotation(Mesh * mesh);

    // serialization
    CENTER_OF_ROTATION_API void SerializeMesh(Mesh * mesh, const char * path);
    CENTER_OF_ROTATION_API void ReadCenters(Mesh * mesh, const char * path);
    CENTER_OF_ROTATION_API void SerializeCenters(Mesh * mesh, const char * path);
    CENTER_OF_ROTATION_API const char * SerializationError(Mesh * mesh);

    // runtime animation
    // CENTER_OF_ROTATION_API void SetMeshVertexBuffer(Mesh * mesh, void * vertexBufferHandle);
    CENTER_OF_ROTATION_API void Animate(Mesh * mesh, BoneQuaternion * rotations,
        BoneTranslation * translations, float* transformed);
    CENTER_OF_ROTATION_API const char * AnimationError(Mesh * mesh);
}

