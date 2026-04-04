#pragma once

#include <core/utils/ecs/system.hpp>
#include <vector>

namespace bottle::core::render {

struct Vertex {
public:
    float x, y, z;
    float r, g, b;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};

class RenderSystem: public utils::System {
public:
    virtual void update() override = 0;
    virtual ~RenderSystem() override = 0;
};

}