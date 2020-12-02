#include "area.h"

#include <Eigen/Dense>

float area(const Eigen::Vector3f & vertex1, const Eigen::Vector3f & vertex2, 
    const Eigen::Vector3f & vertex3)
{
    Eigen::Vector3f side1 = vertex1 - vertex2;
    Eigen::Vector3f side2 = vertex2 - vertex3;
    return 0.5f * side1.cross(side2).norm();
}