@echo off
cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build .
echo [101;93m DLL [0m
ls *.dll