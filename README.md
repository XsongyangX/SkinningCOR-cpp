# Skinning with centers of rotations
Made for the project in IFT 6113, at UdeM

# Research paper
See the [paper](https://la.disneyresearch.com/publication/skinning-with-optimized-cors/) by two researchers at Disney.

## Executive summary
Skinning is an algorithm operating on a mesh with a bone rig. The algorithm is supposed to deform the mesh according to the transformations made to the bones, represented by animation curves and key frames.

The advantage of using this algorithm over other ones is that it minimizes unwanted distortions on the mesh. The popular linear blend skinning (LBS) method produces the candy wrapper artifact, while the other popular dual quaternion skinning (DQS) method produces joint bulges. Though joint bulges are tolerable in some cases, they are inacceptable in high quality animation productions.

The pre-computed centers of rotations act as a bridge between the best of LBS and DQS. It eliminates all candy wrapper artifacts and joint bulge artifacts. Because the algorithm is divided into pre-computational and real-time, the first part, aka the center of rotations, can be done prior to animating, thus neutralizing the additional overhead of this algorithm. The real-time part of the algorithm is similar to DQS and LBS in time complexity.

# Implementation
The implementation in this repository is tailored to be a C++ DLL plugin for the Unity engine. However, it can still be used within C++ to create a standalone executable.

The parallel repo in C# is available [here](https://github.com/XsongyangX/Skinning-with-COR). Animation data is also in that repo.

## Required components
The following C++ components are required to build a dynamically linked library (DLL) for Unity.
* CMake and C++ toolsuite for your OS
    * This repo is made for Windows, but can still compile on Linux.
* Eigen3, a linear algebra library

The following C++ components are required to build a standalone executable for the command line.
* All of the components for the DLL
* Libigl, please edit the CMakelists accordingly

# Building

## DLL
To build the repo into a Windows DLL, you can make use of `export.bat` but remember to change the paths in the last line of the script to be where you want the DLL to be.
```bat
:: Inside the top of the repo, using a Windows Developer Powershell to get CMake
mkdir build/
cd build/
../export.bat
```

## Executable
To build the repo into an executable, you must use CMake.
```bash
# Top of the repo
mkdir binbuild/
cd binbuild/
cmake .. -G"NMake Makefiles" # it can be any generator you have
cmake --build .
```

# Running

## DLL
The DLL can only be run in conjunction with Unity. See the repository [here](https://github.com/XsongyangX/Skinning-with-COR).

## Executable
The executable's job is to find centers of rotations on a skinned mesh and produce a single deformation. You may change `main.cpp` to reflect your choices.

By default, the executable requires one argument.
```bash
$ ./skinning_COR-bin
Usage: ./skinning_COR-bin path-to-file(base name, no extension)
ex: ./skinning_COR-bin ../../logs/Beta_Joints
```

The data given to the executable must consist of 3 or 4 files of the following.
* A file of vertices, printed using the `<<` operator on `Eigen` matrices, with extension `.vertices`.
    * Ex. `Beta_Joints.vertices`
* A file of triangles, printed using the `<<` operator on `Eigen` matrices, with extension `.triangles`.
    * Ex. `Beta_Joints.triangles`
* A file of skin weights, printed in triplet format such that the first int is the bone index, the second int is the vertex index and the last float is the weight. This file has a `.weights` extension and should be accompanied by a `.weights.size` file that holds two ints: the number of bones and the number of vertices.
    * Ex. `Beta_Joints.weights` and `Beta_Joints.weights.size`
* (optional) A file of the centers of rotations, printed like the vertices. You may request the executable to generate these centers instead of getting them from a file.

All of these files can be serialized from inside the executable, using `Mesh::Serialize`. But the initial data must come from Unity's C#.

### Visualization with libigl
It is possible to visualize the mesh and its centers of rotation with the executable using libigl. But you must edit `main.cpp` to do that. There are functions in `viewer.h` that calls libigl viewer code.

# Documentation
The C++ repo uses a C interface to exchange data with C#. The interface is called `center_of_rotation_api.h` and contains all the exposed function headers of the DLL.

Every other file, except for `viewer.h`, `viewer.cpp` and `main.cpp`, contains the implementation of a small procedure in the algorithm or serialization procedures.

* `area.h` calculates the area of a triangle
* `Mesh.h` holds the state and data of the skinned mesh and the essential parts of the algorithm
* `serialize.h` contains readers and writers for mesh data
* `similarity.h` calculates a similarity function defined in the research paper
