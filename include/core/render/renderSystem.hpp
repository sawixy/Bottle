#pragma once

#include <core/resources/render/shaderResource.hpp>
#include <core/utils/ecs/system.hpp>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

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

class RenderComponent {
protected:
    Mesh mesh;
    std::vector<resources::render::ShaderResource*> shaders;

public:
    RenderComponent() = default;
    RenderComponent(Mesh mesh, std::vector<resources::render::ShaderResource*> shaders)
        : mesh(std::move(mesh)), shaders(std::move(shaders)) {}

    virtual ~RenderComponent() = default;

    const Mesh& getMesh() const { return mesh; }
    Mesh& getMesh() { return mesh; }

    const std::vector<Vertex>& getVertices() const { return mesh.vertices; }
    std::vector<Vertex>& getVertices() { return mesh.vertices; }

    const std::vector<uint32_t>& getIndices() const { return mesh.indices; }
    std::vector<uint32_t>& getIndices() { return mesh.indices; }

    const std::vector<resources::render::ShaderResource*>& getShaders() const { return shaders; }
    std::vector<resources::render::ShaderResource*>& getShaders() { return shaders; }

    void setMesh(Mesh newMesh) { mesh = std::move(newMesh); }
    void setVertices(std::vector<Vertex> newVertices) { mesh.vertices = std::move(newVertices); }
    void setIndices(std::vector<uint32_t> newIndices) { mesh.indices = std::move(newIndices); }
    void setShaders(std::vector<resources::render::ShaderResource*> newShaders) { shaders = std::move(newShaders); }
};

class RenderSystem: public utils::System {
public:
    virtual void addComponent(std::unique_ptr<RenderComponent> comp) = 0;
    virtual std::unique_ptr<resources::render::ShaderResource> createShaderResource(
        const std::string& name,
        const std::string& path,
        resources::render::ShaderResource::ShaderType type) = 0;
    virtual std::unique_ptr<RenderComponent> createComponent(
        Mesh mesh,
        std::vector<resources::render::ShaderResource*> shaders) = 0;
    virtual void update() override = 0;
    virtual ~RenderSystem() override = default;
};

}