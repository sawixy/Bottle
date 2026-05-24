#pragma once 

#include "core/utils/ecs/component.hpp"
#include <core/utils/locator.hpp>
#include <core/resources/render/shaderResource.hpp>
#include <core/utils/ecs/system.hpp>
#include <vector>
#include <stdint.h>

#ifdef DEBUG
#include <iostream>
#endif

namespace bottle::core::render {
    
struct Vertex {
public:
    float x, y, z;
    float r, g, b;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class RenderComponentInner {
protected:
    Mesh mesh;
    std::vector<resources::render::ShaderResource*> shaders;

public:
    RenderComponentInner(Mesh mesh, std::vector<resources::render::ShaderResource*> shaders) : mesh(mesh), shaders(std::move(shaders)) {}

    Mesh& getMesh() { return mesh; }
    std::vector<resources::render::ShaderResource*>& getShaders() { return shaders; }

    virtual void setMesh(Mesh newMesh) { mesh = newMesh; }

    virtual ~RenderComponentInner() {
        for (resources::render::ShaderResource* shader : shaders) {
            shader->unload();
        }
    }
};

class RenderComponent : public utils::Component {
private:
    RenderComponentInner* inner;

public:
    RenderComponent(Mesh mesh, std::vector<resources::render::ShaderResource*> shaders, std::vector<std::pair<std::string, size_t>> uniforms = {});

    Mesh& getMesh() { return inner->getMesh(); }
    std::vector<resources::render::ShaderResource*>& getShaders() { return inner->getShaders(); }
    RenderComponentInner* getInner() { return inner; }

    void setMesh(Mesh newMesh) { inner->setMesh(std::move(newMesh)); }

    ~RenderComponent() {
#ifdef DEBUG
        std::cout << "Destroying RenderComponent" << std::endl;
#endif
        delete inner;
    }
};    

}