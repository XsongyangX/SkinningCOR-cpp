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
    float vertices [4][3] = {
        {1.0f, 2.0f, 0.0f},
        {2.0f, 3.0f, 9.0f},
        {-1.0f, -2.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f}
    };

    int triangles [3][3] = {
        {0,1,2},
        {2,1,3},
        {2,0,1}
    };

    BoneWeight weightPerVertex[] = { {3, 0.5f}, {1, 0.1f}, {0, 0.1f}, {2, 0.3f} };

    BoneWeight* weights [] =
    {
        weightPerVertex, weightPerVertex, weightPerVertex, weightPerVertex,
    };
    
    auto mesh = CreateMesh(vertices, 4, triangles, 3, weights, 4, 4);
}