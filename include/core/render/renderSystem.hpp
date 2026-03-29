#pragma once

#include <core/utils/ecs/system.hpp>
#include <vector>

namespace bottle::core::render {

struct Vertex {
public:
    float x, y, z;
};

class Mesh {
private:
    std::vector<Vertex> vertices;
};

class RenderSystem: public utils::System {
public:
    virtual void update() override = 0;
    virtual ~RenderSystem() override = 0;
};

}