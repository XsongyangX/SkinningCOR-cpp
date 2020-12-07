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
    if (argc != 2)
    {
        cerr << "Usage: ./skinning_COR-bin path-to-file(base name, no extension)" << endl;
        cerr << "ex: ./skinning_COR-bin ../../logs/Beta_Joints" << endl;
        exit(1);
    }
    
    // read mesh from disk
    auto path = string(argv[1]);
    Mesh * mesh;
    try
    {
        mesh = ReadMesh(path);
    }
    catch (const exception & e)
    {
        cerr << e.what() << endl;
        cerr << "Does the file exist and is readable?" << endl;
        exit(1);
    }
    auto message = HasFailedMeshConstruction(mesh);
    std::cout << message << std::endl;
    FreeErrorMessage(message);

    // read centers from disk
    ReadCenters(mesh, path.c_str());
    auto readingCenters = HasFailedGettingCentersOfRotation(mesh);
    std::cout << readingCenters << endl;
    FreeErrorMessage(readingCenters);

    // output centers count
    std::cout << mesh->GetCenterCount() << std::endl;
    auto centerErrorMessage = HasFailedGettingCentersOfRotation(mesh);
    std::cout << centerErrorMessage << std::endl;
    FreeErrorMessage(centerErrorMessage);

    // compute a deformation
    const Eigen::Vector4f identity = Eigen::Quaternionf::Identity().coeffs();
    vector<BoneTransformation> transformations(mesh->GetBoneCount());
    for_each(transformations.begin(), transformations.end(),
        [&](BoneTransformation b)
        {
            b = BoneTransformation{
                identity.w(), identity.x(), identity.y(), identity.z(),
                0,0,0};
        }
    );

    vector<float> transformed(3 * mesh->GetRestVertexCount());
    for_each(transformed.begin(), transformed.end(), 
        [](float element)
        {
            element = 0;
        }
    );

    Animate(mesh, transformations.data(), transformed.data());
    auto animationErrorMessage = AnimationError(mesh);
    std::cout << animationErrorMessage << std::endl;
    FreeErrorMessage(animationErrorMessage);


    for (int i = 0; i < 10; i+= 3)
    {
        cout << transformed[i] 
            << ", " << transformed[i + 1] << ", " 
            << transformed[i + 2] << endl;
    }

    delete mesh;
}