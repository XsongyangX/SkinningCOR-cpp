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
        1,2,3,
        1,2,3,
        1,2,3,
    };
    int vertexCount = 5;

    int triangles [] = {
        1,2,3,
        1,2,3,
        1,2,3,
        1,2,3,
        1,2,3,
    };
    int triangleCount = 5;

    BoneWeight weights[] = {
        {0, 1}, {0, 0.5f}, {1, 0.5f}, {0, 1}, {0, 0.5f}, {1, 0.5f}, {1, 1}
    };

    uint8_t bones [] = {
        1, 2, 1, 2, 1
    };
    int uniqueBones = 2;

    auto mesh = CreateMesh(
        vertices, vertexCount,
        triangles, triangleCount,
        weights, bones,
        uniqueBones
    );

    std::cout << mesh->GetCentersOfRotation() << std::endl;
    auto message = HasFailedMeshConstruction(mesh);
    std::cout << message << std::endl;
    FreeErrorMessage(message);

    std::cout << "Similarity " << mesh->Similarity(atoi(argv[1]), atoi(argv[2])) << std::endl;
}