cmake_minimum_required(VERSION 3.1)

# windows VCPKG
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

project(skinning_COR CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB src "*.h" "*.cpp")
list(FILTER src EXCLUDE REGEX "main.cpp|viewer.*")

set(Eigen3_DIR "$ENV{VCPKG_ROOT}/installed/x86-windows/share/eigen3")
find_package (Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} ${src})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)

option(BUILD_BINARY "Whether to generate an executable to test the dll" ON)

# libigl
option(LIBIGL_WITH_OPENGL            "Use OpenGL"         ON)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"           ON)

if(BUILD_BINARY)
# location of the libigl cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR} "C:/Users/Song/Documents/UDEM/ift6113/hw/ift6113_2020/hw3_cpp/cmake/")
find_package(LIBIGL REQUIRED QUIET)
add_executable(${PROJECT_NAME}-bin main.cpp viewer.cpp viewer.h)
target_link_libraries(${PROJECT_NAME}-bin ${PROJECT_NAME} igl::core igl::opengl_glfw)
target_include_directories(${PROJECT_NAME}-bin PRIVATE .)
endif()

# change the executable name
#set_target_properties(${PROJECT_NAME}-bin
#        PROPERTIES OUTPUT_NAME ${PROJECT_NAME})