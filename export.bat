@echo off
cmake .. -DBUILD_BINARY=OFF -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -A x64
cmake --build . --config Release
echo [101;93m DLL [0m
ls Release/*.dll
cp Release/*.dll ../../"Skinning with COR"/Assets/Plug-ins/Windows