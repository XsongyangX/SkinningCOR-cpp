cmake_minimum_required(VERSION 3.1)

# windows VCPKG
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

project(skinning_COR CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB src "*.h" "*.cpp")
list(REMOVE_ITEM src main.cpp)

include(GNUInstallDirs)

set(Eigen3_DIR "$ENV{VCPKG_ROOT}/installed/x86-windows/share/eigen3")
find_package (Eigen3 REQUIRED NO_MODULE)

#add_library(${PROJECT_NAME} SHARED ${src})
add_library(${PROJECT_NAME} ${src})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)


add_executable(${PROJECT_NAME}-bin main.cpp)
target_link_libraries(${PROJECT_NAME}-bin ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}-bin PRIVATE .)

# change the executable name
#set_target_properties(${PROJECT_NAME}-bin
#        PROPERTIES OUTPUT_NAME ${PROJECT_NAME})