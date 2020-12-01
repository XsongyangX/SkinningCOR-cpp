#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#include "Mesh.h"
#define CENTER_OF_ROTATION_DEBUG
#include "center_of_rotation.h"

using namespace std;
using namespace Eigen;

int main(int argc, char * argv[])
{
    float vertices [] = {
        1.0f, 2.0f, 3.0f,
        2.0f, 0.0f, -1.0f,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };
    int vertexCount = 5;

    int triangles [] = {
        1,2,3,
        1,2,3,
        0,0,0,
        0,0,0,
        0,0,0,
    };
    int triangleCount = 5;

    BoneWeight weights[] = {
        {0, 1}, {1,2}, {0, 0}, {0, 0}, {0, 0}
    };

    uint8_t bones [] = {
        1, 1, 1, 1, 1
    };
    int boneCount = 2;

    auto mesh = CreateMesh(
        vertices, vertexCount,
        triangles, triangleCount,
        weights, bones,
        boneCount
    );

    std::cout << mesh->GetCentersOfRotation() << std::endl;
    auto message = HasFailedMeshConstruction(mesh);
    std::cout << message << std::endl;
    FreeErrorMessage(message);
}