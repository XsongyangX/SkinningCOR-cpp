#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <string>

#include "Mesh.h"
#include "serialize.h"
#define CENTER_OF_ROTATION_DEBUG
#include "center_of_rotation_api.h"

using namespace std;
using namespace Eigen;

int main(int argc, char * argv[])
{
    // float vertices [] = {
    //     1.0f, 2.0f, 3.0f,
    //     2.0f, 0.0f, -1.0f,
    //     1,2,3,
    //     1,2,3,
    //     1,2,3,
    // };
    // int vertexCount = 5;

    // int triangles [] = {
    //     1,2,3,
    //     1,2,3,
    //     1,2,3,
    //     1,2,3,
    //     1,2,3,
    // };
    // int triangleCount = 5;

    // BoneWeight weights[] = {
    //     {0, 1}, {0, 0.5f}, {1, 0.5f}, {0, 1}, {0, 0.5f}, {1, 0.5f}, {1, 1}
    // };

    // uint8_t bones [] = {
    //     1, 2, 1, 2, 1
    // };
    // int uniqueBones = 2;

    // auto mesh = CreateMesh(
    //     vertices, vertexCount,
    //     triangles, triangleCount,
    //     weights, bones,
    //     uniqueBones
    // );
    
    // from disk
    auto mesh = ReadMesh(string("C:/Users/Song/Documents/UDEM/ift6113/project/skinning_cor/logs/Beta_Surface"));

    auto message = HasFailedMeshConstruction(&mesh);
    std::cout << message << std::endl;
    FreeErrorMessage(message);

    std::cout << mesh.GetCentersOfRotation() << std::endl;
    auto centerErrorMessage = HasFailedGettingCentersOfRotation(&mesh);
    std::cout << centerErrorMessage << std::endl;
    FreeErrorMessage(centerErrorMessage);

    // auto weights = ReadWeights(string("C:/Users/Song/Documents/UDEM/ift6113/project/skinning_cor/logs/Beta_Joints"));
    // ifstream file(string("C:/Users/Song/Documents/UDEM/ift6113/project/skinning_cor/logs/Beta_Joints") + string(".weights.size"));

    // string line;
    // while (getline(file, line))
    // {
    //     cout << line << endl;
    // }
}