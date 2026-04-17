#include <meshes.hpp>

namespace bottle {

core::render::Mesh createTriangleMesh() {
    core::render::Mesh mesh;
    mesh.vertices = {
        {0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
        {0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
        {-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f}
    };
    mesh.indices = {0, 1, 2};
    return mesh;
}

}