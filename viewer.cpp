#include "viewer.h"

#include <igl/opengl/glfw/Viewer.h>

#include "Mesh.h"

void view_centers_of_rotation(Mesh &mesh)
{
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(mesh.GetVertices().cast<double>().eval(), mesh.GetFaces());
    
    // points
    // viewer.data().point_size = 10;
    viewer.data().add_points(mesh.GetCentersOfRotation().cast<double>(), Eigen::RowVector3d(1,0,0));

    bool show_mesh = true;
    viewer.callback_key_down = [&](igl::opengl::glfw::Viewer &viewer, unsigned char key, int) -> bool {
        switch (key)
        {
        default:
            return false;
        case ' ':
        {
            if (show_mesh)
                viewer.data().set_visible(false);
            else
                viewer.data().set_visible(true);
            return true;
        }
        }
    };
    viewer.launch();
}

void view_points(Mesh &mesh)
{
    igl::opengl::glfw::Viewer viewer;
    viewer.data().add_points(mesh.GetCentersOfRotation().cast<double>(), Eigen::RowVector3d(1,0,0));
    viewer.launch();
}